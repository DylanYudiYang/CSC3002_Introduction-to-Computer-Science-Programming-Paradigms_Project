/*
 * File: editor.h
 * --------------
 * This interface defines code editor class inherited from
 * QPlainTextEdit and a LineNumberArea class inherited from
 * QWidgetto add the area to show the line number.
 */

#ifndef EDITOR_H
#define EDITOR_H
#include <QPlainTextEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

//Editor definition

class editor : public QPlainTextEdit
{
    Q_OBJECT

public:
    editor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

    void insertCompletion(const QString &completion);

private:
//Area to show the line number
    QWidget *lineNumberArea;

//
    QString textUnderCursor() const;

    QCompleter *c = nullptr;
};



//Resize and make extra area at the left of the editor
//Make this a separate widget for the convenience of addingh other code editor features

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(editor *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    editor *codeEditor;
};

#endif // EDITOR_H
