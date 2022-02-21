#ifndef ADDITIONALTAGSWIDGET_H
#define ADDITIONALTAGSWIDGET_H

#include <QFrame>
#include <QPushButton>

class AdditionalTagsWidget : public QFrame
{
    Q_OBJECT

public:
    explicit AdditionalTagsWidget(QWidget* parent = nullptr);

signals:
    void deleteButtonPressed();

private:
    QPushButton *m_deleteButton;
};
#endif // ADDITIONALTAGSWIDGET_H
