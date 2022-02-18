#include "tagdialog.h"

#include <QString>
#include <QVBoxLayout>
#include <QLabel>

TagDialog::TagDialog(const QString &url, const TMSUTagList &tags, const QList< QPair< QString, int > > &tagSummaryList, QWidget* parent) :
    QDialog(parent)
{
    this->setWindowTitle(QStringLiteral("TMSU Tags"));

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    for(const auto &tag : tags)
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
