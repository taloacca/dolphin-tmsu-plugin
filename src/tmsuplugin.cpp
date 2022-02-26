#include "tmsuplugin.h"

#include "tagusage.h"

#include <QFileInfo>
#include <QTextCodec>
#include <QMessageBox>
#include <QProgressDialog>

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

    QString actionText = (fileItemInfos.urlList().size() > 1 ? QStringLiteral("Edit Common TMSU Tags") : QStringLiteral("Edit TMSU Tags"));
    QAction* editAction = new QAction(QIcon::fromTheme(QStringLiteral("tag")), actionText, parentWidget);
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

FileTagSetMap TMSUPlugin::getTagsForFiles(const QStringList &files)
{
    if(files.isEmpty())
        return FileTagSetMap();

    FileTagSetMap tags;

    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
    QStringList argList = {"tags", "-1", "--name", "never"};
    argList.append(files);
    process.start("tmsu", argList);

    int filesCount = 0;
    tags[files[filesCount]] = TMSUTagSet();

    QProgressDialog progress(QStringLiteral("Fetching tags for files..."), "", 0, files.size());
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(1500);
    progress.setCancelButton(nullptr);

    while(process.waitForReadyRead())
    {
        char buffer[512];
        while (process.readLine(buffer, sizeof(buffer)) > 0)
        {
            QString tagName = QTextCodec::codecForLocale()->toUnicode(buffer);
            if(tagName == "\n")
            {
                filesCount++;
                progress.setValue(filesCount);
                if(filesCount >= files.size())
                {
                    QMessageBox messageBox;
                    messageBox.critical(0, "Error", "Error fetching TMSU tags!");
                    return FileTagSetMap();
                }
                tags[files[filesCount]] = TMSUTagSet();
                continue;
            }
            // Remove newline
            tagName.chop(1);
            tags[files[filesCount]].insert(TMSUTag::fromEscapedString(tagName));
        }
    }
    if(checkTmsuProcessError(process))
    {
        return FileTagSetMap();
    }
    if(tags.size() != files.size())
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Error fetching TMSU tags!");
        return FileTagSetMap();
    }

    return tags;
}

TagUsageList TMSUPlugin::getTagUsage()
{
    TagUsageList tagUsage;
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
            tagUsage.append(TagUsage(tagName, tagCount));
        }
    }
    if(checkTmsuProcessError(process))
    {
        return TagUsageList();
    }

    return tagUsage;
}

QStringList TMSUPlugin::getValueUsage()
{
    QStringList valueUsage;
    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
    process.start("tmsu", {"values", "-1"});
    while(process.waitForReadyRead())
    {
        char buffer[512];
        while (process.readLine(buffer, sizeof(buffer)) > 0)
        {
            QString valueName = QTextCodec::codecForLocale()->toUnicode(buffer);
            // Remove newline
            valueName.chop(1);

            valueUsage.append(valueName);
        }
    }
    if(checkTmsuProcessError(process))
    {
        return QStringList();
    }

    return valueUsage;
}

void TMSUPlugin::setFileTagSetMap(const FileTagSetMap &oldFileTagSetMap, const FileTagSetMap &newFileTagSetMap)
{
    FileTagSetMap tagRemoveMap;
    FileTagSetMap tagAddMap;
    for(auto it = newFileTagSetMap.keyValueBegin(); it != newFileTagSetMap.keyValueEnd(); ++it)
    {
        TMSUTagSet oldTags = oldFileTagSetMap[it->first];

        TMSUTagSet tagsToRemove = oldTags - it->second;
        TMSUTagSet tagsToAdd = it->second - oldTags;

        if(!tagsToRemove.isEmpty())
            tagRemoveMap[it->first] = tagsToRemove;
        if(!tagsToAdd.isEmpty())
            tagAddMap[it->first] = tagsToAdd;
    }

    if(!tagRemoveMap.isEmpty())
        removeTagsForFiles(tagRemoveMap);

    if(!tagAddMap.isEmpty())
        addTagsForFiles(tagAddMap);
}

