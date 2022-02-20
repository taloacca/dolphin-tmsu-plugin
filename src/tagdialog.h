#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"
#include "tagusage.h"
#include "tagvalidator.h"
#include "tagusagelistmodel.h"
#include "flowlayout.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QMap>
#include <QLineEdit>

typedef QMap< QString, QSet< TMSUTag > > FileTagSetMap;

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const FileTagSetMap &fileTagSetMap, const TagUsageList &tagUsageList, QWidget* parent = nullptr);

private Q_SLOTS:
    void confirmTag();
    void removeTag();

private:
    void addTagWidget(const TMSUTag &tag);

    FileTagSetMap m_fileTagSetMap;

    QVBoxLayout *m_layout;
    QDialogButtonBox *m_buttonBox;
    FlowLayout *m_tagLayout;
    QLineEdit *m_newTagName;
    TagValidator *m_validator;
    QCompleter *m_completer;
    TagUsageListModel *m_listModel;
};
#endif // TAGDIALOG_H
