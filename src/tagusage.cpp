#include "tagusage.h"

TagUsage::TagUsage(const QString &name, const int &count) : m_name(name), m_count(count)
{
}

bool TagUsage::tagUsageComparator(const TagUsage &t1, const TagUsage &t2)
{
    return t1.m_count > t2.m_count;
}
