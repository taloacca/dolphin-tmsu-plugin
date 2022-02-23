#include "tagdialog.h"
#include "tagwidget.h"

#include <QString>
#include <QFileInfo>
#include <QLabel>

TagDialog::TagDialog(const FileTagSetMap &fileTagSetMap, const TagUsageList &tagUsageList, QWidget* parent) :
    QDialog(parent), m_isEditingMultipleFiles(fileTagSetMap.size() > 1), m_fileTagSetMap(fileTagSetMap), m_additionalTagsWidget(nullptr)
{
    QString titleFilename;
    if(m_isEditingMultipleFiles)
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

    m_tagListModel = new TagUsageListModel(tagUsageList, this);

    m_tagCompleter = new QCompleter(m_tagListModel, this);
    m_tagCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    m_tagCompleter->setCompletionMode(QCompleter::PopupCompletion);
    m_tagCompleter->setCaseSensitivity(Qt::CaseInsensitive);

    m_validator = new TagValidator(this);

    m_editLineLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_editLineLayout);

    m_newTagName = new QLineEdit(this);
    m_newTagName->setPlaceholderText(QStringLiteral("Tag Name"));
    m_newTagName->setMinimumWidth(200);
    m_newTagName->setClearButtonEnabled(true);
    m_newTagName->setCompleter(m_tagCompleter);
    m_newTagName->setValidator(m_validator);
    m_editLineLayout->addWidget(m_newTagName);

    m_newTagValue = new QLineEdit(this);
    m_newTagValue->setPlaceholderText(QStringLiteral("Tag Value (Optional)"));
    m_newTagValue->setMinimumWidth(200);
    m_newTagValue->setClearButtonEnabled(true);
    m_newTagValue->setValidator(m_validator);
    m_editLineLayout->addWidget(m_newTagValue);

    m_addButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")), "", this);
    m_addButton->setAutoDefault(true);
    m_addButton->setDefault(true);
    connect(m_addButton, &QPushButton::clicked, this, &TagDialog::confirmTag);
    m_editLineLayout->addWidget(m_addButton);

    // TODO: Fix min size of flowlayout.  maybe also max size with scrollbar?
    m_tagLayout = new FlowLayout();
    m_mainLayout->addLayout(m_tagLayout);

    QSet< TMSUTag > commonTagSet;
    for(auto it = fileTagSetMap.keyValueBegin(); it != fileTagSetMap.keyValueEnd(); ++it)
    {
        if(it == fileTagSetMap.keyValueBegin())
        {
            commonTagSet = it->second;
        }
        else
        {
            commonTagSet.intersect(it->second);
        }
    }

    for(auto it = fileTagSetMap.keyValueBegin(); it != fileTagSetMap.keyValueEnd(); ++it)
    {
        m_uncommonTagSet.unite(it->second - commonTagSet);
    }

    QList< TMSUTag > sorted = commonTagSet.values();
    std::sort(sorted.begin(), sorted.end(), TMSUTag::tmsuTagComparator);
    for(const auto &tag : sorted)
    {
        addTagWidget(tag);
    }

    if(!m_uncommonTagSet.isEmpty())
    {
        m_additionalTagsWidget = new AdditionalTagsWidget(this);
        updateAdditionTagsTooltip();

        connect(m_additionalTagsWidget, &AdditionalTagsWidget::deleteButtonPressed, this, &TagDialog::removeAllUncommonTags);
        m_tagLayout->addWidget(m_additionalTagsWidget);
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
    QString newTagValue = m_newTagValue->text();
    m_newTagName->clear();
    m_newTagValue->clear();

    if(newTagName.isEmpty())
        return;

    TMSUTag newTag = TMSUTag(newTagName, newTagValue);

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

void TagDialog::removeAllUncommonTags()
{
    for(const auto &tag : m_uncommonTagSet)
    {
        for(auto it = m_fileTagSetMap.keyValueBegin(); it != m_fileTagSetMap.keyValueEnd(); ++it)
        {
            it->second.remove(tag);
        }
    }
    m_uncommonTagSet.clear();
    if(m_additionalTagsWidget)
    {
        m_tagLayout->removeWidget(m_additionalTagsWidget);
        m_additionalTagsWidget->deleteLater();
        m_additionalTagsWidget = nullptr;
    }
}

void TagDialog::addTagWidget(const TMSUTag &tag)
{
    TagWidget *tagWidget = new TagWidget(tag, this);
    connect(tagWidget, &TagWidget::deleteButtonPressed, this, &TagDialog::removeTag);
    m_tagLayout->addWidget(tagWidget);

    m_uncommonTagSet.remove(tag);
    if(m_isEditingMultipleFiles && m_uncommonTagSet.isEmpty())
    {
        if(m_additionalTagsWidget)
        {
            m_tagLayout->removeWidget(m_additionalTagsWidget);
            m_additionalTagsWidget->deleteLater();
            m_additionalTagsWidget = nullptr;
        }
    }
    else
    {
        updateAdditionTagsTooltip();
    }
}

void TagDialog::updateAdditionTagsTooltip()
{
    if(m_additionalTagsWidget)
    {
        QStringList tagStringList;
        for(const auto &tag : m_uncommonTagSet.values())
        {
            tagStringList.append(tag.toString());
        }
        m_additionalTagsWidget->setToolTip(tagStringList.join(", "));
    }
}
