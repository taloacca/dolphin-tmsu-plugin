#include "tagflowlayout.h"
#include "additionaltagswidget.h"
#include "tagwidget.h"

#include <algorithm>

TagFlowLayout::TagFlowLayout(QWidget* parent) : FlowLayout(parent)
{
}

void TagFlowLayout::addItem(QLayoutItem *item)
{
    FlowLayout::addItem(item);
    std::sort(itemList.begin(), itemList.end(),
        [](QLayoutItem *a, QLayoutItem *b) -> bool
    {
        AdditionalTagsWidget *aAdditionalTagsWidget = qobject_cast<AdditionalTagsWidget *>(a->widget());
        AdditionalTagsWidget *bAdditionalTagsWidget = qobject_cast<AdditionalTagsWidget *>(b->widget());
        // Ensure that the AdditionalTagsWidget is always last in the list, if it exists
        if(aAdditionalTagsWidget)
            return false;
        if(bAdditionalTagsWidget)
            return true;

        TagWidget *aWidget = qobject_cast<TagWidget *>(a->widget());
        TagWidget *bWidget = qobject_cast<TagWidget *>(b->widget());
        return bWidget->getTag() > aWidget->getTag();
    });
}
