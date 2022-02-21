#include "additionaltagswidget.h"

#include <QHBoxLayout>
#include <QLabel>

AdditionalTagsWidget::AdditionalTagsWidget(QWidget* parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QLabel *nameLabel = new QLabel(QStringLiteral("<b><i>Additional Uncommon Tags...</i></b>"), this);
    nameLabel->setTextFormat(Qt::RichText);
    mainLayout->addWidget(nameLabel);

    m_deleteButton = new QPushButton(QIcon::fromTheme(QStringLiteral("edit-delete")), "", this);
    m_deleteButton->setFlat(true);
    mainLayout->addWidget(m_deleteButton);

    connect(m_deleteButton, &QPushButton::clicked, this, &AdditionalTagsWidget::deleteButtonPressed);
}
