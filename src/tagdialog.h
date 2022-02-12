#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include <QDialog>

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const QString &url, const QList< QString > &tags, const QList< QPair< QString, int > > &tagSummaryList, QWidget* parent = nullptr);
};
#endif // TAGDIALOG_H
