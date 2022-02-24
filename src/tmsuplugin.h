#ifndef TMSUPLUGIN_H
#define TMSUPLUGIN_H

#include <KPluginFactory>
#include <KAbstractFileItemActionPlugin>
#include <KFileItemListProperties>
#include <KFileItem>

#include "tmsutag.h"
#include "tagdialog.h"

#include <QAction>
#include <QObject>
#include <QVariant>
#include <QProcess>

class TMSUPlugin : public KAbstractFileItemActionPlugin
{
    Q_OBJECT

public:
    TMSUPlugin(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);

    QList< QAction* > actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget) override;

private:
    FileTagSetMap getTagsForFiles(const QStringList &files);
    TagUsageList getTagUsage();
    void setFileTagSetMap(const FileTagSetMap &oldFileTagSetMap, const FileTagSetMap &newFileTagSetMap);
    void removeTagsForFile(const QString &file, const TMSUTagSet &tagSet);
    void addTagsForFiles(const FileTagSetMap &tagAddMap);
    bool checkTmsuProcessError(const QProcess &process);

private Q_SLOTS:
    void editTags();
    void copyTags();
    void pasteTags();

private:
    QString m_workingDirectory;
    TMSUTagSet m_copiedTags;
};
#endif // TMSUPLUGIN_H
