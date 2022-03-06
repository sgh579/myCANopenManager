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
    qint64 bytesWritten = m_serialPort->write(m_writeData);//在这一步写

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }

    m_timer.start(5000);
}

void writer::code()
{
    int i;
    QByteArray temp=QByteArray::fromHex("fffe");
    m_writeData.resize(6+DLC);
    m_writeData[0]=temp[0];
    m_writeData[1]=temp[0];
    m_writeData[2]=ID;
    m_writeData[3]=DLC;
    for(i=0;i<DLC;i++)
    {
        m_writeData[4+i]=CANDataMessange[i];
    }
    m_writeData[4+DLC]=temp[1];
    m_writeData[5+DLC]=temp[1];
}
