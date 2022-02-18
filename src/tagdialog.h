#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "tmsutag.h"

#include <QDialog>
#include <QMap>

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(const QMap< QString, QList< TMSUTag > > &fileTagMap, const QMap< QString, int > &tagInfoMap, QWidget* parent = nullptr);
};
#endif // TAGDIALOG_H
