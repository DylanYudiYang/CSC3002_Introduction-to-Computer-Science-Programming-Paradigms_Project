/*
 * File: ide.cpp
 * -------------
 * This program implements the ide.h interface to implement
 * the IDE class, which is a subclass of QMainWindow. It is
 * implemented as an IDE by using QMainWindow as a main framework
 * and combining all the widgets together: the menubar by QMenubar,
 * the editor by QPlainTextEdit (including Highlighter and Completer),
 * and the debug output pane by QDockWidget. The code of implementing
 * the completer is adapted from the official example of Qt.
 */

#include "ide.h"
#include "editor.h"
#include "BugCheck.h"
#include "Translater.h"
#include "simulate.h"
#include "preprocess.h"

#include <QString>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFile>
#include <QHBoxLayout>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <QPushButton>
#include <QCloseEvent>
#include <QDebug>
#include <qdockwidget.h>
#include <QStringListModel>
#include <QGuiApplication>
#include <QDebug>

#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

IDE::IDE(QWidget * parent)
               : QMainWindow(parent)
{
    create_UI();

};

IDE::~IDE()
{

};

void IDE::create_UI(){

    existed = false;

//code_editor
    code_w = new editor; // instantiates a QPlainTextEdit

    // set the tab distance to be 4 spaces
    code_w->setTabStopDistance(QFontMetricsF(code_w->font()).horizontalAdvance(" ") * 4);
    connect(code_w,SIGNAL(textChanged()),this,SLOT(changeTitle()));

    //syntax hightlighter
    QTextDocument *code_Document = code_w->document();
    high = new Highlighter(code_Document);

    //code completer
    completer = new QCompleter(this);
    completer->setModel(modelFromFile(QString::fromStdString(":/resources/wordlist.txt"))); // the model comes from a list of words to help QCompleter complete words
//    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    code_w->setCompleter(completer); // set up the completer for the editor


    setCentralWidget(code_w);// editor position
    resize(800, 600); // set the main window size
    setWindowTitle(QString::fromLocal8Bit("Untitled.txt"));

//menubar
    menubar= new QMenuBar;

//file(F)
//connect() connect the action button with function. In this way, the button can really implement some function.
    file= new QMenu(QString::fromLocal8Bit("File"));

    newfile= new QAction(QString::fromLocal8Bit("New File(&N)"),this);
    connect(newfile,&QAction::triggered,this,&IDE::Newfile);

    openfile=new QAction(QString::fromLocal8Bit("Open File(&O)"),this);
    connect(openfile,&QAction::triggered,this,&IDE::Openfile);

    save=new QAction(QString::fromLocal8Bit("Save(&S)"),this);
    connect(save,&QAction::triggered,this,&IDE::Save);

    saveas=new QAction(QString::fromLocal8Bit("Save As(&A)"),this);
    connect(saveas,&QAction::triggered,this,&IDE::Saveasfile);

    Exit=new QAction(QString::fromLocal8Bit("Exit(&Q)"),this);
    connect(Exit,&QAction::triggered,this,&IDE::Closefile);


    file->addAction(newfile);
    file->addAction(openfile);
    file->addAction(save);
    file->addAction(saveas);
    file->addAction(Exit);

//edit(E)
    edit= new QMenu(QString::fromLocal8Bit("Edit"));

    Cut= new QAction(QString::fromLocal8Bit("Cut(&X)"),this);
    connect(Cut,&QAction::triggered,code_w,&QPlainTextEdit::cut);

    Copy= new QAction(QString::fromLocal8Bit("Copy(&C)"),this);
    connect(Copy,&QAction::triggered,code_w,&QPlainTextEdit::copy);

    Paste= new QAction(QString::fromLocal8Bit("Paste(&V)"),this);
    connect(Paste,&QAction::triggered,code_w,&QPlainTextEdit::paste);

    Search= new QAction(QString::fromLocal8Bit("Search(&F)"),this);
    connect(Search,&QAction::triggered,this,&IDE::Searchcode);


    edit->addAction(Cut);
    edit->addAction(Copy);    
    edit->addAction(Paste);
    edit->addAction(Search);

//build(B)
    build=new QMenu(QString::fromLocal8Bit("Build"));

    run= new QAction(QString::fromLocal8Bit("Run(&R)"),this);
    connect(run,&QAction::triggered,this,&IDE::Runcode);

    build->addAction(run);

//debug(D)
    debug= new QMenu(QString::fromLocal8Bit("Debug"));

    check_syntax= new QAction(QString::fromLocal8Bit("Check Syntax(&D)"),this);
    connect(check_syntax,&QAction::triggered,this,&IDE::Debugging);

    debug->addAction(check_syntax);


//help(H)
    help= new QMenu(QString::fromLocal8Bit("Help"));

    about= new QAction(QString::fromLocal8Bit("About(&P)"),this);
    connect(about,&QAction::triggered,this,&IDE::HelpaboutIDE);

    help->addAction(about);


//set menubar
    menubar->addMenu(file);
    menubar->addMenu(edit);
    menubar->addMenu(build);
    menubar->addMenu(debug);
    menubar->addMenu(help);
    this->setMenuBar(menubar);


//search window
    QHBoxLayout *HBL= new QHBoxLayout;//布局
    searchdlg= new QDialog(this); //search window
    QLabel *label= new QLabel(QString::fromLocal8Bit("Search Code"));
    codebefound= new QLineEdit; //code to be searched
    QPushButton *QPB= new QPushButton(QString::fromLocal8Bit("Search"));//搜索框上的搜索按钮
    connect(QPB,&QPushButton::clicked,this,&IDE::Result_show);
    HBL->addWidget(label);
    HBL->addWidget(codebefound);
    HBL->addWidget(QPB);
    searchdlg->setLayout(HBL);//设置当前页面布局
    searchdlg->hide();//当不用的时候收起来

//the window to show the result of syntax check
    debug_dock = new QDockWidget(tr("Debug Output"), this);
    debug_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    debug_dock ->setFixedHeight(180);
    output_pane = new QPlainTextEdit;
    output_pane->setReadOnly(true);
    output_pane->adjustSize();
    debug_dock->setWidget(output_pane); // add a QPlainTextEdit to the dock to display the result
    debug_dock->hide(); // hide the dock until syntax check
    addDockWidget(Qt::BottomDockWidgetArea, debug_dock); // put the dock at the bottom of the window


};


