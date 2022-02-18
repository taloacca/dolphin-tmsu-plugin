#include "tmsuplugin.h"
#include "tmsupluginsettings.h"

#include "tagdialog.h"
#include "tagusage.h"

#include <QProcess>
#include <QTextCodec>
#include <QPair>
#include <QMap>

K_PLUGIN_CLASS_WITH_JSON(TMSUPlugin, "tmsuplugin.json")

TMSUPlugin::TMSUPlugin(QObject* parent, const KPluginMetaData &metaData, const QVariantList &args) :
    KAbstractFileItemActionPlugin(parent)
{
    Q_UNUSED(metaData);
    Q_UNUSED(args);
    TMSUTag tag(QStringLiteral("test"), QStringLiteral("value"));
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

TMSUTagList TMSUPlugin::getTagsForFile(const QString &file)
{
    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    TMSUTagList tags;

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
            tags += TMSUTag::fromEscapedString(tagName);
        }
    }

    return tags;
}

void TMSUPlugin::setTagsForFile(const QString &file, const TMSUTagList &tags)
{
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
    process.start("tmsu", {"untag", "--all", file});
    process.waitForFinished();
    process.start("tmsu", {"tag", file, "--tags", tagString});
    process.waitForFinished();
}

void TMSUPlugin::editTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();

    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    QMap< QString, QList< TMSUTag > > fileTagMap;
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
                int tagCount = tagSummary.mid(lastSpaceIdx-1).toInt();

                // TODO: this won't work if a tag has leading or trailing whitespace, but it needs to be trimmed since the TMSU command adds whitespace for formatting
                QString tagName = tagSummary.left(lastSpaceIdx).trimmed();
                tagUsageList.append(TagUsage(tagName, tagCount));
            }
        }
    }

    for(const auto &url : urls)
    {
        fileTagMap[url.toLocalFile()] = getTagsForFile(url.toLocalFile());
    }

    TagDialog tagDialog(fileTagMap, tagUsageList);
    tagDialog.exec();
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
        setTagsForFile(url.toLocalFile(), m_copiedTags);
    }
}

#include "tmsuplugin.moc"
