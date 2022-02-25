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

void TagLineEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    forceEmptyCompletion();
}