// convert the completion word list to a model acceptable by the QCompleter, i.e.,
// accept the file path and extract the contents of this file into a QStringListModel
QAbstractItemModel *IDE::modelFromFile(const QString& path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qDebug()<<file.error();
        qDebug()<<file.errorString();
        return new QStringListModel(completer);
    }


#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << QString::fromUtf8(line.trimmed());
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}



// Whenever the text is modified compared to last saved text,
// add a "*" after the window title, i.e., the file path
void IDE::changeTitle(){
    if (!windowTitle().endsWith(QString::fromLocal8Bit("*"))){
        setWindowTitle(windowTitle() + QString::fromLocal8Bit("*"));
    }
}


void IDE::Newfile(){
    //code_w->setPlainText(QString::fromLocal8Bit("new file"));
    IDE *another = new IDE;
    another->existed=false;
    another->show();
};

void IDE::Openfile(){
    if (code_w->document()->isModified()){
        check_save();
    }

    curFile = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("open_a_file"),".","text(*.txt);;AllFile(*.*)");
    loadFile(curFile);
    existed = true;
};

bool IDE::loadFile(const QString & filePath){
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly|QFile::Text)){
        QMessageBox::warning(this, QString::fromLocal8Bit("Read File"),QString::fromLocal8Bit("Cannot Read File%1:\n%2").arg(filePath).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    code_w->setPlainText(in.readAll());
    curFile=QFileInfo(file).canonicalFilePath();
    setWindowTitle(filePath);
    return true;
}

// Save button
void IDE::Save(){
    if (existed){
        Savefile(curFile);
    }else{
        Saveasfile();
    }
}


bool IDE::Savefile(const QString &filePath){
/*
    QFile file(fileName);
    file.open(QFile::WriteOnly | QFile::Text);

    QTextStream out(&file);
    out << code_w->toPlainText();
*/
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly|QFile::Text)){
        QMessageBox::warning(this, QString::fromLocal8Bit("save_file"),QString::fromLocal8Bit("cannot_save_file%1:\n%2").arg(filePath).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out<<code_w->toPlainText();

    existed=true;
    code_w->document()->setModified(false);

    curFile=QFileInfo(file).canonicalFilePath();
    setWindowTitle(filePath);
    return true;

};


void IDE::Saveasfile(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    "code.txt");
    //tr("") when we open the file browser, the title of the browser becomes Save As and the default saved file is called "code.txt"
    if (fileName.isEmpty()){
        return;
    }
    Savefile(fileName);
    existed=true;
    code_w->document()->setModified(false);
};

// Pop up a message box asking for the command if the current file
// has not been saved. Returns true if the user choosed to save the
// file, returns falso otherwise.
bool IDE::check_save(){

    QMessageBox box;
    box.setWindowTitle(QString::fromLocal8Bit("Warning"));
    box.setIcon(QMessageBox::Warning);
    box.setText(QString::fromLocal8Bit("\"") + windowTitle().left(windowTitle().length() - 1) + QString::fromLocal8Bit("\" has not been saved yet. Do you want to save it?"));
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if (box.exec()==QMessageBox::Yes){
        Save();
        return true;
    } else return false;


}

// close the current window
void IDE::Closefile(){
    if(existed){
        this->close();

    }else{
        QMessageBox::StandardButton user_choice = QMessageBox::question( this,  "C++ IDE",                                                              tr("Are you sure?\n"),
                                                                         QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                         QMessageBox::Yes);
        if (user_choice != QMessageBox::Yes) {
            return;

        } else {
            // save data
            Save();
            this->close();
        }
    }
};



