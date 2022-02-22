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

class TMSUPlugin : public KAbstractFileItemActionPlugin
{
    Q_OBJECT

public:
    TMSUPlugin(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);

    QList< QAction* > actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget) override;

private:
    TMSUTagSet getTagsForFile(const QString &file);
    void setFileTagSetMap(const FileTagSetMap &oldFileTagSetMap, const FileTagSetMap &newFileTagSetMap);
    void removeTagsForFile(const QString &file, const TMSUTagSet &tagSet);
    void addTagsForFiles(const FileTagSetMap &tagAddMap);

private Q_SLOTS:
    void editTags();
    void copyTags();
    void pasteTags();

private:
    QString m_workingDirectory;
    TMSUTagSet m_copiedTags;
};
#endif // TMSUPLUGIN_H
