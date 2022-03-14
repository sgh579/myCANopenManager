#include "fhd.h"
#include "ui_fhd.h"

fhd::fhd(QWidget *parent)
    : QDialog(parent)
    , uiD(new Ui::Dialog)
{
    uiD->setupUi(this);

}

fhd::~fhd()
{
    delete uiD;
}
