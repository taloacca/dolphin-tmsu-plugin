#ifndef TMSUTAG_H
#define TMSUTAG_H

#include <QString>
#include <QSet>
#include <QHash>

class TMSUTag
{
public:
    TMSUTag(const QString &name, const QString &value = "");

    static TMSUTag fromEscapedString(const QString &str);
    static QString escapeString(const QString &str);
    static QString unescapeString(const QString &str);

    QString toEscapedString() const;
    QString toString() const;

    const QString &getName() const { return m_name; }

    bool hasValue() const { return !m_value.isEmpty(); }
    const QString &getValue() const { return m_value; }

    static bool tmsuTagComparator(const TMSUTag &t1, const TMSUTag &t2);
    bool operator==(const TMSUTag &other) const;

private:
    QString m_name;
    QString m_value;
};

uint qHash(const TMSUTag &c, uint seed) noexcept;

typedef QSet< TMSUTag > TMSUTagSet;

#endif // TMSUTAG_H
