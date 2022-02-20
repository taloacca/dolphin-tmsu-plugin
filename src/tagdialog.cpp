#include "tagdialog.h"
#include "tagwidget.h"

#include <QString>
#include <QFileInfo>
#include <QLabel>

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
    setWindowTitle(QStringLiteral("Edit TMSU tags for ") + titleFilename);

    m_mainLayout = new QVBoxLayout(this);

    // TODO: need to decide how to handle multiple sets of tags on multiple files.  Show common tags only?
    // TODO: handle setting a value on a tag

    m_listModel = new TagUsageListModel(tagUsageList, this);

    m_completer = new QCompleter(m_listModel, this);
    m_completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);

    m_validator = new TagValidator(this);

    m_editLineLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_editLineLayout);

    m_newTagName = new QLineEdit(this);
    m_newTagName->setMinimumWidth(400);
    m_newTagName->setClearButtonEnabled(true);
    m_newTagName->setCompleter(m_completer);
    m_newTagName->setValidator(m_validator);
    connect(m_newTagName, &QLineEdit::returnPressed, this, &TagDialog::confirmTag);
    m_editLineLayout->addWidget(m_newTagName);

    // Ensure that m_newTagName gets cleared correctly after choosing something from the completer
    connect(m_completer, SIGNAL(activated(const QString&)), m_newTagName, SLOT(clear()), Qt::QueuedConnection);

    m_addButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), "", this);
    m_addButton->setAutoDefault(true);
    m_addButton->setDefault(true);
    connect(m_addButton, &QPushButton::clicked, this, &TagDialog::confirmTag);
    m_editLineLayout->addWidget(m_addButton);

    m_tagLayout = new FlowLayout();
    m_mainLayout->addLayout(m_tagLayout);
    for(auto it = fileTagSetMap.keyValueBegin(); it != fileTagSetMap.keyValueEnd(); ++it)
    {
        QList< TMSUTag > sorted = it->second.values();
        std::sort(sorted.begin(), sorted.end(), TMSUTag::tmsuTagComparator);
        for(const auto &tag : sorted)
        {
            addTagWidget(tag);
        }
    }

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    m_buttonBox->setCenterButtons(true);
    m_mainLayout->addWidget(m_buttonBox);
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
        addTagWidget(newTag);
    }
}

void TagDialog::removeTag()
{
    TagWidget *tagWidget = qobject_cast<TagWidget *>(sender());
    if(tagWidget)
    {
        {
            TMSUTag tagVal = tagWidget->getTag();
            for(auto it = m_fileTagSetMap.keyValueBegin(); it != m_fileTagSetMap.keyValueEnd(); ++it)
            {
                it->second.remove(tagVal);
            }
        }

        m_tagLayout->removeWidget(tagWidget);
        tagWidget->deleteLater();
    }
}

void TagDialog::addTagWidget(const TMSUTag &tag)
{
    TagWidget *tagWidget = new TagWidget(tag, this);
    connect(tagWidget, &TagWidget::deleteButtonPressed, this, &TagDialog::removeTag);
    m_tagLayout->addWidget(tagWidget);
}
