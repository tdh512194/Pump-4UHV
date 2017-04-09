#include "windowcommand.h"

//Overload '<<' operator instead of using 'append' to accept larger parameter
QByteArray &operator <<(QByteArray &QBArr, const quint8 Dat)
{
    QBArr.append(Dat);
    return QBArr;
}

QByteArray &operator <<(QByteArray &QBArr, const quint16 Dat)
{
    return QBArr<<quint8(Dat>>8)<<quint8(Dat);
}

QByteArray &operator <<(QByteArray &QBArr, const quint32 Dat)
{
    return QBArr<<quint16(Dat>>16)<<quint16(Dat);
}

QByteArray &operator <<(QByteArray &QBArr, const quint64 Dat)
{
    return QBArr<<quint32(Dat>>32)<<quint32(Dat);
}

QByteArray &operator <<(QByteArray &QBArr, const QByteArray &Dat)
{
    QBArr.append(Dat);
    return QBArr;
}

//Define the bases
QByteArray tmp = "00000";
const WindowCommand::TypDATA WindowCommand::NumBase = tmp;

QByteArray tmp4 = "01";
const WindowCommand::TypWIN WindowCommand::ChannelBase = tmp4;

QByteArray tmp10 = "80";
const WindowCommand::TypWIN WindowCommand::TemperatureBase = tmp10;



WindowCommand::WindowCommand(const quint8 WCNum)
    :mWCNo(WCNum)
    ,mADDR(TypADDR(AddressBase + WCNum))
    ,mWIN("000")
    ,mCOM(COMRead)
    ,mDATA(* new TypDATA())
    ,mCRC("00")
    ,mMSG(*new TypMSG())
    ,IsTemperature(false)
    ,IsHV(false)
    ,mCHANNEL(5) //no Channel is set
{
    if (WindowCommand::WCObjSet.contains(WCNum))   //If There's An Object With That BPNum, Then Throw Exception
        throw std::invalid_argument(std::string("Pump #")+std::to_string(WCNum)+std::string(" Has Already Existed !!!"));
    else
        WindowCommand::WCObjSet.insert(WCNum,this);
}

WindowCommand::~WindowCommand()
{
    WindowCommand::WCObjSet.remove(mWCNo);
}

WindowCommand &WindowCommand::WC(const quint8 WCNum)
{
    if (WindowCommand::WCObjSet.contains(WCNum))
        return *WindowCommand::WCObjSet.value(WCNum);
    else
        throw std::invalid_argument(std::string("Pump #")+std::to_string(WCNum)+std::string(" Does Not Exist !!!"));
}

WindowCommand &WindowCommand::WC(const TypMSG &QBArr)
{
    // <STX> + <ADDR> + <WIN> + <COM> + <DATA> + <ETX> + <CRC>
    WindowCommand::TypADDR WCAdress = *QBArr.mid(sizeof(STX),SzADDR);

    quint8 tmp1 = sizeof(STX);
    WindowCommand &WCTmp = WindowCommand::WC(WCAdress - AddressBase);
    WCTmp.mADDR = WCAdress;
    tmp1 += SzADDR;
    WCTmp.mWIN = QBArr.mid(tmp1,SzWIN);
    tmp1 += SzWIN;
    WCTmp.mCOM = *QBArr.mid(tmp1,SzCOM);
    tmp1 += SzCOM;
    quint8 tmp2 = sizeof(QBArr);
    WCTmp.mCRC = QBArr.right(tmp2 - SzCRC);
    tmp2 -= tmp1 + SzCRC + sizeof(ETX); //data length
    WCTmp.mDATA = QBArr.mid(tmp1,tmp2);

    WCTmp.mMSG.clear();
    WCTmp.mMSG << STX << WCTmp.mADDR << WCTmp.mWIN << WCTmp.mCOM << WCTmp.mDATA << ETX <<WCTmp.mCRC;
    return WCTmp;

}

