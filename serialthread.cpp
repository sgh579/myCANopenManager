#include "serialthread.h"
#ifndef __NOT_USE_THREAD
#include "mainwindow.h"
#include <QTime>
serialThread::serialThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false), lifeTime(0)
{
    mTimer.start(5000);
    connect(&mTimer,&QTimer::timeout,this,&serialThread::handleTimeout);

}
serialThread::~serialThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void serialThread::run()
{
    sleep(1);


    QString currentPortName;

    //qDebug()<<"here are serial thread"<<endl;
    emit heartBeat(tr("here are serial thread"));


    QSerialPort serialPort;
    serialPort.setPortName(portName);
    serialPort.setBaudRate(baudRate);
    reader serialPortReader(&serialPort);
    emit heartBeat(tr("port name and baud rate set by serial thread: %1 %2").arg(portName).arg(baudRate));
//    while(1)
//    {
//        sleep(1);
//        //emit heartBeat(tr("in while(1)"));
//    }

}

void serialThread::handleTimeout()
{
    mutex.lock();
    if (!mTimer.isActive())
        mTimer.start(5000);
    lifeTime+=5;
    emit heartBeat(tr("serial thread is alive lifetime: %1 s").arg(lifeTime));
    mutex.unlock();

}

void serialThread::transaction(const QString &portName, int baudRate)
{
    QMutexLocker locker(&mutex);//上锁
    this->portName=portName;
    this->baudRate=baudRate;
    qDebug()<<"transaction OK"<<endl;
}

#endif
