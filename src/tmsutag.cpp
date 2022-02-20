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
    return tmp.replace(" ", "\\ ").replace("=", "\\=");
}

QString TMSUTag::unescapeString(const QString &str)
{
    QString tmp = str;
    return tmp.replace("\\ ", " ").replace("\\=", "=");
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

// Tags are unique based on name, so the value isn't used in these functions.
// If tag with a value is being added, the tag name needs to be checked first.
bool TMSUTag::operator==(const TMSUTag &other) const
{
    return getName() == other.getName();
}

uint qHash(const TMSUTag &c) noexcept
{
    return qHash(c.getName());
}
