#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
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
    ui->writeButton->setEnabled(0);
    ui->lineEdit->setEnabled(0);
    ui->lineEdit_2->setEnabled(0);
    ui->lineEdit_3->setEnabled(0);

//设置样式
    {
        connect(ui->checkBox,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox->setCheckState(Qt::Checked);
            else
                ui->checkBox->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_2,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_2->setCheckState(Qt::Checked);
            else
                ui->checkBox_2->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_3,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_3->setCheckState(Qt::Checked);
            else
                ui->checkBox_3->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_4,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_4->setCheckState(Qt::Checked);
            else
                ui->checkBox_4->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_5,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_5->setCheckState(Qt::Checked);
            else
                ui->checkBox_5->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_6,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_6->setCheckState(Qt::Checked);
            else
                ui->checkBox_6->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_7,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_7->setCheckState(Qt::Checked);
            else
                ui->checkBox_7->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_8,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_8->setCheckState(Qt::Checked);
            else
                ui->checkBox_8->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_9,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_9->setCheckState(Qt::Checked);
            else
                ui->checkBox_9->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_10,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_10->setCheckState(Qt::Checked);
            else
                ui->checkBox_10->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_11,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_11->setCheckState(Qt::Checked);
            else
                ui->checkBox_11->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_12,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_12->setCheckState(Qt::Checked);
            else
                ui->checkBox_12->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_13,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_13->setCheckState(Qt::Checked);
            else
                ui->checkBox_13->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_14,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_14->setCheckState(Qt::Checked);
            else
                ui->checkBox_14->setCheckState(Qt::Unchecked);
        });

        connect(ui->checkBox_15,&QCheckBox::stateChanged,[=](int state){
            if (state==2)
                ui->checkBox_15->setCheckState(Qt::Checked);
            else
                ui->checkBox_15->setCheckState(Qt::Unchecked);
        });


    }


    //setIOCheckBox(0xaa,0x55);
    setIOCheckBox(1,0,0xaa,0x55);


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
    m_standardOutput<<"writer object built successfully"<<endl;
    //thread->transaction(ui->serialPortComboBox->currentText(),serialPortBaudRate);
    //thread->start();
    ui->writeButton->setEnabled(1);
    ui->lineEdit->setEnabled(1);
    ui->lineEdit_2->setEnabled(1);
    ui->lineEdit_3->setEnabled(1);


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
    QString IDStr = serialPortReader->ID.toHex();
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
    QString IDStr=ui->lineEdit_2->text();
    //serialPortWriter->ID=ui->lineEdit_2->text()();
    QByteArray temp1=QByteArray::fromStdString(IDStr.toStdString());
    serialPortWriter->ID=QByteArray::fromHex(temp1);
    serialPortWriter->DLC=ui->lineEdit_3->text().toInt();
    QByteArray temp2=QByteArray::fromStdString(HEX.toStdString());
    serialPortWriter->CANDataMessange=QByteArray::fromHex(temp2);
    serialPortWriter->write();
}


//void MainWindow::setBulbButtonState(uchar DOColor,uchar DIColor)
//{

//}

void MainWindow::setIOCheckBox(bool DOChange,bool DIChange,uchar DOColor,uchar DIColor)
{
    if(DIChange){
        //DI 0
        if((uchar)(0x01 & DIColor))ui->checkBox_9->setCheckState(Qt::Checked);
        else ui->checkBox_9->setCheckState(Qt::Unchecked);
        //DI 1
        if((uchar)(0x02 & DIColor))ui->checkBox_16->setCheckState(Qt::Checked);
        else ui->checkBox_16->setCheckState(Qt::Unchecked);
        //DI 2
        if((uchar)(0x04 & DIColor))ui->checkBox_10->setCheckState(Qt::Checked);
        else ui->checkBox_10->setCheckState(Qt::Unchecked);
        //DI 3
        if((uchar)(0x08 & DIColor))ui->checkBox_11->setCheckState(Qt::Checked);
        else ui->checkBox_11->setCheckState(Qt::Unchecked);
        //DI 4
        if((uchar)(0x010 & DIColor))ui->checkBox_12->setCheckState(Qt::Checked);
        else ui->checkBox_12->setCheckState(Qt::Unchecked);
        //DI 5
        if((uchar)(0x020 & DIColor))ui->checkBox_13->setCheckState(Qt::Checked);
        else ui->checkBox_13->setCheckState(Qt::Unchecked);
        //DI 6
        if((uchar)(0x040 & DIColor))ui->checkBox_14->setCheckState(Qt::Checked);
        else ui->checkBox_14->setCheckState(Qt::Unchecked);
        //DI 7
        if((uchar)(0x080 & DIColor))ui->checkBox_15->setCheckState(Qt::Checked);
        else ui->checkBox_15->setCheckState(Qt::Unchecked);
    }
    if(DOChange)
    {
        //DO 0
        if((uchar)(0x01 & DOColor))ui->checkBox->setCheckState(Qt::Checked);
        else ui->checkBox->setCheckState(Qt::Unchecked);
        //DO 1
        if((uchar)(0x02 & DOColor))ui->checkBox_2->setCheckState(Qt::Checked);
        else ui->checkBox_2->setCheckState(Qt::Unchecked);
        //DO 2
        if((uchar)(0x04 & DOColor))ui->checkBox_3->setCheckState(Qt::Checked);
        else ui->checkBox_3->setCheckState(Qt::Unchecked);
        //DO 3
        if((uchar)(0x08 & DOColor))ui->checkBox_4->setCheckState(Qt::Checked);
        else ui->checkBox_4->setCheckState(Qt::Unchecked);
        //DO 4
        if((uchar)(0x010 & DOColor))ui->checkBox_5->setCheckState(Qt::Checked);
        else ui->checkBox_5->setCheckState(Qt::Unchecked);
        //DO 5
        if((uchar)(0x020 & DOColor))ui->checkBox_6->setCheckState(Qt::Checked);
        else ui->checkBox_6->setCheckState(Qt::Unchecked);
        //DO 6
        if((uchar)(0x040 & DOColor))ui->checkBox_7->setCheckState(Qt::Checked);
        else ui->checkBox_7->setCheckState(Qt::Unchecked);
        //DO 7
        if((uchar)(0x080 & DOColor))ui->checkBox_8->setCheckState(Qt::Checked);
        else ui->checkBox_8->setCheckState(Qt::Unchecked);
    }

}










