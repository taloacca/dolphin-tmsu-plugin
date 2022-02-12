#include "tmsuplugin.h"
#include "tmsupluginsettings.h"

#include "tagdialog.h"

#include <QProcess>
#include <QTextCodec>
#include <QPair>

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

    TMSUPluginSettings* settings = TMSUPluginSettings::self();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TMSU_DB", settings->dbPath());

    QList< QPair< QString, int > > tagSummaryList;
    {
        QProcess process;
        process.setProcessEnvironment(env);
        process.start("tmsu", {"info", "-u", "--color", "never"});
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
                QString tagName = tagSummary.left(lastSpaceIdx).trimmed();
                tagSummaryList += QPair< QString, int >(tagName, tagCount);
            }
        }
    }

    for(const auto &url : urls)
    {
        QList< QString > tags;
        QProcess process;
        process.setProcessEnvironment(env);
        process.start("tmsu", {"tags", "-1", "-n", "never", url.toLocalFile()});
        while(process.waitForReadyRead())
        {
            char buffer[512];
            while (process.readLine(buffer, sizeof(buffer)) > 0)
            {
                QString tagName = QTextCodec::codecForLocale()->toUnicode(buffer);
                // Remove newline
                tagName.chop(1);
                tags += tagName;
            }
        }

        // TagDialog tagDialog(url.toLocalFile(), tags);
        // tagDialog.exec();
    }
}
#include "tmsuplugin.moc"
