#ifndef PSWDIALOG_H
#define PSWDIALOG_H

#include <QDialog>

namespace Ui {
class PswDialog;
}

class PswDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PswDialog(QWidget *parent = 0);
    ~PswDialog();
    void set_adjust_mode();
    void set_license_mode(const char* snText);
    QString get_enter_key()const;
    
private:
    Ui::PswDialog *ui;
};

#endif // PSWDIALOG_H
