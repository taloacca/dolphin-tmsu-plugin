#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"
#include "tagusage.h"
#include "tagvalidator.h"
#include "tagusagelistmodel.h"
#include "additionaltagswidget.h"
#include "flowlayout.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QMap>
#include <QLineEdit>
#include <QPushButton>

typedef QMap< QString, TMSUTagSet > FileTagSetMap;

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const FileTagSetMap &fileTagSetMap, const TagUsageList &tagUsageList, QWidget* parent = nullptr);

    const FileTagSetMap &getFileTagSetMap() const { return m_fileTagSetMap; }

private Q_SLOTS:
    void confirmTag();
    void removeTag();
    void removeAllUncommonTags();

private:
    void addTagWidget(const TMSUTag &tag);
    void updateAdditionTagsTooltip();

    bool m_isEditingMultipleFiles;
    FileTagSetMap m_fileTagSetMap;
    TMSUTagSet m_uncommonTagSet;

    AdditionalTagsWidget *m_additionalTagsWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_editLineLayout;

    QLineEdit *m_newTagName;
    QLineEdit *m_newTagValue;
    QCompleter *m_tagCompleter;
    TagUsageListModel *m_tagListModel;
    QPushButton *m_addButton;
    FlowLayout *m_tagLayout;
    TagValidator *m_validator;
    QDialogButtonBox *m_buttonBox;
};
#endif // TAGDIALOG_H
