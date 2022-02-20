#include "tagwidget.h"

#include <QHBoxLayout>
#include <QLabel>

TagWidget::TagWidget(const TMSUTag &tag, QWidget* parent) : QFrame(parent), m_tmsuTag(tag)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QLabel *nameLabel = new QLabel(tag.getName(), this);
    mainLayout->addWidget(nameLabel);

    if(tag.hasValue())
    {
        QLabel *valueLabel = new QLabel(tag.getValue(), this);
        valueLabel->setFrameShape(QFrame::StyledPanel);
        valueLabel->setFrameShadow(QFrame::Raised);
        mainLayout->addWidget(valueLabel);
    }

    m_deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("edit-delete")), "", this);
    m_deleteButton->setFlat(true);
    mainLayout->addWidget(m_deleteButton);

    connect(m_deleteButton, &QPushButton::clicked, this, &TagWidget::deleteButtonPressed);
}
