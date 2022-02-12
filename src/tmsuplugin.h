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

private Q_SLOTS:
    void editTags();
};
#endif // TMSUPLUGIN_H
