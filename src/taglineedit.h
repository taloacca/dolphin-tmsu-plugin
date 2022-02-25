#ifndef TAGLINEEDIT_H
#define TAGLINEEDIT_H

#include <QLineEdit>
#include <QTimer>

class TagLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit TagLineEdit(QWidget* parent = nullptr);

private Q_SLOTS:
    void textChanged();

private:
    void forceEmptyCompletion();
    void focusInEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

    QTimer timer;
};
#endif // TAGLINEEDIT_H
