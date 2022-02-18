#ifndef TAGUSAGELISTMODEL_H
#define TAGUSAGELISTMODEL_H

#include "tagusage.h"

#include <QAbstractItemModel>
#include <QList>

class TagUsageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    TagUsageListModel(const TagUsageList &list, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    TagUsageList m_list;
};
#endif // TAGUSAGELISTMODEL_H
