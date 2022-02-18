#include "tagdialog.h"
#include "tagusagelistmodel.h"

#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCompleter>

TagDialog::TagDialog(const QMap< QString, QList< TMSUTag > > &fileTagMap, const TagUsageList &tagUsageList, QWidget* parent) :
    QDialog(parent)
{
    this->setWindowTitle(QStringLiteral("TMSU Tags"));

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    // TODO: need to decide how to handle multiple sets of tags on multiple files.  Show common tags only?

    // QList< QString > tagList;
    // for(auto it = tagInfoMap.keyValueBegin(); it != tagInfoMap.keyValueEnd(); ++it)
    // {
    //     tagList.append(it->first);
    // }

    TagUsageListModel *model = new TagUsageListModel(tagUsageList);

    QCompleter *completer = new QCompleter(model);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    // completer->setFilterMode(Qt::MatchContains);

    QLineEdit *newTagName = new QLineEdit();
    newTagName->setMinimumWidth(150);
    newTagName->setClearButtonEnabled(true);
    newTagName->setCompleter(completer);
    mainLayout->addWidget(newTagName);

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
