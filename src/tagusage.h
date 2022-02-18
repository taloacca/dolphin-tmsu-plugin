#ifndef TAGUSAGE_H
#define TAGUSAGE_H

#include <QString>
#include <QList>

class TagUsage
{
public:
    TagUsage(const QString &name, const int &count);

    static bool tagUsageComparator(const TagUsage &t1, const TagUsage &t2);

    QString getName() const { return m_name; }

private:
    QString m_name;
    int m_count;
};

typedef QList< TagUsage > TagUsageList;

#endif // TAGUSAGE_H
