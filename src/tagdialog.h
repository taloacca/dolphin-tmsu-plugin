#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include <QDialog>

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(QString url, QList< QString > tags, QWidget* parent = nullptr);
};
#endif // TAGDIALOG_H
