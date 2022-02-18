#ifndef TMSUTAG_H
#define TMSUTAG_H

#include <QString>
#include <QList>

class TMSUTag
{
public:
    TMSUTag(const QString &name, const QString &value = "");

    static TMSUTag fromEscapedString(const QString &str);
    static QString escapeString(const QString &str);
    static QString unescapeString(const QString &str);

    QString toEscapedString() const;

    const QString &getName() const { return m_name; }

    bool hasValue() const { return !m_value.isEmpty(); }
    const QString &getValue() const { return m_value; }

private:
    QString m_name;
    QString m_value;
};

typedef QList< TMSUTag > TMSUTagList;

#endif // TMSUTAG_H
