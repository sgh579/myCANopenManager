#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QtSerialPort/QSerialPortInfo>
#include "ui_fhd.h"
#include "fhd.h"
#include <typeinfo>
#include <QList>
#include <synchapi.h>


//绘制主页面,完成部分初始化工作
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_standardOutput(stdout)
    , findPortTimerPeriod(1000)
{
    //一段小测试
//    QList<QString> list;
//    list<<"word1"<<"word2"<<"word3";
//    for(int i=0;i<list.size();i++){
//        m_standardOutput<<list[i]<<endl;
//    }

    ui->setupUi(this);

    d =new fhd(this);


    ui->tableWidget->setAlternatingRowColors(true);

    connect(ui->show,&QPushButton::clicked,d,&QWidget::show);
    connect(d,&fhd::fhdToMainwindowDOSignal,this,&MainWindow::buttonTrigger_sendMessage);

    setFixedSize(792,647);
    setWindowTitle(tr("My CANopen Manager"));

    //查找可用的端口
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->serialPortComboBox->addItem(info.portName());

    //备选的波特率
    ui->baudComboBox->addItem(tr("9600"));
    ui->baudComboBox->addItem(tr("115200"));

    //连接串口
    connect(ui->connectPushButton,&QPushButton::clicked,this,&MainWindow::setPortParameter);

    //手动刷新串口
    connect(ui->findPortButton,&QPushButton::clicked,this,&MainWindow::findPort);

    //定时刷新串口
    connect(&findPortTimer,&QTimer::timeout,this,&MainWindow::findPort);


    //发送
    connect(ui->writeButton,&QPushButton::clicked,this,&MainWindow::writerSerialPortFromButton);

    //开启设备
    connect(ui->pushButton_22,&QPushButton::clicked,this,&MainWindow::startSlaveEquipment);

    //debug
    connect(ui->pushButton_21,&QPushButton::clicked,this,&MainWindow::deBugByButton);



    findPortTimer.start(findPortTimerPeriod);

    //一开始没有连接串口的时候,锁起来一些输入控件
    ui->writeButton->setEnabled(0);
    ui->lineEdit->setEnabled(0);
    ui->lineEdit_2->setEnabled(0);
    ui->lineEdit_3->setEnabled(0);


    //设置IO口初始的样子
    setIOCheckBox(1,0,0x01,0x55);

    //设置一个default的equipment
    equipment = new GCAN4055(-1);

}

MainWindow::~MainWindow()
{
    //thread->wait();
    delete ui;
}

//设置串口
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
    {
        qDebug()<<tr("can't open %1 ,error code %2")
                  .arg(serialPort.portName()).arg(serialPort.error())<<endl;
        ui->writeButton->setEnabled(0);
        ui->lineEdit->setEnabled(0);
        ui->lineEdit_2->setEnabled(0);
        ui->lineEdit_3->setEnabled(0);
        return;
    }
    else
        qDebug()<<tr(" open %1 successfully")
                  .arg(serialPort.portName())<<endl;
    serialPortReader = new reader(&serialPort);//serialPortReader对象初始化
    connect(serialPortReader,&reader::timeToShow,this,&MainWindow::timeToShowSerialMessage);
    connect(serialPortReader,&reader::setIOCheckBoxSignal,this,&MainWindow::setIOCheckBox);
    //timeToShowSerialMessage();
    qDebug()<<"reader object built successfully"<<endl;
    serialPortWriter = new writer(&serialPort);
    connect(serialPortWriter,&writer::timeToShowWritten,this,&MainWindow::timeToShowSerialMessageWritten);
    m_standardOutput<<"writer object built successfully"<<endl;
    //thread->transaction(ui->serialPortComboBox->currentText(),serialPortBaudRate);
    //thread->start();
    ui->writeButton->setEnabled(1);
    ui->lineEdit->setEnabled(1);
    ui->lineEdit_2->setEnabled(1);
    ui->lineEdit_3->setEnabled(1);


}

//在表格中显示报文信息
void MainWindow::showMessage(const QString &s1,const QString &s2,const QString &s3,int direction)
{
    int rowIndex=ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowIndex);
    ui->tableWidget->setItem(rowIndex,0,new QTableWidgetItem(s1));
    ui->tableWidget->setItem(rowIndex,1,new QTableWidgetItem(s2));
    ui->tableWidget->setItem(rowIndex,2,new QTableWidgetItem(s3));
    if(direction==1)ui->tableWidget->setItem(rowIndex,3,new QTableWidgetItem("发送"));
    if(direction==0)ui->tableWidget->setItem(rowIndex,3,new QTableWidgetItem("接收"));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽
    ui->tableWidget->scrollToBottom();
}

