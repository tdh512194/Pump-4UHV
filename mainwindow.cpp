#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialinterface.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    NowSI(new SerialInterface("COM8"))
{
    ui->setupUi(this);
    //connect parameter is address hence the '&'
    connect(&NowSI->mSerialPort,SIGNAL(readyRead()),this,SLOT(serialport_read()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void CRCchecksum(QByteArray &data)
{
    //the function generates 2 CRC checksum and connect it at the end of the data string
    quint8 checksum = 0;
    QByteArray bytes[2];
    int len = data.size();
    //XOR all the bits AFTER 0x02
    for(int i = 1; i < len; i++)
    {
        checksum = checksum ^ data[i];
    }
    //a check sum of a 2-digits hex
    //split it into two ASCII characters and convert to hex again
    QByteArray checkSum;
    checkSum.append(QString::number((checksum /16),16).toUpper());
    checkSum.append(QString::number((checksum %16),16).toUpper());
    //connect the checksum at the end of the data string
    data.QByteArray::append(checkSum);
}


void MainWindow::on_pushButton_clicked()
{
    QByteArray data;
    data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    CRCchecksum(data);
    NowSI->simpleWriteNow(data);


}

void MainWindow::serialport_read()
{
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    QByteArray a = NowSI->mSerialPort.readAll();
    ui->plainTextEdit->insertPlainText(a.toHex());
}
