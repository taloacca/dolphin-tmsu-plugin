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

    if(input.contains("/") || input.contains("\\"))
        return QValidator::Invalid;

    return QValidator::Acceptable;
}
