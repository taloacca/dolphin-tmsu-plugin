#include "tagwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

TagWidget::TagWidget(const TMSUTag &tag, QWidget* parent) : QFrame(parent), m_tmsuTag(tag)
{
    this->setFrameShape(QFrame::StyledPanel);
    this->setFrameShadow(QFrame::Raised);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QLabel *nameLabel = new QLabel(tag.getName());
    mainLayout->addWidget(nameLabel);

    if(tag.hasValue())
    {
        // QLabel *valueEqualsLabel = new QLabel(QStringLiteral(" = "));
        // mainLayout->addWidget(valueEqualsLabel);

        QLabel *valueLabel = new QLabel(tag.getValue());
        valueLabel->setFrameShape(QFrame::StyledPanel);
        valueLabel->setFrameShadow(QFrame::Raised);
        mainLayout->addWidget(valueLabel);
    }

    // QPushButton* clearButton = new QPushButton(QIcon::fromTheme(QStringLiteral("edit-clear")), "", nameLabel);
    // QPushButton* clearButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), "", nameLabel);
    // QPushButton* clearButton = new QPushButton(QIcon::fromTheme(QStringLiteral("window-close")), "", nameLabel);
    // QPushButton* clearButton = new QPushButton(QIcon::fromTheme(QStringLiteral("dialog-cancel")), "", nameLabel);
    QPushButton* clearButton = new QPushButton(QIcon::fromTheme(QStringLiteral("edit-delete")), "", nameLabel);
    clearButton->setFlat(true);
    mainLayout->addWidget(clearButton);
}
