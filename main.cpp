#include "mainwindow.h"
#include <QApplication>
#include <windowcommand.h>
#include <QDebug>

int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
