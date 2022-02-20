#include "tagdialog.h"
#include "tagusagelistmodel.h"
#include "tagwidget.h"
#include "tagvalidator.h"

#include <QString>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCompleter>
#include <QPushButton>

TagDialog::TagDialog(const FileTagSetMap &fileTagSetMap, const TagUsageList &tagUsageList, QWidget* parent) :
    QDialog(parent), m_fileTagSetMap(fileTagSetMap)
{
    QString titleFilename;
    if(fileTagSetMap.size() > 1)
    {
        titleFilename = "multiple files";
    }
    else
    {
        QFileInfo info(fileTagSetMap.keyValueBegin()->first);
        titleFilename = info.fileName();
    }
    this->setWindowTitle(QStringLiteral("Edit TMSU tags for ") + titleFilename);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    // TODO: need to decide how to handle multiple sets of tags on multiple files.  Show common tags only?

    TagUsageListModel *model = new TagUsageListModel(tagUsageList, this);

    QCompleter *completer = new QCompleter(model, this);
    // TODO: Is the model really considered sorted?
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    // TODO: hook up TagWidget delete buttons
    // TODO: add ok/cancel buttons on this dialog

    TagValidator *validator = new TagValidator(this);

    m_newTagName = new QLineEdit(this);
    m_newTagName->setMinimumWidth(400);
    m_newTagName->setClearButtonEnabled(true);
    m_newTagName->setCompleter(completer);
    m_newTagName->setValidator(validator);
    mainLayout->addWidget(m_newTagName);

    connect(m_newTagName, &QLineEdit::returnPressed, this, &TagDialog::confirmTag);

    // TODO: this messes things up if 'this' is FlowLayout parent
    m_tagLayout = new FlowLayout;
    mainLayout->addLayout(m_tagLayout);
    for(auto it = fileTagSetMap.keyValueBegin(); it != fileTagSetMap.keyValueEnd(); ++it)
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
    QString newTagName = m_newTagName->text();
    m_newTagName->clear();
    if(newTagName.isEmpty())
        return;

    TMSUTag newTag(newTagName);

    bool isTagNew = false;
    for(auto it = m_fileTagSetMap.keyValueBegin(); it != m_fileTagSetMap.keyValueEnd(); ++it)
    {
        if(!it->second.contains(newTag))
        {
            it->second.insert(newTag);
            isTagNew = true;
        }
    }

    if(isTagNew)
    {
        TagWidget *tagWidget = new TagWidget(newTag, this);
        m_tagLayout->addWidget(tagWidget);
    }
}
