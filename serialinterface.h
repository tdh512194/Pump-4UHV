#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QMainWindow>
#include <QHash>
#include <QSerialPort>


namespace Ui {
class SerialInterface;
}

class SerialInterface : public QMainWindow
{
public:
    static QHash <QString, SerialInterface*> &SIObjSet;

    QString &mPortName;

    bool mIsConnected;

//public:
    //it was private
    QSerialPort &mSerialPort;
    //unique way to define
    explicit SerialInterface(const QString &PortName);
    ~SerialInterface();
    //return the in used SerialInterface
    static SerialInterface &SI(const QString &PortName);
    //return the in used port name
    const QString GetPortName() const;
    //check if the Portname has existed
    static bool IsSI(const QString &PortName);
    //count the available ports
    static quint8 CountSI();
    //delete the port with PortName
    static void DeleteSI(const QString &PortName);


    //check existence, remove from Hashtable,
    //close port, set Portname, assign var
    //open port and set the mIsConnected bool
    //insert the new Port to the Hashtable
    void ReconfigSerialPort(const QString &PortName);
    //close the in used port
    void Disconnect();
    //return the connect bool
    bool IsConnected();

    //write
    //const to make sure the function does not change the referenced value
    //The WindowCommand function should change the value of referenced result object.
    bool WriteNow(const QByteArray &DataPendingToSend);
    //Simple version of the write function, take strings and convert to hex and send
    //Test purpose
    void simpleWriteNow(const QByteArray Data);

    //Read to be implemented later

};

#endif // SERIALINTERFACE_H