bool WindowCommand::IsWC(const quint8 WCNum)
{
    return WindowCommand::WCObjSet.contains(WCNum);
}

quint8 WindowCommand::CountWC()
{
    return WindowCommand::WCObjSet.size();
}

bool WindowCommand::DeleteWC(const quint8 WCNum)
{
    if(WindowCommand::WCObjSet.contains(WCNum))
    {
        WindowCommand::WCObjSet.value(WCNum)->~WindowCommand();
        return true;
    }
    else
        return false;
}

/*quint8 WindowCommand::GetWCNo() const
{

}

void WindowCommand::SetWCNo(const quint8 WCNum)
{

}

const WindowCommand::TypMSG WindowCommand::GetMSG() const
{

}

WindowCommand::TypADDR WindowCommand::GetADDR() const
{

}

WindowCommand::TypWIN WindowCommand::GetWIN() const
{

}

WindowCommand::TypCOM WindowCommand::GetCOM() const
{

}

const WindowCommand::TypDATA WindowCommand::GetDATA() const
{

}

WindowCommand::TypCRC WindowCommand::GetCRC() const
{

}*/

const WindowCommand::TypCRC WindowCommand::GenerateCRC(const QByteArray &data)
{
    // <STX> + <ADDR> + <WIN> + <COM> + <DATA> + <ETX>
    quint8 checksum = 0;
    QByteArray bytes[2];
    int len = data.size();
    //XOR all the bits AFTER STX
    for(int i = sizeof(STX); i < len; i++)
    {
        checksum = checksum ^ data[i];
    }
    //a check sum of a 2-digits hex
    //split it into two ASCII characters and convert to hex again
    WindowCommand::TypCRC CRC;
    CRC.append(QString::number((checksum /16),16).toUpper());
    CRC.append(QString::number((checksum %16),16).toUpper());


    return CRC;
}

const WindowCommand::TypMSG WindowCommand::GenerateMSG()
{
    // <STX> + <ADDR> + <WIN> + <COM> + <DATA> + <ETX>
    //reset the flags
    IsTemperature = false;
    IsHV = false;

    mMSG.clear();
    mMSG << STX << mADDR << mWIN << mCOM << mDATA << ETX;
    mCRC.clear();
    mCRC = WindowCommand::GenerateCRC(mMSG);

    //connect the checksum at the end of the data string
    mMSG << mCRC;

    return mMSG;
}

/*QString WindowCommand::GetWindow() const
{

}

QString WindowCommand::GetCommand() const
{

}

const QString WindowCommand::GetData() const
{

}

const QString WindowCommand::GetMessage() const
{

}*/

//Complete Functions
WindowCommand &WindowCommand::Raw(const QByteArray RawMSG)
{
    //<ADDR> + <WIN> + <COM> + <DATA>
    WindowCommand::TypADDR WCAddress = *RawMSG.left(SzADDR);

    quint8 tmp1 = 0;
    WindowCommand &WCTmp = WindowCommand::WC(WCAddress - AddressBase);
    WCTmp.mADDR = WCAddress;
    tmp1 += SzADDR;
    WCTmp.mWIN = RawMSG.mid(tmp1,SzWIN);
    tmp1 += SzWIN;
    WCTmp.mCOM = *RawMSG.mid(tmp1,SzCOM);
    tmp1 += SzCOM;
    WCTmp.mDATA = RawMSG.right(tmp1 + 1);
    QByteArray data;
    return WCTmp;
}

WindowCommand &WindowCommand::SetBaudRate(const int BaudRate)
{
    mWIN = "108";
    mCOM = COMWrite;
    mDATA = NumBase;

    switch (BaudRate){
    case 1200:
    {
        mDATA << BaudRate1200;
        break;
    }
    case 2400:
    {
        mDATA << BaudRate2400;
        break;
    }
    case 4800:
    {
        mDATA << BaudRate4800;
        break;
    }
    case 9600:
    {
        mDATA << BaudRate9600;
        break;
    }
    }

    return *this;
}

