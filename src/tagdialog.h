#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"
#include "tagusage.h"

#include <QDialog>
#include <QMap>
#include <QLineEdit>

typedef QMap< QString, QList< TMSUTag > > FileTagListMap;

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const FileTagListMap &fileTagMap, const TagUsageList &tagUsageList, QWidget* parent = nullptr);

private Q_SLOTS:
    void confirmTag();

private:
    FileTagListMap m_fileTagListMap;
    QLineEdit *m_newTagName;
};
#endif // TAGDIALOG_H
