#ifndef TMSUPLUGIN_H
#define TMSUPLUGIN_H

#include <KPluginFactory>
#include <KAbstractFileItemActionPlugin>
#include <KFileItemListProperties>

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
    QList< QString > getTagsForFile(const QString &file);

private Q_SLOTS:
    void editTags();
    void copyTags();
    void pasteTags();

private:
    QList< QString > m_copiedTags;
};
#endif // TMSUPLUGIN_H
