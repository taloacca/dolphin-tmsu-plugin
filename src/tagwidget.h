#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include "tmsutag.h"

#include <QFrame>
#include <QPushButton>

class TagWidget : public QFrame
{
    Q_OBJECT

public:
    explicit TagWidget(const TMSUTag &tag, QWidget* parent = nullptr);

    const TMSUTag &getTag() const { return m_tmsuTag; }

signals:
    void deleteButtonPressed();

private:
    TMSUTag m_tmsuTag;
    QPushButton *m_deleteButton;
};
#endif // TAGWIDGET_H
