#include "tmsuplugin.h"

#include "tagusage.h"

#include <QFileInfo>
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
    m_workingDirectory = QFileInfo(fileItemInfos.items().first().localPath()).path();

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
    TMSUTagSet tags;

    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
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
        messageBox.critical(0, "Error", "Error running TMSU command!");
        return TMSUTagSet();
    }

    return tags;
}

void TMSUPlugin::setFileTagSetMap(const FileTagSetMap &fileTagSetMap)
{
    QMap< QString, TMSUTagSet > tagRemoveMap;
    QMap< QString, TMSUTagSet > tagAddMap;
    for(auto it = fileTagSetMap.keyValueBegin(); it != fileTagSetMap.keyValueEnd(); ++it)
    {
        TMSUTagSet oldTags = getTagsForFile(it->first);

        TMSUTagSet tagsToRemove = oldTags - it->second;
        TMSUTagSet tagsToAdd = it->second - oldTags;

        if(!tagsToRemove.isEmpty())
            tagRemoveMap[it->first] = tagsToRemove;
        if(!tagsToAdd.isEmpty())
            tagAddMap[it->first] = tagsToAdd;
    }

    for(auto it = tagRemoveMap.keyValueBegin(); it != tagRemoveMap.keyValueEnd(); ++it)
    {
        if(!it->second.isEmpty())
            removeTagsForFile(it->first, it->second);
    }

    if(!tagAddMap.isEmpty())
        addTagsForFiles(tagAddMap);
}

void TMSUPlugin::removeTagsForFile(const QString &file, const TMSUTagSet &tags)
{
    QList< QString > escapedTags;
    for(const auto &tag : tags)
    {
        escapedTags.append(tag.toEscapedString());
    }

    QString tagString = escapedTags.join(" ");

    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
    process.start("tmsu", {"untag", file, "--tags", tagString});
    if ((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Error running TMSU command!");
        return;
    }
    process.waitForFinished();
}

void TMSUPlugin::addTagsForFiles(const QMap< QString, TMSUTagSet > &tagAddMap)
{
    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
    process.start("tmsu", {"tag", "-"});
    for(auto it = tagAddMap.keyValueBegin(); it != tagAddMap.keyValueEnd(); ++it)
    {
        QList< QString > escapedTags;
        for(const auto &tag : it->second)
        {
            escapedTags.append(tag.toEscapedString());
        }

        QString tagString = escapedTags.join(" ");
        process.write("\"" + it->first.toUtf8() + "\" \"" + tagString.toUtf8() + "\"\n");
    }
    process.closeWriteChannel();
    process.waitForFinished();
    if ((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Error running TMSU command!");
        return;
    }
}

void TMSUPlugin::editTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();

    FileTagSetMap fileTagSetMap;
    TagUsageList tagUsageList;
    {
        QProcess process;
        process.setWorkingDirectory(m_workingDirectory);
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
            messageBox.critical(0, "Error", "Error running TMSU command!");
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
        setFileTagSetMap(newFileTagSetMap);
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

    FileTagSetMap fileTagSetMap;
    for(const auto &url : urls)
    {
        fileTagSetMap[url.toLocalFile()] = m_copiedTags;
    }
    setFileTagSetMap(fileTagSetMap);
}

#include "tmsuplugin.moc"
