#ifndef TAGUSAGELISTMODEL_H
#define TAGUSAGELISTMODEL_H

#include "tagusage.h"

#include <QAbstractItemModel>
#include <QList>

class TagUsageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    TagUsageListModel(const TagUsageList &list, QObject *parent);

private:
    TagUsageList m_list;
};
#endif // TAGUSAGELISTMODEL_H
