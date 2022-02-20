#include "tagvalidator.h"

TagValidator::TagValidator(QObject* parent) : QValidator(parent)
{
}

QValidator::State TagValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    static QList< QString > badNames {".", "..", "and", "AND", "or", "OR", "not", "NOT", "eq", "EQ", "ne", "NE", "lt", "LT", "gt", "GT", "le", "LE", "ge", "GE"};
    for(const auto &bad : badNames)
    {
        if(input == bad)
        {
            return QValidator::Intermediate;
        }
    }

    if(input.contains("/"))
        return QValidator::Invalid;

    // Allow a backslash only before an equals sign and nowhere else.  Backslashes aren't allowed in TMSU tag names, but since
    // the equals sign is used to separate tag/value strings in TagDialog, "\=" can be used to put the equals sign in the tag name or value.
    if(input.contains("\\"))
    {
        QRegularExpression backslashNotBeforeEquals("\\\\[^=]");
        if(input.contains(backslashNotBeforeEquals))
            return QValidator::Invalid;
        QRegularExpression endingBackslash("\\\\$");
        if(input.contains(endingBackslash))
            return QValidator::Intermediate;
    }
    // Only allow one unescaped equals sign.  As stated above, the escaped version must be used to put the character in tag/value strings.
    QRegularExpression unescapedEquals("[^\\\\]=");
    if(input.count(unescapedEquals) > 1)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}
