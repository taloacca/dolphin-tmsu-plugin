#include "tagdialog.h"

#include <QString>
#include <QVBoxLayout>
#include <QLabel>

TagDialog::TagDialog(const QString &url, const QList< QString > &tags, const QList< QPair< QString, int > > &tagSummaryList, QWidget* parent) :
    QDialog(parent)
{
    this->setWindowTitle(QStringLiteral("TMSU Tags"));

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    for(const auto &tag : tags)
    {
        QLabel *tagLabel = new QLabel(tag);
        mainLayout->addWidget(tagLabel);
    }
}
