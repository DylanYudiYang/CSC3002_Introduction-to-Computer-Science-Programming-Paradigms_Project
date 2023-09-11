/*
 * File: ide.h
 * -----------
 * This interface defines the IDE class inherited from QMainwindow
 * and all other widgets under the framework.
 */

#ifndef IDE_H
#define IDE_H

#include <QtWidgets/QMainWindow>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <QString>
#include <QPlainTextEdit>
#include <qdialog.h>
#include <qlineedit.h>
#include <QCloseEvent>
#include <vector>
#include <map>

#include <highlighter.h>
#include <editor.h>

using namespace std;


class IDE: public QMainWindow
{
    Q_OBJECT
public:
    IDE (QWidget * parent = nullptr);
    ~IDE();

private slots://slot functions
    void changeTitle(); //change the window title whenever the text is modified

    void Newfile();
    void Openfile();
    void Save();
    void Saveasfile();
    void Closefile();
    void Searchcode();
    void Runcode();
    void Debugging();
    void HelpaboutIDE();
    void Result_show();//show the search result
    void closeEvent(QCloseEvent *event);

private:
    void create_UI();
    bool check_save(); //check the save status
    bool loadFile(const QString &fileName);
    bool Savefile(const QString &fileName);

private:
//code editor
    editor *code_w;

//syntax hightlighter
    Highlighter *high;

//code completer
    QCompleter *completer = nullptr;
    QAbstractItemModel *modelFromFile(const QString& path);

//menubar
    QMenuBar *menubar;

//Debug_output_dock
    QDockWidget *debug_dock;
    QPlainTextEdit *output_pane;

//file(F)
    QMenu *file;
    QAction *newfile;
    QAction *openfile;
    QAction *save;
    QAction *saveas;
    QAction *Exit;

//edit(E)
    QMenu *edit;
    QAction *Cut;
    QAction *Copy;
    QAction *Paste;
    QAction *Search;


//build(B)
    QMenu *build;
    QAction *run;

//debug(D)
    QMenu *debug;
    QAction *check_syntax;

//help(H)
    QMenu *help;
    QAction *about;

//search window
    QDialog *searchdlg;//search window
    QLineEdit *codebefound;//code to be searched

//whether the currently editing text exists as a file or not
    bool existed;

//map to store the debug result
    map<int, vector<int>> bug_info;

//file path
    QString curFile;//current file path

};

#endif // IDE_H
