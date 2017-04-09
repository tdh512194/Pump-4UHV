#include "serialinterface.h"



SerialInterface::SerialInterface(const QString &PortName) :
    mPortName(*new QString(PortName)),
    mSerialPort(*new QSerialPort(PortName)),
    mIsConnected(false)
{
    //Configure the Serial Port
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setBaudRate(QSerialPort::Baud9600);

    //Check if the Port exists
    if(SerialInterface::SIObjSet.contains(PortName))
        //if it exists, display error message
        throw std::invalid_argument(std::string("Serial Interface with Port ")
                                   + PortName.toStdString()
                                   + std::string(" has already existed!!"));
    else
    {
        //Open port and set the value of mIsConnected
        if(mSerialPort.open(QIODevice::ReadWrite))
            mIsConnected = true;
        else
            mIsConnected = false;
        //Insert the Port to the Hashtable
        SerialInterface::SIObjSet.insert(PortName, this);
    }

}
//Deconstructor
SerialInterface::~SerialInterface()
{
    //close the Serial port
    mSerialPort.close();
    //remove the port from the Hashtable
    SerialInterface::SIObjSet.remove(mPortName);
}

//Return the current port
SerialInterface &SerialInterface::SI(const QString &PortName)
{
    if(SerialInterface::SIObjSet.contains(PortName))
        return *SerialInterface::SIObjSet.value(PortName);
    else
        throw std::invalid_argument(std::string("Serial Interface with Port ")
                                    + PortName.toStdString()
                                    + std::string(" does not exist!!"));
}

//Return the current port name
const QString SerialInterface::GetPortName() const
{
    return mSerialPort.portName();
}

//Check if the port is on the Hashtable
bool SerialInterface::IsSI(const QString &PortName)
{
    return SerialInterface::SIObjSet.contains(PortName);
}

//Count the number of ports
quint8 SerialInterface::CountSI()
{
    return SerialInterface::SIObjSet.size();
}

//Delete a port
void SerialInterface::DeleteSI(const QString &PortName)
{
    SerialInterface::SIObjSet.value(PortName)->~SerialInterface();
}

//
void SerialInterface::ReconfigSerialPort(const QString &PortName)
{
    if(SerialInterface::SIObjSet.contains(PortName))
        throw std::invalid_argument(std::string("Serial Interface with Port ")
                                   + PortName.toStdString()
                                   + std::string(" has already existed!!"));
    else
    {
      DeleteSI(PortName);
      mSerialPort.setPortName(PortName);
      mPortName = PortName;
      if(mSerialPort.open(QIODevice::ReadWrite))
          mIsConnected = true;
      else
          mIsConnected = false;
      SerialInterface::SIObjSet.insert(PortName,this);
    }
}

void SerialInterface::Disconnect()
{
    mSerialPort.close();
}

bool SerialInterface::IsConnected()
{
    return mIsConnected;
}

void SerialInterface::simpleWriteNow(const QByteArray Data)
{
    mSerialPort.write(Data);
}

//bool SerialInterface::WriteNow(const QByteArray &DataPendingToSend)
//{

//}


QHash<QString, SerialInterface *> &SerialInterface::SIObjSet = * new QHash<QString, SerialInterface *>();

