#include "tagdialog.h"

#include <QString>
#include <QVBoxLayout>
#include <QLabel>

TagDialog::TagDialog(const QMap< QString, QList< TMSUTag > > &fileTagMap, const QMap< QString, int > &tagInfoMap, QWidget* parent) :
    QDialog(parent)
{
    this->setWindowTitle(QStringLiteral("TMSU Tags"));

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    for(auto it = fileTagMap.keyValueBegin(); it != fileTagMap.keyValueEnd(); ++it)
    {
        QLabel *fileLabel = new QLabel(QStringLiteral("Tags for file ") + it->first);
        mainLayout->addWidget(fileLabel);

        for(const auto &tag : it->second)
        {
            QLabel *nameLabel = new QLabel(QStringLiteral("Tag: ") + tag.getName());
            mainLayout->addWidget(nameLabel);

            if(tag.hasValue())
            {
                QLabel *valueLabel = new QLabel(QStringLiteral("Value: ") + tag.getValue());
                mainLayout->addWidget(valueLabel);
            }
        }
    }
}
