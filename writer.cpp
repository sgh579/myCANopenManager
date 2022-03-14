#include "writer.h"

writer::writer(QSerialPort *serialPort, QObject *parent)
    : QObject(parent)
    , m_serialPort(serialPort)
    , m_standardOutput(stdout)
    , m_bytesWritten(0)
{
    m_timer.setSingleShot(true);
    connect(m_serialPort, &QSerialPort::bytesWritten, this, &writer::handleBytesWritten);
    connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &writer::handleError);
    connect(&m_timer, &QTimer::timeout, this, &writer::handleTimeout);
    ID.resize(2);
}

writer::~writer()
{

}

void writer::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size()) {
        m_bytesWritten = 0;
        m_standardOutput << QObject::tr("Data successfully sent to port %1").arg(m_serialPort->portName()) << endl;
    }
}

void writer::handleTimeout()
{
    m_standardOutput << QObject::tr("Operation timed out for port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
}

void writer::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::WriteError) {
        m_standardOutput << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
}

void writer::write()
{
    code();
    QByteArray temp;
    temp.resize(1);
    m_standardOutput<<"writer: message to send:";
    for(int i=0;i<7+DLC;i++)
    {
        temp[0]=m_writeData[i];
        m_standardOutput<<temp.toHex();
    }
    m_standardOutput<<endl;
    qint64 bytesWritten = m_serialPort->write(m_writeData);//在这一步写

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
    emit timeToShowWritten();
    //m_timer.start(5000);
}

void writer::code()
{
    int i;
    QByteArray temp=QByteArray::fromHex("fffe");
    m_writeData.resize(7+DLC);
    m_writeData[0]=temp[0];
    m_writeData[1]=temp[0];
    m_writeData[2]=ID[0];
    m_writeData[3]=ID[1];
    m_writeData[4]=DLC;
    for(i=0;i<DLC;i++)
    {
        m_writeData[5+i]=CANDataMessange[i];
    }
    m_writeData[5+DLC]=temp[1];
    m_writeData[6+DLC]=temp[1];
}
