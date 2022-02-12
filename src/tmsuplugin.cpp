#include "tmsuplugin.h"

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

    QAction* action = new QAction(QIcon::fromTheme(QStringLiteral("tag")), QStringLiteral("Edit TMSU Tags"), parentWidget);
    action->setProperty("urls", QVariant::fromValue(fileItemInfos.urlList()));
    action->setProperty("parentWidget", QVariant::fromValue(parentWidget));
    connect(action, &QAction::triggered, this, &TMSUPlugin::editTags);
    actions += action;

    return actions;
}

void TMSUPlugin::editTags()
{
    const QList< QUrl > urls = sender()->property("urls").value< QList< QUrl > >();
}
#include "tmsuplugin.moc"
