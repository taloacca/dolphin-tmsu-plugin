#include "taglineedit.h"

#include <QCompleter>

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

void TagLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    // If this delay isn't here, the popup gets drawn at the wrong place initially when TagDialog is created.
    timer.singleShot(100, this, &TagLineEdit::forceEmptyCompletion);
}

void TagLineEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    forceEmptyCompletion();
}
