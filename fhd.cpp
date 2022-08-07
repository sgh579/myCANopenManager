#include "fhd.h"
#include "ui_fhd.h"

fhd::fhd(QWidget *parent)
    : QDialog(parent)
    , uiD(new Ui::Dialog)
{
    uiD->setupUi(this);

    //连接设备小窗口按钮和发送函数
    connect(uiD->setButton,&QPushButton::clicked,this,&fhd::pushButtonSetDO);

}

fhd::~fhd()
{
    delete uiD;
}

void fhd::pushButtonSetDO()
{
    //检测自己的checkbox，得到一个uchar
    uchar DOCheckbox = 0x00;
    if(uiD->DO0->isChecked())DOCheckbox|=0x01;
    else DOCheckbox&=0xfe;

    if(uiD->DO1->isChecked())DOCheckbox|=0x02;
    else DOCheckbox&=0xfd;

    if(uiD->DO2->isChecked())DOCheckbox|=0x04;
    else DOCheckbox&=0xfb;

    if(uiD->DO3->isChecked())DOCheckbox|=0x08;
    else DOCheckbox&=0xf7;

    if(uiD->DO4->isChecked())DOCheckbox|=0x10;
    else DOCheckbox&=0xef;

    if(uiD->DO5->isChecked())DOCheckbox|=0x20;
    else DOCheckbox&=0xdf;

    if(uiD->DO6->isChecked())DOCheckbox|=0x40;
    else DOCheckbox&=0xbf;

    if(uiD->DO7->isChecked())DOCheckbox|=0x80;
    else DOCheckbox&=0x7f;


    //发射信号
    emit fhdToMainwindowDOSignal(DOCheckbox);
}