//由发送串口信息时发出的信号触发,在表格中打印出对应的信息
void MainWindow::timeToShowSerialMessageWritten()
{
    QString IDStr = serialPortWriter->ID.toHex();
    QString DLCStr = QString::number(serialPortWriter->DLC);
    QString HEXStr = serialPortWriter->CANDataMessange.toHex();
    showMessage(IDStr,DLCStr,HEXStr,1);

}

//由接收到串口信息时发出的信号触发,在表格中打印出对应的信息
void MainWindow::timeToShowSerialMessage()
{
    QString IDStr = serialPortReader->ID.toHex();
    QString DLCStr = QString::number(serialPortReader->DLC);
    QString HEXStr = serialPortReader->CANDataMessange.toHex();
    QByteArray temp=QByteArray::fromHex("7f05");

    //m_standardOutput<<"IDStr.toInt()/100"<<(IDStr.toInt()/100)<<endl;

    if((IDStr.toInt()/100==7) && serialPortReader->CANDataMessange[0]==temp[0])//发现处于预操作态的设备
    {
        int idTemp=IDStr.toInt()-700;//设备ID
        equipment = new GCAN4055(idTemp);
        ui->label_2->setText(QString::number(idTemp));
        ui->label_4->setText(tr("预操作"));
    }
    if((IDStr.toInt()/100==7)  && serialPortReader->CANDataMessange[0]==temp[1])//发现处于操作态的设备
    {
        equipment->status=GCAN4055::status_type::operational;
        ui->label_4->setText(tr("操作"));
    }
    if(IDStr.toInt()==(180+equipment->ID))//TPDO
    {
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x1);
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x2)>>1;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x4)>>2;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x8)>>3;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x10)>>4;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x20)>>5;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x40)>>6;
        equipment->DI[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x80)>>7;
        setIOCheckBox(1,0,serialPortReader->CANDataMessange[0],0);
    }
    if(IDStr.toInt()==(200+equipment->ID))//TPDO
    {
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x1);
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x2)>>1;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x4)>>2;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x8)>>3;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x10)>>4;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x20)>>5;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x40)>>6;
        equipment->DO[0]=(uchar)(serialPortReader->CANDataMessange[0] & 0x80)>>7;
        setIOCheckBox(0,1,0,serialPortReader->CANDataMessange[0]);
    }


    showMessage(IDStr,DLCStr,HEXStr,0);
}

//寻找计算机可用的端口
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

//由按钮触发打印调试信息
void MainWindow::deBugByButton()
{
    m_standardOutput<<"deBugByButton"<<endl;
//    QString buttonstate = d->uiD->DO0->checkState();
    m_standardOutput<<"buttonstate"<<d->uiD->DO0->checkState()<<endl;
}

//设置一个定时器,定时打印调试信息
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

//根据文本编辑的内容发送can报文
void MainWindow::writerSerialPortFromButton()
{
    QString HEX=ui->lineEdit->text();
    QString IDStr=ui->lineEdit_2->text();
    //serialPortWriter->ID=ui->lineEdit_2->text()();
    QByteArray temp1=QByteArray::fromStdString(IDStr.toStdString());
    serialPortWriter->ID=QByteArray::fromHex(temp1);
    serialPortWriter->DLC=ui->lineEdit_3->text().toInt();
    QByteArray temp2=QByteArray::fromStdString(HEX.toStdString());
    serialPortWriter->CANDataMessange=QByteArray::fromHex(temp2);
    serialPortWriter->write();
}


//发出启动设备的命令,由按键触发
void MainWindow::startSlaveEquipment()
{
    QString HEX="0100";
    QString IDStr="0000";
    QByteArray temp1=QByteArray::fromStdString(IDStr.toStdString());
    QByteArray temp2=QByteArray::fromStdString(HEX.toStdString());
    serialPortWriter->ID=QByteArray::fromHex(temp1);
    serialPortWriter->DLC=2;
    serialPortWriter->CANDataMessange=QByteArray::fromHex(temp2);
    serialPortWriter->write();

}

