#include "ide.h"

#include <QtWidgets/QMainWindow>
#include <QApplication>
#include <QString>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IDE w;
    w.show();
    return a.exec();
}