/*WindowCommand &WindowCommand::ReadModel()
{

}*/

WindowCommand &WindowCommand::SelectSerialType(QByteArray Serial)
{
    mWIN = "504";
    mCOM = COMWrite;

    Serial.toUpper();
    if( Serial == "RS232")
    {
        mDATA << SerialRS232;
    }
    else if ( Serial == "RS485")
    {
        mDATA << SerialRS485;
    }


    return *this;
}

/*WindowCommand &WindowCommand::ReadInterlock()
{

}*/

WindowCommand &WindowCommand::UnitPressure(QByteArray Pressure)
{
    mWIN = "600";
    mCOM = COMWrite;
    mDATA = NumBase;

    Pressure = Pressure.toUpper();
    if( Pressure == "TORR")
    {
        mDATA << PressureTorr;
    }
    else if( Pressure == "MBAR")
    {
        mDATA << PressuremBar;
    }
    else if ( Pressure == "PA")
    {
        mDATA << PressurePa;
    }

    return *this;
}

WindowCommand &WindowCommand::UnitPressure()
{
    mWIN = "600";
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::ReadModel()
{
    mWIN = "319";
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::ReadBaudRate()
{
    mWIN = "108";
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::HVSwitch(const int Channel, QByteArray State)
{
    IsHV = true; //set the flag
    mWIN.clear();
    switch (Channel){
    case 1:
        mCHANNEL = CHANNEL1;
        break;
    case 2:
        mCHANNEL = CHANNEL2;
        break;
    case 3:
        mCHANNEL = CHANNEL3;
        break;
    case 4:
        mCHANNEL = CHANNEL4;
        break;
    }

    State = State.toUpper();
    if(State == "ON")
    {
        mDATA[0] = ChannelON;
        mCOM = COMWrite;
    }
    else if(State == "OFF")
    {
        mDATA[0] = ChannelOFF;
        mCOM = COMWrite;
    }
    else if(State =="READ")
    {
        mCOM = COMRead;
    }

    mWIN << ChannelBase << mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::HVSwitch()
{
    IsHV = true;
    mWIN = ChannelBase;
    if (mCHANNEL != 5) //already set Channel
        mWIN[2] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::On()
{
    mDATA[0] = ChannelON;
    mCOM = COMWrite;
    return *this;
}
WindowCommand &WindowCommand::Off()
{
    mDATA[0] = ChannelOFF;
    mCOM = COMWrite;
    return *this;
}

/*WindowCommand &WindowCommand::SelectChannelError() //Win 505, view in Win 206, to be appended
{

}*/

WindowCommand &WindowCommand::ReadTemperature() //to be appended
{
    IsTemperature = true; //set the flag
    //in case the channel is set before calling and fall into non-Temperature case
    //set the mCHANNEL to correct value
    mWIN = TemperatureBase;
    switch(mCHANNEL){
    case 0x30: //Fan case
        mWIN[2] = mCHANNEL;
    case 0x31:
        mWIN = TemperatureBase;
        mCHANNEL = CHANNELTEMP1;
        mWIN[2] = mCHANNEL;
        break;
    case 0x32:
        mWIN = TemperatureBase;
        mCHANNEL = CHANNELTEMP2;
        mWIN[2] = mCHANNEL;
        break;
    case 0x33:
        mWIN = TemperatureBase;
        mCHANNEL = CHANNELTEMP3;
        mWIN[2] = mCHANNEL;
        break;
    case 0x34:
        mWIN = TemperatureBase;
        mCHANNEL = CHANNELTEMP4;
        mWIN[2] = mCHANNEL;
        break;
    case 5: //havent set Channel
        break;
    }
    return *this;
}

WindowCommand &WindowCommand::ReadV() //to be appended
{
    mWIN[0] = MeasuredBase;
    mWIN[1] = mCHANNEL; // = 5 if not set channel
    mWIN[2] = MeasuredV;
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::ReadI() //to be appended
{
    mWIN[0] = MeasuredBase;
    mWIN[1] = mCHANNEL; // = 5 if not set channel
    mWIN[2] = MeasuredI;
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::ReadP() //to be appended
{
    mWIN[0] = MeasuredBase;
    mWIN[1] = mCHANNEL; // = 5 if not set channel
    mWIN[2] = MeasuredP;
    mCOM = COMRead;

    return *this;
}

WindowCommand &WindowCommand::SetChannel(const int Channel)
{
    if(IsTemperature == true) //case Temperature
    {
        switch (Channel){
        case 1:
            mCHANNEL = CHANNELTEMP1;
            break;
        case 2:
            mCHANNEL = CHANNELTEMP2;
            break;
        case 3:
            mCHANNEL = CHANNELTEMP3;
            break;
        case 4:
            mCHANNEL = CHANNELTEMP4;
            break;
        }
    }
    else //case SWITCH and the rest
    {
        switch (Channel){
        case 1:
            mCHANNEL = CHANNEL1;
            break;
        case 2:
            mCHANNEL = CHANNEL2;
            break;
        case 3:
            mCHANNEL = CHANNEL3;
            break;
        case 4:
            mCHANNEL = CHANNEL4;
            break;
        }
    }
    if(IsHV == true || IsTemperature == true) // WIN X-X-Channel
    {
        mWIN[2] = mCHANNEL;
    }
    else //the rests WIN X-Channel-X
        mWIN[1] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::SetChannel(QByteArray Channel) //accepts Fan
{
    Channel = Channel.toUpper();
    if( Channel == "FAN")
    {
        mCHANNEL = FAN;
        mWIN[2] = mCHANNEL;
        mCOM = COMRead;
    }


    return *this;
}

WindowCommand &WindowCommand::Channel1()
{
    if(IsTemperature == true)
        mCHANNEL = CHANNELTEMP1;
    else
        mCHANNEL = CHANNEL1;
    if(IsHV == true || IsTemperature == true) // WIN X-X-Channel, already called the function
    {
        mWIN[2] = mCHANNEL;
    }
    else //the rests WIN X-Channel-X,
        mWIN[1] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::Channel2()
{
    if(IsTemperature == true)
        mCHANNEL = CHANNELTEMP2;
    else
        mCHANNEL = CHANNEL2;
    if(IsHV == true || IsTemperature == true) // WIN X-X-Channel, already called the function
    {
        mWIN[2] = mCHANNEL;
    }
    else //the rests WIN X-Channel-X,
        mWIN[1] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::Channel3()
{
    if(IsTemperature == true)
        mCHANNEL = CHANNELTEMP3;
    else
        mCHANNEL = CHANNEL3;
    if(IsHV == true || IsTemperature == true) // WIN X-X-Channel, already called the function
    {
        mWIN[2] = mCHANNEL;
    }
    else //the rests WIN X-Channel-X,
        mWIN[1] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::Channel4()
{
    if(IsTemperature == true)
        mCHANNEL = CHANNELTEMP4;
    else
        mCHANNEL = CHANNEL4;
    if(IsHV == true || IsTemperature == true) // WIN X-X-Channel, already called the function
    {
        mWIN[2] = mCHANNEL;
    }
    else //the rests WIN X-Channel-X,
        mWIN[1] = mCHANNEL;

    return *this;
}

WindowCommand &WindowCommand::Fan()
{
    if(IsTemperature == true) //already called the function
    {
        mCHANNEL = FAN;
        mWIN[2] = mCHANNEL;
        return *this;
    }
    else
        throw std::invalid_argument(std::string("Cannot read from Fan!!"));
}

WindowCommand &WindowCommand::Read()
{
    mCOM = COMRead;
    return *this;
}

WindowCommand &WindowCommand::Write()
{
    mCOM = COMWrite;
    return *this;
}

QHash<const quint8, WindowCommand *> &WindowCommand::WCObjSet = * new QHash<const quint8, WindowCommand *>();


