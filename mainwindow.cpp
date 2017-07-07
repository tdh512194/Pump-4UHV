#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialinterface.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    NowSI(new SerialInterface("COM2")),
    pump(new WindowCommand(1))
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
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->HVSwitch(3, "ON").GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);

    ui->lineEdit->insert(data.toHex());


}

void MainWindow::on_pushButton_2_clicked()
{
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->HVSwitch().Channel1().Read().GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);

    ui->lineEdit->insert(data.toHex());
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->ProtectSwitch(1,"ON").GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);

    ui->lineEdit->insert(data.toHex());
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->ReadI().GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);

    ui->lineEdit->insert(data.toHex());
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->Channel4().ReadV().GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);

    ui->lineEdit->insert(data.toHex());
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->lineEdit->clear();
    ui->plainTextEdit->clear();
    QByteArray data;
    //data = data.QByteArray::fromHex(ui->lineEdit->text().toStdString().data());
    data = pump->ReadP().GenerateMSG();
    //CRCchecksum(data);
    NowSI->simpleWriteNow(data);
    ui->lineEdit->insert(data);
}

void MainWindow::serialport_read()
{
    //ui->plainTextEdit->moveCursor(QTextCursor::End);

    QByteArray a = NowSI->mSerialPort.readAll();
    ui->plainTextEdit->insertPlainText(a);
}