void TMSUPlugin::removeTagsForFiles(const FileTagSetMap &tagRemoveMap)
{
    // TMSU doesn't have a stdin option for 'untag', so instead just group the removes by tag, since it's faster than calling an untag per-file in most cases
    QMap< QString, QSet< QString > > tagToFileSetMap;
    for(auto it = tagRemoveMap.keyValueBegin(); it != tagRemoveMap.keyValueEnd(); ++it)
    {
        for(const auto &tag : it->second)
        {
            tagToFileSetMap[tag.toEscapedString()].insert(it->first);
        }
    }

    QProgressDialog progress(QStringLiteral("Removing tags..."), "", 0, tagToFileSetMap.size());
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(1500);
    progress.setCancelButton(nullptr);
    int tagsDone = 0;

    for(auto it = tagToFileSetMap.keyValueBegin(); it != tagToFileSetMap.keyValueEnd(); ++it)
    {
        progress.setValue(tagsDone++);
        QProcess process;
        process.setWorkingDirectory(m_workingDirectory);
        QStringList argList = {"untag", "--tags", it->first};
        argList.append(it->second.values());

        process.start("tmsu", argList);
        process.waitForFinished();
        if(checkTmsuProcessError(process))
        {
            break;
        }
    }
}

void TMSUPlugin::addTagsForFiles(const FileTagSetMap &tagAddMap)
{
    QProcess process;
    process.setWorkingDirectory(m_workingDirectory);
    process.start("tmsu", {"tag", "-"});

    // Progress dialog is less useful here.  Writing to the process is so fast that almost all of the time is spent waiting for the process to finish, so there
    // isn't a great way to determine overall progress.
    QProgressDialog progress(QStringLiteral("Adding tags for files..."), "", 0, 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);
    if(tagAddMap.size() > 100)
    {
        progress.setMinimumDuration(0);
        progress.setValue(0);
    }
    for(auto it = tagAddMap.keyValueBegin(); it != tagAddMap.keyValueEnd(); ++it)
    {
        QList< QString > escapedTags;
        for(const auto &tag : it->second)
        {
            escapedTags.append(tag.toEscapedString());
        }

        QString tagString = escapedTags.join(" ");
        process.write("\"" + it->first.toUtf8() + "\" " + tagString.toUtf8() + "\n");
    }
    process.closeWriteChannel();
    process.waitForFinished();
    checkTmsuProcessError(process);
}

bool TMSUPlugin::checkTmsuProcessError(const QProcess &process)
{
    if((process.exitStatus() != QProcess::NormalExit) || (process.error() != QProcess::UnknownError) || (process.exitCode() != 0))
    {
        QString errorStr;
        if((process.exitStatus() != QProcess::NormalExit))
            errorStr = QStringLiteral("Process crashed");
        else if((process.error() != QProcess::UnknownError))
            errorStr = process.errorString();
        else if((process.exitCode() != 0))
            errorStr = QStringLiteral(" Process returned ") + QString::number(process.exitCode());
        QMessageBox messageBox;
        messageBox.critical(0, "Error", QStringLiteral("Error running TMSU command: ") + errorStr);
        return true;
    }
    return false;
}

void TMSUPlugin::editTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();
    QStringList files;
    for(const auto &url : urls)
    {
        files.append(url.toLocalFile());
    }

    FileTagSetMap oldFileTagSetMap = getTagsForFiles(files);
    if(oldFileTagSetMap.isEmpty())
        return;

    TagUsageList tagUsageList = getTagUsage();
    QStringList valueUsageList = getValueUsage();

    TagDialog tagDialog(oldFileTagSetMap, tagUsageList, valueUsageList);
    if(tagDialog.exec() == QDialog::Accepted)
    {
        FileTagSetMap newFileTagSetMap = tagDialog.getFileTagSetMap();
        setFileTagSetMap(oldFileTagSetMap, newFileTagSetMap);
    }
}

void TMSUPlugin::copyTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();
    Q_ASSERT(urls.size() == 1);

    FileTagSetMap fileTagSetMap = getTagsForFiles(QStringList(urls[0].toLocalFile()));
    if(fileTagSetMap.size() > 0)
        m_copiedTags = *(fileTagSetMap.begin());
}

void TMSUPlugin::pasteTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();

    FileTagSetMap newFileTagSetMap;
    for(const auto &url : urls)
    {
        newFileTagSetMap[url.toLocalFile()] = m_copiedTags;
    }

    // Since pasting won't delete tags, existing tags don't matter
    FileTagSetMap emptyFileTagSetMap;
    setFileTagSetMap(emptyFileTagSetMap, newFileTagSetMap);
}

#include "tmsuplugin.moc"
