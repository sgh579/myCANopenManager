#ifndef FHD_H
#define FHD_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class fhd : public QDialog
{
    Q_OBJECT

public:
    fhd(QWidget *parent = nullptr);
    ~fhd();
    Ui::Dialog *uiD;

private:

};
#endif // DIALOG_H
