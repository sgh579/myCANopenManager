#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_standardOutput(stdout)
    , findPortTimerPeriod(1000)
{
    ui->setupUi(this);
    //thread = new serialThread(this);
    setFixedSize(792,647);
    setWindowTitle(tr("my CANopen Manager"));
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->serialPortComboBox->addItem(info.portName());//查找可用的端口
    ui->baudComboBox->addItem(tr("9600"));
    ui->baudComboBox->addItem(tr("115200"));

    connect(ui->connectPushButton,&QPushButton::clicked,this,&MainWindow::setPortParameter);
    connect(ui->findPortButton,&QPushButton::clicked,this,&MainWindow::findPort);
    connect(&findPortTimer,&QTimer::timeout,this,&MainWindow::findPort);
    connect(this,&MainWindow::showMessageSignal,this,&MainWindow::showMessage);
    //connect(&deBugTimer,&QTimer::timeout, this, &MainWindow::deBugTimeout);
    connect(ui->writeButton,&QPushButton::clicked,this,&MainWindow::writerSerialPortFromButton);
    //thread->start();
    findPortTimer.start(findPortTimerPeriod);
    m_standardOutput<<"hi"<<endl;

}

MainWindow::~MainWindow()
{
    //thread->wait();
    delete ui;
}

void MainWindow::setPortParameter()
{
    serialPort.close();//先关上
    serialPort.setPortName(ui->serialPortComboBox->currentText());//用combobx的参数设定串口端口名
    int serialPortBaudRate=ui->baudComboBox->currentText().toInt();
    serialPort.setBaudRate(serialPortBaudRate);//设置波特率
//    if(!serialPort.open(QIODevice::ReadWrite))//打开串口
//        emit debugSignal(tr("can't open %1 ,error code %2")
//                   .arg(serialPort.portName()).arg(serialPort.error()));
//    else emit debugSignal(tr(" open %1 successfully")
//                    .arg(serialPort.portName()));
    if(!serialPort.open(QIODevice::ReadWrite))
        qDebug()<<tr("can't open %1 ,error code %2")
                  .arg(serialPort.portName()).arg(serialPort.error())<<endl;
    else
        qDebug()<<tr(" open %1 successfully")
                  .arg(serialPort.portName())<<endl;
    serialPortReader = new reader(&serialPort);//serialPortReader对象初始化
    connect(serialPortReader,&reader::timeToShow,this,&MainWindow::timeToShowSerialMessage);
    //timeToShowSerialMessage();
    qDebug()<<"reader object built successfully"<<endl;
    serialPortWriter = new writer(&serialPort);
    m_standardOutput<<"writer object built successfully"<<endl;
    //thread->transaction(ui->serialPortComboBox->currentText(),serialPortBaudRate);
    //thread->start();
}

void MainWindow::showMessage(const QString &s1,const QString &s2,const QString &s3)
{
    int rowIndex=ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowIndex);
    ui->tableWidget->setItem(rowIndex,0,new QTableWidgetItem(s1));
    ui->tableWidget->setItem(rowIndex,1,new QTableWidgetItem(s2));
    ui->tableWidget->setItem(rowIndex,2,new QTableWidgetItem(s3));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->tableWidget->scrollToBottom();
}

void MainWindow::timeToShowSerialMessage()
{
    QString IDStr = QString::number(serialPortReader->ID);
    QString DLCStr = QString::number(serialPortReader->DLC);
    QString HEXStr = serialPortReader->CANDataMessange.toHex();
    MainWindow::showMessage(IDStr,DLCStr,HEXStr);
}

void MainWindow::showHeartBeat(const QString &s)
{
    qDebug()<<s<<endl;

}

void MainWindow::findPort()
{
    QString temp=ui->serialPortComboBox->currentText();
    int currentIndex=0;
    int intNum=0;
    intNum = ui->serialPortComboBox->count();
    for(;intNum>0;intNum--)ui->serialPortComboBox->removeItem(intNum-1);
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        ui->serialPortComboBox->addItem(info.portName());//查找可用的端口
    }
    currentIndex=ui->serialPortComboBox->findText(temp);
    if(currentIndex!=-1)ui->serialPortComboBox->setCurrentIndex(currentIndex);

    if (!findPortTimer.isActive())
        findPortTimer.start(findPortTimerPeriod);
}

void MainWindow::deBugTimeout()
{
    int x=0;
    int y=0;
    int width=0;
    int heighth=0;
    width=frameGeometry().width();
    heighth=frameGeometry().height();
    x=frameGeometry().x();
    y=frameGeometry().y();
    m_standardOutput<<"main window:"<<"x:"<<x<<endl;
    m_standardOutput<<"main window:"<<"y:"<<y<<endl;
    m_standardOutput<<"main window:"<<"width:"<<width<<endl;
    m_standardOutput<<"main window:"<<"heighth:"<<heighth<<endl;
    if (!deBugTimer.isActive())
        deBugTimer.start(1000);
}

void MainWindow::writerSerialPortFromButton()
{
    QString HEX=ui->lineEdit->text();
    serialPortWriter->ID=ui->lineEdit_2->text().toInt();
    serialPortWriter->DLC=ui->lineEdit_3->text().toInt();
    QByteArray temp=QByteArray::fromStdString(HEX.toStdString());
    serialPortWriter->CANDataMessange=QByteArray::fromHex(temp);
    serialPortWriter->write();
}














