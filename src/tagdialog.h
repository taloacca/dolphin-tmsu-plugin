#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"
#include "tagusage.h"
#include "flowlayout.h"

#include <QDialog>
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

private:
    FileTagSetMap m_fileTagSetMap;
    FlowLayout *m_tagLayout;
    QLineEdit *m_newTagName;
};
#endif // TAGDIALOG_H
