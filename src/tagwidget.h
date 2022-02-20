#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include "tmsutag.h"

#include <QFrame>

class TagWidget : public QFrame
{
    Q_OBJECT

public:
    explicit TagWidget(const TMSUTag &tag, QWidget* parent = nullptr);

private:
    TMSUTag m_tmsuTag;
};
#endif // TAGWIDGET_H
