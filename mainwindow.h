#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fhd.h"
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QTimer>
#include <QComboBox>
#include "test.h"
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include "reader.h"
#include <QDebug>
#include "writer.h"
#include "gcan4055.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPushButton;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//signals:
//    void showMessageSignal(const QString &s1,const QString &s2,const QString &s3);//在表格内显示特定字符串

private slots:
    void setPortParameter();//设置要打开的串口参数，并打开，实例化reader对象，开始接收
    void showMessage(const QString &s1,const QString &s2,const QString &s3,int direction);//在表格内显示特定字符串
    void findPort();//主动更新可用串口
    void deBugTimeout();//定时打印调试信息
    void deBugByButton();//按键触发打印调试信息
    void writerSerialPortFromButton();
    void setIOCheckBox(bool DOChange,bool DIChange,uchar DOColor,uchar DIColor);
    void startSlaveEquipment();
public slots:
    void timeToShowSerialMessage();//在表格内显示由串口接收到的特定字符串
    void timeToShowSerialMessageWritten();//在表格内显示由串口发送的特定字符串
    void buttonTrigger_sendMessage(uchar DOSetState);

private:
    Ui::MainWindow *ui;
    QString serialPortName;
    QSerialPort serialPort;
    reader *serialPortReader;
    writer *serialPortWriter;
    QTextStream m_standardOutput;
    QTimer deBugTimer;
    QTimer findPortTimer;
    int findPortTimerPeriod;
    GCAN4055 *equipment;
    fhd *d;
};
#endif // MAINWINDOW_H
