#include "taglineedit.h"

#include <QCompleter>
#include <QKeyEvent>

TagLineEdit::TagLineEdit(QWidget* parent) : QLineEdit(parent)
{
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(textChanged()), Qt::QueuedConnection);
}

void TagLineEdit::textChanged()
{
    forceEmptyCompletion();
}

void TagLineEdit::forceEmptyCompletion()
{
    if(completer() && text().isEmpty())
    {
        completer()->setCompletionPrefix("");
        completer()->complete();
    }
}

void TagLineEdit::keyPressEvent(QKeyEvent *event)
{
    QLineEdit::keyPressEvent(event);
    switch(event->key())
    {
        case Qt::Key_Backspace:
        case Qt::Key_Down:
        {
            forceEmptyCompletion();
        }
        default: break;
    }
}
