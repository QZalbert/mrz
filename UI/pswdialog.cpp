#include "pswdialog.h"
#include "ui_pswdialog.h"

PswDialog::PswDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PswDialog)
{
    ui->setupUi(this);
}

PswDialog::~PswDialog()
{
    delete ui;
}

void PswDialog::set_adjust_mode(){
    this->setWindowTitle(QString::fromUtf8("校正密码"));
    ui->lb_sn->hide();
    ui->le_psw->clear();
    ui->le_psw->setEchoMode(QLineEdit::Password);
    ui->le_psw->setFocus(Qt::MouseFocusReason);
}

void PswDialog::set_license_mode(const char* snText){
    this->setWindowTitle(QString::fromUtf8("使用授权"));
    ui->lb_sn->show();
    char snDisp[256];
    const char* psn=snText;
    char* psnDisp=snDisp;
    int n=0;
    while(*psn){
        *psnDisp++=*psn++;
        n++;
        if(n==4){
            *psnDisp++=' ';
            n=0;
        }
    }
    *psnDisp=0;

    QString snString=QString::fromUtf8("序列号: ")+QString::fromUtf8(snDisp);
    ui->lb_sn->setText(snString);
    ui->le_psw->clear();
    ui->le_psw->setEchoMode(QLineEdit::Normal);
    ui->le_psw->setFocus(Qt::MouseFocusReason);
}

QString PswDialog::get_enter_key()const{
    return ui->le_psw->text();
}
