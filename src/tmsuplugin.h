#ifndef TMSUPLUGIN_H
#define TMSUPLUGIN_H

#include <KPluginFactory>
#include <KAbstractFileItemActionPlugin>
#include <KFileItemListProperties>

#include "tmsutag.h"

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
    void setTagsForFile(const QString &file, const TMSUTagSet &tags);

private Q_SLOTS:
    void editTags();
    void copyTags();
    void pasteTags();

private:
    TMSUTagSet m_copiedTags;
};
#endif // TMSUPLUGIN_H
