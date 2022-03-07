#include "reader.h"
#include <QDebug>
#ifndef __NOT_USE_READER
#include <QCoreApplication>
#include <QTime>
#include "mainwindow.h"
QT_USE_NAMESPACE
reader::reader(QSerialPort *serialPort, QObject *parent)
    : QObject(parent)
    , m_serialPort(serialPort)
    , m_standardOutput(stdout)
    , decodeTimerPeriod(100)
    , printReadDataPeriod(1000)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &reader::handleReadyRead);
    connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &reader::handleError);
    connect(&m_timer, &QTimer::timeout, this, &reader::handleTimeout);
    connect(&decodeTimer, &QTimer::timeout, this, &reader::decode);
    qDebug()<<"here are reader()"<<endl;
    m_timer.start(printReadDataPeriod);
    //sleep(1000);
    decodeTimer.start(decodeTimerPeriod);
    emit timeToShow();
    ID.resize(2);
}

reader::~reader()
{
}

void reader::handleReadyRead()
{
    m_readData.append(m_serialPort->readAll());//把读到的字符串放到m_readData后面
    //m_standardOutput<<m_readData.toHex() << endl;//通过标准输出把读到的在终端里输出
    if (!m_timer.isActive())
        m_timer.start(printReadDataPeriod);
}

void reader::handleTimeout()
{
    if (m_readData.isEmpty()) {
        //m_standardOutput << QObject::tr("No data was currently available for reading from port %1").arg(m_serialPort->portName()) << endl;
        //qDebug()<<QObject::tr("No data was currently available for reading from port %1").arg(m_serialPort->portName()) << endl;
    } else {
        //m_standardOutput << QObject::tr("Data successfully received from port %1").arg(m_serialPort->portName()) << endl;
        //qDebug()<< QObject::tr("Data successfully received from port %1").arg(m_serialPort->portName()) << endl;
        //m_standardOutput << m_readData << endl;//通过标准输出把读到的在终端里输出
        //qDebug()<< m_readData << endl;//通过标准输出把读到的在终端里输出
        //qDebug()<< m_readData.toHex() << endl;//通过标准输出把读到的在终端里输出
    }

    //QCoreApplication::quit();

}

void reader::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    }
}

void reader::decode()
{
int i;
int frameHeadLocation=0;
QByteArray temp=QByteArray::fromHex("fffe");

bool findTheFrame=0;

for(i=0;i<10;i++)
{
    if(m_readData[i]==temp[0] && m_readData[i+1]==temp[0])
    {
        if(i==0)
        {
            frameHeadLocation=i;
            findTheFrame=1;
            break;
        }
        else
        {
            m_readData.remove(0,i);
        }
    }
}
if(!findTheFrame ||  m_readData.size()<7+DLC)
{
    //qDebug()<<"not found this time"<<endl;
    return;
}

ID[0]=m_readData[frameHeadLocation+2];
ID[1]=m_readData[frameHeadLocation+3];
DLC=m_readData.at(frameHeadLocation+4);
for(i=0;i<DLC;i++)
CANDataMessange[i]=m_readData.at(frameHeadLocation+5+i);
m_readData.remove(frameHeadLocation,7+DLC);//从字节队列中删除这一帧can转串口报文

//qDebug()<<"ID:"<<ID<<endl;
//qDebug()<<"DLC:"<<DLC<<endl;
//qDebug()<<"HEX:"<<CANDataMessange.toHex()<<endl;

emit timeToShow();
//QByteArray temp1=QByteArray::fromHex("18");
//if(ID[0]==temp1[0] && DLC==1)
//{
//    uchar DIStatus=CANDataMessange[0];
//    emit setIOCheckBoxSignal(0,1,0,DIStatus);
//}





if (!decodeTimer.isActive())
    decodeTimer.start(decodeTimerPeriod);
}

#endif
