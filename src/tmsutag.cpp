#include "tmsutag.h"

#include <QRegularExpression>
#include <QList>

TMSUTag::TMSUTag(const QString &name, const QString &value) : m_name(name), m_value(value)
{
}

TMSUTag TMSUTag::fromEscapedString(const QString &str)
{
    if(str.contains("="))
    {
        QRegularExpression expr("[^\\\\]\\K=");
        QList< QString > list = str.split(expr);
        if(list.size() > 1)
        {
            return TMSUTag(unescapeString(list[0]), unescapeString(list[1]));
        }
    }
    return TMSUTag(unescapeString(str));
}

QString TMSUTag::escapeString(const QString &str)
{
    QString tmp = str;
    return tmp.replace(" ", "\\ ")
              .replace("=", "\\=")
              .replace("\"", "\\\"")
              .replace("'", "\\'");
}

QString TMSUTag::unescapeString(const QString &str)
{
    QString tmp = str;
    return tmp.replace("\\ ", " ")
              .replace("\\=", "=")
              .replace("\\\"", "\"")
              .replace("\\'", "'");
}

QString TMSUTag::toEscapedString() const
{
    QString ret;
    ret = escapeString(m_name);
    if(hasValue())
    {
        ret += "=";
        ret += escapeString(m_value);
    }
    return ret;
}

QString TMSUTag::toString() const
{
    QString ret;
    ret = m_name;
    if(hasValue())
    {
        ret += "=";
        ret += m_value;
    }
    return ret;
}

bool TMSUTag::tmsuTagComparator(const TMSUTag &t1, const TMSUTag &t2)
{
    if(t1.getName() == t2.getName())
        return t1.getValue() < t2.getValue();
    return t1.getName() < t2.getName();
}

bool TMSUTag::operator==(const TMSUTag &other) const
{
    return getName() == other.getName() &&
           getValue() == other.getValue();
}

bool TMSUTag::operator>(const TMSUTag &other) const
{
    if(getName() != other.getName())
        return getName() > other.getName();
    return getValue() > other.getValue();
}

uint qHash(const TMSUTag &c, uint seed) noexcept
{
    seed = qHash(c.getName(), seed);
    seed = qHash(c.getValue(), seed);
    return seed;
}
