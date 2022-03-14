//#define __NOT_USE_READER
#ifndef __NOT_USE_READER
#ifndef READER_H
#define READER_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE
class reader : public QObject
{
    Q_OBJECT

public:
    explicit reader(QSerialPort *serialPort, QObject *parent = nullptr);
    ~reader();

private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);
    void decode();
signals:
    void timeToShow();
    void setIOCheckBoxSignal(bool DOChange,bool DIChange,uchar DOColor,uchar DIColor);
private:
    QSerialPort *m_serialPort;
    QByteArray  m_readData;
    QTextStream m_standardOutput;
    QTimer      m_timer;

    QTimer decodeTimer;
    int decodeTimerPeriod;
    int printReadDataPeriod;
public:
    QByteArray ID;
    int DLC;
    QByteArray CANDataMessange;
};

#endif // READER_H
#endif
