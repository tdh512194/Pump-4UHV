#include "mainwindow.h"
#include <QApplication>
#include <windowcommand.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    WindowCommand test(0);
    QByteArray mess;
    mess = test.ReadP().SetChannel(1).GenerateMSG();

    qDebug() << mess.toHex();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
