#include "highlighter.h"
#include <QDebug>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    Highlight_rule rule;

    //keywords
    keyword_Format.setForeground(Qt::darkYellow);//set color for keywords
    keyword_Format.setFontWeight(QFont::Bold);//设置字体粗细
//    QStringList keyword_Patterns;//Pattern list

    //将需要高亮的keyword 输入到list中
    keyword_Patterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                     << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                     << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                     << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                     << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                     << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                     << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                     << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                     << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                     << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"
                     << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
                     << "\\bfor\\b" << "\\breturn\\b";
    //配置keywords 的rules
    foreach (const QString &pattern, keyword_Patterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keyword_Format;
        Highlight_rules.append(rule);
    }


    //Iostream
    Iostream_Format.setForeground(QColor("orange"));//#include
    QStringList Iostream_Patterns;
    Iostream_Patterns<<"\\bcin\\b"<<"\\bcout\\b";
    //配置darkblue 颜色 的rules
    foreach (const QString &pattern, Iostream_Patterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = Iostream_Format;
        Highlight_rules.append(rule);
    }

    //include library
    Library_Format.setForeground(Qt::darkBlue);//#include
    QStringList Darkblue_kw_Patterns;
    Darkblue_kw_Patterns<<"\\B#include\\b"<<"\\bforeach\\b"<<"\\B#ifndef\\b"<<"\\B#define\\b"<<"\\B#endif\\b";
    //配置darkblue 颜色 的rules
    foreach (const QString &pattern, Darkblue_kw_Patterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = Library_Format;
        Highlight_rules.append(rule);
    }

    //class
    class_Format.setFontWeight(QFont::Bold); //class 的字体
    class_Format.setForeground(Qt::darkMagenta); //class 的颜色 深紫色
    //配置 class name 的rules
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");// 配置"类名"正则模式
    rule.format = class_Format;// 配置"类名"的文本格式
    Highlight_rules.append(rule);

    //comment
    //One line comment
    OneLineComment_Format.setForeground(Qt::darkGreen);
    //配置 One line comment 的rules
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = OneLineComment_Format;
    Highlight_rules.append(rule);
    //multi lines comment
    MultiLineComment_Format.setForeground(Qt::darkGreen);
    //highlightBlock 里通过判断具体情况来配置rule
    //comment_Start comment_End 用于highlightBlock
    comment_Start = QRegExp("/\\*");
    comment_End = QRegExp("\\*/");

    //quotation
    quotation_Format.setForeground(Qt::darkGreen);
    //配置引用高亮
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotation_Format;
    Highlight_rules.append(rule);

    //<xxx>
    StandardLib_Format.setForeground(Qt::darkGreen);
    //配置引用高亮
    rule.pattern = QRegExp("\\<.*\\>");
    rule.format = StandardLib_Format;
    Highlight_rules.append(rule);

    //function
    function_Format.setFontItalic(true);//斜体
    function_Format.setForeground(Qt::blue);//颜色
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = function_Format;
    Highlight_rules.append(rule);
};

Highlighter::~Highlighter(){

};


void Highlighter::highlightBlock(const QString &text){
    qDebug()<< text;
    foreach (const Highlight_rule &rule, Highlight_rules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);//find the first place in the text that match the highlight pattern
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);//设置highlight的格式
            index = expression.indexIn(text, index + length);//index update to the place next to the first place
        }
    }

    //多行注释问题
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)//no value will be -1
        startIndex = comment_Start.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = comment_End.indexIn(text, startIndex);//comment结束的位置
        int commentLength;//comment 的长度
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex; //没找到end text结尾-start"/*"
        } else {
            commentLength = endIndex - startIndex + comment_End.matchedLength();//找到end end-start
        }
        setFormat(startIndex, commentLength, MultiLineComment_Format); //设置highlight的格式
        startIndex = comment_Start.indexIn(text, startIndex + commentLength);//下一次搜索起点
    }

};
