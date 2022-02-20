#include "tagdialog.h"
#include "tagusagelistmodel.h"
#include "tagwidget.h"

#include <QString>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCompleter>
#include <QPushButton>

TagDialog::TagDialog(const FileTagListMap &fileTagListMap, const TagUsageList &tagUsageList, QWidget* parent) :
    QDialog(parent), m_fileTagListMap(fileTagListMap)
{
    QString titleFilename;
    if(fileTagListMap.size() > 1)
    {
        titleFilename = "multiple files";
    }
    else
    {
        QFileInfo info(fileTagListMap.keyValueBegin()->first);
        titleFilename = info.fileName();
    }
    this->setWindowTitle(QStringLiteral("Edit TMSU tags for ") + titleFilename);

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
    m_newTagName->setMinimumWidth(400);
    m_newTagName->setClearButtonEnabled(true);
    m_newTagName->setCompleter(completer);
    mainLayout->addWidget(m_newTagName);

    connect(m_newTagName, &QLineEdit::returnPressed, this, &TagDialog::confirmTag);

    m_tagLayout = new FlowLayout;
    mainLayout->addLayout(m_tagLayout);
    for(auto it = fileTagListMap.keyValueBegin(); it != fileTagListMap.keyValueEnd(); ++it)
    {
        for(const auto &tag : it->second)
        {
            TagWidget *tagWidget = new TagWidget(tag, this);
            m_tagLayout->addWidget(tagWidget);
        }
    }
}

void TagDialog::confirmTag()
{
    QString newTag = m_newTagName->text();
    if(!isValidTagName(newTag))
    {
        // TODO: pop up tooltip?
        return;
    }
    m_newTagName->clear();

    TagWidget *tagWidget = new TagWidget(TMSUTag(newTag), this);
    m_tagLayout->addWidget(tagWidget);
}

bool TagDialog::isValidTagName(const QString &tagName)
{
    bool invalid = false;
    static QList< QString > badNames {".", "..", "and", "AND", "or", "OR", "not", "NOT", "eq", "EQ", "ne", "NE", "lt", "LT", "gt", "GT", "le", "LE", "ge", "GE"};
    for(const auto &bad : badNames)
    {
        if(tagName == bad)
        {
            invalid = true;
            break;
        }
    }
    invalid |= tagName.contains("\\");
    invalid |= tagName.contains("/");
    return !invalid;
}
