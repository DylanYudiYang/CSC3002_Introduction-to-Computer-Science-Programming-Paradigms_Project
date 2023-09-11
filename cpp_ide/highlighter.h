#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QString>
#include <QDebug>


class Highlighter: public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = 0);
    ~Highlighter();
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;//防止写错void 函数

private:
    struct Highlight_rule{
        QRegExp pattern;//the situation that need a hightlight
        QTextCharFormat format;//hightlight format
    };//the format of a single rule

    QVector<Highlight_rule> Highlight_rules; //a list of rules

    QStringList keyword_Patterns;

    //需要被设置highlight rule 的对象
    QTextCharFormat keyword_Format;//the format of keywords
    QTextCharFormat class_Format;//the format of class
    QTextCharFormat OneLineComment_Format; //单行comment: //XXXXXXX
    QTextCharFormat MultiLineComment_Format; //多行comment: /*XXXXXXX*/
    QTextCharFormat Library_Format;//#include

    //comment start & end 用于解决多行注释的问题
    QRegExp comment_Start;
    QRegExp comment_End;

    QTextCharFormat quotation_Format; //"xxx"
    QTextCharFormat function_Format;//function

    QTextCharFormat singleLine_Key;
    QTextCharFormat singleLine_Value;
    QTextCharFormat StandardLib_Format;//<xxx>
    QTextCharFormat Iostream_Format;//cin;cout



};

#endif // HIGHLIGHTER_H