//根据DOColor，DIColor的二进制每位上的0和1来确定输入输出的状态，用来绘制示意图
void MainWindow::setIOCheckBox(bool DOChange,bool DIChange,uchar DOColor,uchar DIColor)
{
    if(DIChange){
        //DI 0
        if((uchar)(0x01 & DIColor))d->uiD->DI0->setCheckState(Qt::Checked);
        else d->uiD->DI0->setCheckState(Qt::Unchecked);
        //DI 1
        if((uchar)(0x02 & DIColor))d->uiD->DI1->setCheckState(Qt::Checked);
        else d->uiD->DI1->setCheckState(Qt::Unchecked);
        //DI 2
        if((uchar)(0x04 & DIColor))d->uiD->DI2->setCheckState(Qt::Checked);
        else d->uiD->DI2->setCheckState(Qt::Unchecked);
        //DI 3
        if((uchar)(0x08 & DIColor))d->uiD->DI3->setCheckState(Qt::Checked);
        else d->uiD->DI3->setCheckState(Qt::Unchecked);
        //DI 4
        if((uchar)(0x010 & DIColor))d->uiD->DI4->setCheckState(Qt::Checked);
        else d->uiD->DI4->setCheckState(Qt::Unchecked);
        //DI 5
        if((uchar)(0x020 & DIColor))d->uiD->DI5->setCheckState(Qt::Checked);
        else d->uiD->DI5->setCheckState(Qt::Unchecked);
        //DI 6
        if((uchar)(0x040 & DIColor))d->uiD->DI6->setCheckState(Qt::Checked);
        else d->uiD->DI6->setCheckState(Qt::Unchecked);
        //DI 7
        if((uchar)(0x080 & DIColor))d->uiD->DI7->setCheckState(Qt::Checked);
        else d->uiD->DI7->setCheckState(Qt::Unchecked);
    }
    if(DOChange)
    {
        //DO 0
        if((uchar)(0x01 & DOColor))d->uiD->DO0->setCheckState(Qt::Checked);
        else d->uiD->DO0->setCheckState(Qt::Unchecked);
        //DO 1
        if((uchar)(0x02 & DOColor))d->uiD->DO1->setCheckState(Qt::Checked);
        else d->uiD->DO1->setCheckState(Qt::Unchecked);
        //DO 2
        if((uchar)(0x04 & DOColor))d->uiD->DO2->setCheckState(Qt::Checked);
        else d->uiD->DO2->setCheckState(Qt::Unchecked);
        //DO 3
        if((uchar)(0x08 & DOColor))d->uiD->DO3->setCheckState(Qt::Checked);
        else d->uiD->DO3->setCheckState(Qt::Unchecked);
        //DO 4
        if((uchar)(0x010 & DOColor))d->uiD->DO4->setCheckState(Qt::Checked);
        else d->uiD->DO4->setCheckState(Qt::Unchecked);
        //DO 5
        if((uchar)(0x020 & DOColor))d->uiD->DO5->setCheckState(Qt::Checked);
        else d->uiD->DO5->setCheckState(Qt::Unchecked);
        //DO 6
        if((uchar)(0x040 & DOColor))d->uiD->DO6->setCheckState(Qt::Checked);
        else d->uiD->DO6->setCheckState(Qt::Unchecked);
        //DO 7
        if((uchar)(0x080 & DOColor))d->uiD->DO7->setCheckState(Qt::Checked);
        else d->uiD->DO7->setCheckState(Qt::Unchecked);
    }

}


// 发送RPDO（对于从站而言）
// 入口参数:uchar DOSetState
// 按二进制位对于8位DO
// 解析DOSetState,发送报文
// 报文格式:
// 帧ID:0x200 + NodeID
// DLC:1
// HEX:DOSetState
void MainWindow::buttonTrigger_sendMessage(uchar DOSetState)
{
    int IDInt = 200+equipment->ID;
    QString IDStr = QString::number(IDInt);
    QString DLCStr = "01";
    QString HEXStr = QString::number(DOSetState);
    m_standardOutput<<"DOSetState"<<DOSetState<<endl;
    QByteArray temp1=QByteArray::fromStdString(IDStr.toStdString());
    serialPortWriter->ID=QByteArray::fromHex(temp1);
    serialPortWriter->DLC=1;
    QByteArray temp2=QByteArray::fromStdString(HEXStr.toStdString());
    serialPortWriter->CANDataMessange=QByteArray::fromHex(temp2);
    serialPortWriter->write();
}






