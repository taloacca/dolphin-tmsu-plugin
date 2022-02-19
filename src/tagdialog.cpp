#include "tagdialog.h"
#include "tagusagelistmodel.h"

#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QCompleter>

TagDialog::TagDialog(const FileTagListMap &fileTagListMap, const TagUsageList &tagUsageList, QWidget* parent) :
    QDialog(parent), m_fileTagListMap(fileTagListMap)
{
    this->setWindowTitle(QStringLiteral("TMSU Tags"));

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    // TODO: need to decide how to handle multiple sets of tags on multiple files.  Show common tags only?

    TagUsageListModel *model = new TagUsageListModel(tagUsageList);

    QCompleter *completer = new QCompleter(model);
    // Is the model really considered sorted?
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    m_newTagName = new QLineEdit();
    m_newTagName->setMinimumWidth(150);
    m_newTagName->setClearButtonEnabled(true);
    m_newTagName->setCompleter(completer);
    mainLayout->addWidget(m_newTagName);

    connect(m_newTagName, &QLineEdit::editingFinished, this, &TagDialog::confirmTag);

    for(auto it = fileTagListMap.keyValueBegin(); it != fileTagListMap.keyValueEnd(); ++it)
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

void TagDialog::confirmTag()
{
}
