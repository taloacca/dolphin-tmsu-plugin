#ifndef TAGLINEEDIT_H
#define TAGLINEEDIT_H

#include <QLineEdit>

class TagLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit TagLineEdit(QWidget* parent = nullptr);

private Q_SLOTS:
    void textChanged();

private:
    void forceEmptyCompletion();
    void keyPressEvent(QKeyEvent *e) override;
};
#endif // TAGLINEEDIT_H
