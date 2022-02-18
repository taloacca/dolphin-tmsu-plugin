#include "tagusagelistmodel.h"

#include <algorithm>

TagUsageListModel::TagUsageListModel(const TagUsageList &list, QObject *parent) : QAbstractListModel(parent), m_list(list)
{
    std::sort(m_list.begin(), m_list.end(), TagUsage::tagUsageComparator);
}

int TagUsageListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_list.count();
}

QVariant TagUsageListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return m_list.at(index.row()).getName();

    return QVariant();
}
