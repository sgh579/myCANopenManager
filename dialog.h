#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QTimer>
#include <QComboBox>
#include "test.h"
//#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QComboBox;
QT_END_NAMESPACE
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
signals:
    void debugSignal(const QString &s);
private slots:
    void numPlusOne();
    void showTheDebugText(const QString &s);
    void handleTimeout();
    void openPort();
private:
    QLabel *debugLabel;
    QPushButton *sendButton;
    QPushButton *takeButton;
    QPushButton *openButton;
    QTimer m_timer;
    int num;
    QString serialPortName;
    QSerialPort serialPort;
    QComboBox *serialPortComboBox;
    void setControlsEnabled(bool enable);

};

#endif // DIALOG_H
