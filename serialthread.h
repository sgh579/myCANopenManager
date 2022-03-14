#define __NOT_USE_THREAD
#ifndef __NOT_USE_THREAD
#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QWaitCondition>
class serialThread : public QThread
{
    Q_OBJECT
public:
    explicit serialThread(QObject *parent = nullptr);
    ~serialThread();
    void run() Q_DECL_OVERRIDE;
    void transaction(const QString &portName,int baudRate);


signals:
    void heartBeat(const QString &s);
private:
    QString portName;
    int baudRate;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    QTimer mTimer;
    int lifeTime;



private slots:
    void handleTimeout();
};

#endif // SERIALTHREAD_H
#endif
