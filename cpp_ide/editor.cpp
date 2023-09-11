/*
 * File: editor.cpp
 * ----------------
 * This program implements the editor.h interface to implement
 * the editor class, which is a subclass of QPlainTextEdit.
 * The main part is adapted from the official examples of Qt.
 * It adds a line number area and the function of highlighting
 * the current line to the editor. It also overrides some functions
 * to help achieve the code completion feature.
 */


#include "editor.h"

#include <QPainter>
#include <QTextBlock>

//class constructor

editor::editor(QWidget *parent) : QPlainTextEdit(parent)
{
    //line number area and highlight of current line
    lineNumberArea = new LineNumberArea(this);

    /* Connect the slots to signals in QPlainTextEdit
     * and calculate the line number area width and
     * highlight the first line when the editor is created
     */
    connect(this, &editor::blockCountChanged, this, &editor::updateLineNumberAreaWidth);
    connect(this, &editor::updateRequest, this, &editor::updateLineNumberArea);
    connect(this, &editor::cursorPositionChanged, this, &editor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

}

//calculate the width of the LineNumberArea widget
int editor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}


//update the width of the line number area
void editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


//Invoked when the editors viewport has been scrolled
void editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}


//resize the line number area when the size of the editor changes
void editor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}


//Highlight the current line (the line containing the cursor)
//when the cursor position changes
void editor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


//called from LineNumberArea whenever it receives a paint event
void editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    //paint the widget's background.
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    //paint the line numbers in the extra area for each line
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    //get the top and bottom y-coordinate of the first text block,
    //and adjust them by the height of the current text block in each iteration
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}








//Accept a completer and set it up in the editor
void editor::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &editor::insertCompletion);
}

//getter function
QCompleter *editor::completer() const
{
    return c;
}


//Accept the completion string from the completer's activated() signal
//and use QTextCusor to fill in the missing characters accordingly
void editor::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

//Use QTextCursor to select a word under the cursor and return it.
QString editor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

//Reimplement the focusInEvent() function in QPlainTextEdit,
//which is an event handler used to receive keyboard focus events for the widget
void editor::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

//Reimplement the keyPressEvent() function in QPlainTextEdit to ignore
//key events like Qt::Key_Enter, Qt::Key_Return, Qt::Key_Escape, Qt::Key_Tab,
//and Qt::Key_Backtab so the completer can handle them
void editor::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); //pop up the completer
}
