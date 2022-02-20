#ifndef TAGVALIDATOR_H
#define TAGVALIDATOR_H

#include <QValidator>

class TagValidator : public QValidator
{
    Q_OBJECT

public:
    explicit TagValidator(QObject* parent = nullptr);

    QValidator::State validate(QString &input, int &pos) const override;
};
#endif // TAGVALIDATOR_H
