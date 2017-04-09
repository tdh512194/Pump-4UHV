#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialinterface.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    SerialInterface *NowSI;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //declaration
    void on_pushButton_clicked();
    //declaration for reading
    void serialport_read();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
