#ifndef TAGFLOWLAYOUT_H
#define TAGFLOWLAYOUT_H

#include "qt/flowlayout.h"

class TagFlowLayout : public FlowLayout
{
    Q_OBJECT

public:
    explicit TagFlowLayout(QWidget* parent = nullptr);

    void addItem(QLayoutItem *item) override;
};
#endif // TAGFLOWLAYOUT_H
