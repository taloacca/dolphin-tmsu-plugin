#include "tagusagelistmodel.h"

TagUsageListModel::TagUsageListModel(const TagUsageList &list, QObject *parent) : QAbstractListModel(parent), m_list(list)
{
}
