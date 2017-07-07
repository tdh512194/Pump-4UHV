#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialinterface.h"
#include <windowcommand.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    SerialInterface *NowSI;
    WindowCommand *pump;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //declaration
    void on_pushButton_clicked();
    //declaration for reading
    void serialport_read();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
