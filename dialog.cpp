#include "dialog.h"
#include <QtSerialPort/QSerialPortInfo>
QT_USE_NAMESPACE
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , debugLabel(new QLabel(tr("hello,look at me!")))
    , sendButton(new QPushButton(tr("send")))
    , takeButton(new QPushButton(tr("take")))
    , openButton(new QPushButton(tr("open")))
    , num(0)
    , serialPort(new QSerialPort())
    , serialPortComboBox(new QComboBox())    
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        serialPortComboBox->addItem(info.portName());//查找可用的端口
    auto mainLayout = new QGridLayout;//界面布局
    mainLayout->addWidget(sendButton,0,0,1,1);
    mainLayout->addWidget(takeButton,0,1,1,1);
    mainLayout->addWidget(debugLabel,1,0,1,2);
    mainLayout->addWidget(serialPortComboBox, 2,0,1,1);
    mainLayout->addWidget(openButton,2,1,1,1);//打开串口按钮
    setLayout(mainLayout);
    setWindowTitle(tr("my CANopen Manager"));
    serialPortComboBox->setFocus();//不太知道干啥用的

    //connect(takeButton,&QPushButton::clicked,this,&Dialog::numPlusOne);
    //connect(sendButton,&QPushButton::clicked,this,&Dialog::showTheNum);
    //connect(&m_timer,&QTimer::timeout,this,&Dialog::handleTimeout);
    connect(openButton,&QPushButton::clicked,this,&Dialog::openPort);//点击打开串口，执行openPort
    connect(this,&Dialog::debugSignal,this,&Dialog::showTheDebugText);//接收错误信号时在debug label中提示
    m_timer.start(5000);
    Dialog::resize(100,100);
}


void Dialog::numPlusOne()
{
    auto numProcess = new test();
    Dialog::num=numProcess->numPlusOne(num);
    if (!m_timer.isActive())
        m_timer.start(5000);
}

void Dialog::showTheDebugText(const QString &s)
{
    setControlsEnabled(true);
    debugLabel->setText(tr("DEBUG: %1").arg(s));
}

void Dialog::handleTimeout()
{
    debugLabel->setText(QString::number(num,10));
}

void Dialog::setControlsEnabled(bool enable)
{
    sendButton->setEnabled(enable);
    takeButton->setEnabled(enable);
    serialPortComboBox->setEnabled(enable);
    //sendButton->setEnabled(enable);
}


void Dialog::openPort()
{
    serialPort.close();
    serialPort.setPortName(serialPortComboBox->currentText());
    if(!serialPort.open(QIODevice::ReadWrite))
        emit debugSignal(tr("can't open %1 ,error code %2")
                   .arg(serialPort.portName()).arg(serialPort.error()));
    else emit debugSignal(tr(" open %1 successfully")
                    .arg(serialPort.portName()));


}



