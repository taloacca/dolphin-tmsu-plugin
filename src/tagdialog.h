#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"
#include "tagusage.h"
#include "taglineedit.h"
#include "tagvalidator.h"
#include "tagusagelistmodel.h"
#include "additionaltagswidget.h"
#include "tagflowlayout.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QMap>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

typedef QMap< QString, TMSUTagSet > FileTagSetMap;

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const FileTagSetMap &fileTagSetMap, const TagUsageList &tagUsageList, const QStringList &valueUsageList, QWidget* parent = nullptr);

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

    TagLineEdit *m_newTagName;
    TagLineEdit *m_newTagValue;
    QCompleter *m_tagCompleter;
    QCompleter *m_valueCompleter;
    TagUsageListModel *m_tagListModel;
    QPushButton *m_addButton;
    QScrollArea *m_scrollArea;
    TagFlowLayout *m_tagLayout;
    TagValidator *m_validator;
    QDialogButtonBox *m_buttonBox;
};
#endif // TAGDIALOG_H
