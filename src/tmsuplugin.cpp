#include "tmsuplugin.h"
#include "tmsupluginsettings.h"

#include "tagdialog.h"
#include "tagusage.h"

#include <QProcess>
#include <QTextCodec>
#include <QPair>
#include <QMap>
#include <QMessageBox>

K_PLUGIN_CLASS_WITH_JSON(TMSUPlugin, "tmsuplugin.json")

TMSUPlugin::TMSUPlugin(QObject* parent, const KPluginMetaData &metaData, const QVariantList &args) :
    KAbstractFileItemActionPlugin(parent)
{
    Q_UNUSED(metaData);
    Q_UNUSED(args);
}

QList< QAction* > TMSUPlugin::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList< QAction* > actions;

    QAction* editAction = new QAction(QIcon::fromTheme(QStringLiteral("tag")), QStringLiteral("Edit TMSU Tags"), parentWidget);
    editAction->setProperty("urls", QVariant::fromValue(fileItemInfos.urlList()));
    editAction->setProperty("parentWidget", QVariant::fromValue(parentWidget));
    connect(editAction, &QAction::triggered, this, &TMSUPlugin::editTags);
    actions += editAction;

    if(fileItemInfos.urlList().size() == 1)
    {
        QAction* copyAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), QStringLiteral("Copy TMSU Tags"), parentWidget);
        copyAction->setProperty("urls", QVariant::fromValue(fileItemInfos.urlList()));
        copyAction->setProperty("parentWidget", QVariant::fromValue(parentWidget));
        connect(copyAction, &QAction::triggered, this, &TMSUPlugin::copyTags);
        actions += copyAction;
    }

    if(!m_copiedTags.empty())
    {
        QAction* pasteAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), QStringLiteral("Paste TMSU Tags"), parentWidget);
        pasteAction->setProperty("urls", QVariant::fromValue(fileItemInfos.urlList()));
        pasteAction->setProperty("parentWidget", QVariant::fromValue(parentWidget));
        connect(pasteAction, &QAction::triggered, this, &TMSUPlugin::pasteTags);
        actions += pasteAction;
    }

    return actions;
}

TMSUTagSet TMSUPlugin::getTagsForFile(const QString &file)
{
    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    TMSUTagSet tags;

    QProcess process;
    process.setProcessEnvironment(env);
    process.start("tmsu", {"tags", "-1", "--name", "never", file});
    while(process.waitForReadyRead())
    {
        char buffer[512];
        while (process.readLine(buffer, sizeof(buffer)) > 0)
        {
            QString tagName = QTextCodec::codecForLocale()->toUnicode(buffer);
            // Remove newline
            tagName.chop(1);
            tags.insert(TMSUTag::fromEscapedString(tagName));
        }
    }
    if ((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Couldn't run TMSU command!");
        return TMSUTagSet();
    }

    return tags;
}

void TMSUPlugin::setNewTagsForFile(const QString &file, const TMSUTagSet &newTags)
{
    TMSUTagSet oldTags = getTagsForFile(file);

    TMSUTagSet tagsToRemove = oldTags - newTags;
    TMSUTagSet tagsToAdd = newTags - oldTags;

    if(!tagsToRemove.isEmpty())
        applyTagsForFile(file, tagsToRemove, false);
    if(!tagsToAdd.isEmpty())
        applyTagsForFile(file, tagsToAdd, true);
}

void TMSUPlugin::applyTagsForFile(const QString &file, const TMSUTagSet &tags, const bool adding)
{
    QString subcommand = adding ? "tag" : "untag";
    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    QList< QString > escapedTags;
    for(auto &tag : tags)
    {
        escapedTags.append(tag.toEscapedString());
    }

    QString tagString = escapedTags.join(" ");

    QProcess process;
    process.setProcessEnvironment(env);
    process.start("tmsu", {subcommand, file, "--tags", tagString});
    if ((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Couldn't run TMSU command!");
        return;
    }
    process.waitForFinished();
}

void TMSUPlugin::editTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();

    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    FileTagSetMap fileTagSetMap;
    TagUsageList tagUsageList;
    {
        QProcess process;
        process.setProcessEnvironment(env);
        process.start("tmsu", {"info", "--usage", "--color", "never"});
        int lineIdx = 0;
        while(process.waitForReadyRead())
        {
            char buffer[512];
            while (process.readLine(buffer, sizeof(buffer)) > 0)
            {
                // Skip unnecessary output that isn't about tag usage
                if(lineIdx < 4)
                {
                    lineIdx++;
                    continue;
                }

                QString tagSummary = QTextCodec::codecForLocale()->toUnicode(buffer);
                // Remove newline
                tagSummary.chop(1);
                int lastSpaceIdx = tagSummary.lastIndexOf(' ');
                int tagCount = tagSummary.mid(lastSpaceIdx+1).toInt();

                // TODO: this won't work if a tag has leading or trailing whitespace, but it needs to be trimmed since the TMSU command adds whitespace for formatting
                QString tagName = tagSummary.left(lastSpaceIdx).trimmed();
                tagUsageList.append(TagUsage(tagName, tagCount));
            }
        }
        if ((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
        {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "Couldn't run TMSU command!");
            return;
        }
    }

    for(const auto &url : urls)
    {
        fileTagSetMap[url.toLocalFile()] = getTagsForFile(url.toLocalFile());
    }

    TagDialog tagDialog(fileTagSetMap, tagUsageList);
    if(tagDialog.exec() == QDialog::Accepted)
    {
        FileTagSetMap newFileTagSetMap = tagDialog.getFileTagSetMap();
        for(auto it = newFileTagSetMap.keyValueBegin(); it != newFileTagSetMap.keyValueEnd(); ++it)
        {
            setNewTagsForFile(it->first, it->second);
        }
    }
}

void TMSUPlugin::copyTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();
    Q_ASSERT(urls.size() == 1);

    m_copiedTags = getTagsForFile(urls[0].toLocalFile());
}

void TMSUPlugin::pasteTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();

    for(const auto &url : urls)
    {
        setNewTagsForFile(url.toLocalFile(), m_copiedTags);
    }
}

#include "tmsuplugin.moc"