void IDE::Searchcode(){
    searchdlg->show();

};

void IDE::Result_show(){
    QString C=codebefound->text();
    bool isfind=code_w->find(C); //Search after the text
    if (isfind){
        //highlight
        QPalette palette =code_w->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
        code_w->setPalette(palette);
    }else{
        QMessageBox::information(this,QString::fromLocal8Bit("Tips:"),QString::fromLocal8Bit("The search has been completed! Should we start from the beginning?"),QMessageBox::Ok);
        QTextCursor cursor = code_w->textCursor();
        cursor.movePosition(QTextCursor::Start);
        code_w->setTextCursor(cursor);
        std::cout<<cursor.position();
        std::cout<<code_w->textCursor().position();


    }
};


// the slot function to run the code
void IDE::Runcode(){

    string f_path;
    bool temp = false; // flag of whether creating a temporary file to run


    if (code_w->document()->isModified()){
        temp = !check_save();
        if (!temp) { // run the current file
            f_path = curFile.toStdString();
        } else { // save the current text as a temporary file to run
            QFile tempfile("temp_run.txt");
            tempfile.open(QFile::WriteOnly|QFile::Text);
            QTextStream out(&tempfile);
            out << code_w->toPlainText();
            tempfile.close();
            f_path = QFileInfo(tempfile).canonicalFilePath().toStdString();
        }
    } else f_path = curFile.toStdString(); // no modification since last save


    // normalize the path of intermediate files
    string processed_path = f_path.substr(0, f_path.find_last_of(".")) + "_preprocessed.txt";
    string MIPS_path = f_path.substr(0, f_path.find_last_of(".")) + "_MIPS.txt";
    string data_path = f_path.substr(0, f_path.find_last_of(".")) + "_data.txt";
    string text_path = f_path.substr(0, f_path.find_last_of(".")) + "_text.txt";
    string machinecode_path = f_path.substr(0, f_path.find_last_of(".")) + "_machinecode.txt";

    ifstream infile;
    ofstream processed_file;
    infile.open(f_path.c_str());
    processed_file.open(processed_path);
    Preprocess(infile, processed_file); // pre-process the source code file
    infile.close();
    processed_file.close();

    ofstream MIPS_code;
    map<string, bool> map_reg;
    MIPS_code.open(MIPS_path.c_str());
    Translator(processed_path, MIPS_code, map_reg); // compile the pre-processed file into MIPS code
    MIPS_code.close();

    simulate(MIPS_path); // pass the MIPS code into assembler and simulator


    // delete the intermediate (and temporary) files
    if (temp) remove(f_path.c_str());
    remove(processed_path.c_str());
    remove(MIPS_path.c_str());
    remove(data_path.c_str());
    remove(text_path.c_str());
    remove(machinecode_path.c_str());
};


// the slot function to check the syntax of current text
void IDE::Debugging(){
    // save the current text into a temporary file for syntax check
    QFile tempfile("temp_debug.txt");
    tempfile.open(QFile::WriteOnly|QFile::Text);
    QTextStream out(&tempfile);
    out << code_w->toPlainText();
    tempfile.close();

    string temp_file_path = QFileInfo(tempfile).canonicalFilePath().toStdString();


    ifstream infile;
    infile.open(temp_file_path.c_str());

    map<int, string> bug_type; // map to store the bug type
    map<int, vector<int>> bug_info; // map to store the bug information (the position and type)
    vector<int> bug_line; // map to store the bug position
    bool no_error = Debugger(infile, bug_type, bug_info, bug_line); // pass the file to the debugger to check syntax
    infile.close();

    remove(temp_file_path.c_str()); // delete the temporary file

    if (no_error) { // no syntax error
        debug_dock->hide();
        QMessageBox::information(NULL, "No Errors", "No Syntax Errors!");
        return;
    }

    string bugs;
    string bug;
    map<int, vector<int>>::iterator iter;
    iter = bug_info.begin();
    // iterate through bug_info to append all the syntax error information together
    for(iter = bug_info.begin(); iter != bug_info.end(); iter++) {
        for(int i = 0; i < static_cast<int>(iter->second.size()); i++) {
            bug = "Line " + to_string(iter->first) + ": " + bug_type[iter->second[i]] + "\n";
            bugs.append(bug);
        }
    }

    output_pane->setPlainText(QString::fromStdString(bugs)); // display the syntax error information
    debug_dock->show(); // show the dock

};

void IDE::HelpaboutIDE(){
    QMessageBox:: about(this,QString::fromLocal8Bit("about"),QString::fromLocal8Bit("IDE based on QT"));
};


void IDE::closeEvent (QCloseEvent *event)
{
    if (code_w->document()->isModified()){
        check_save();
    }

    QMessageBox::StandardButton user_choice = QMessageBox::question( this, "Warning", tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (user_choice != QMessageBox::Yes) {
        event->ignore();
    } else {
        // save data
        event->accept();
    }
}
