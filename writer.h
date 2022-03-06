#ifndef WRITER_H
#define WRITER_H
#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE
class writer : public QObject
{
    Q_OBJECT
public:
    explicit writer(QSerialPort *serialPort, QObject *parent = nullptr);
    ~writer();
    int ID;
    int DLC;
    QByteArray CANDataMessange;
    void write();
private slots:
    void handleBytesWritten(qint64 bytes);
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort     *m_serialPort;
    QByteArray      m_writeData;
    QTextStream     m_standardOutput;
    qint64          m_bytesWritten;
    QTimer          m_timer;
private:
    void code();

};

#endif // WRITER_H
