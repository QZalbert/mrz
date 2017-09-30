#ifndef LOADRECORDDIALOG_H
#define LOADRECORDDIALOG_H

#include "ui_loadRecordDialog.h"

class LoadRecordDialog : public QDialog, public Ui::LoadRecordDialog
{
    Q_OBJECT
public:
    LoadRecordDialog(QWidget* parent=0);
    void set_combo_box(const char* currentProject);
    QString get_project_name()const ;

private slots:
    void delete_record();

private:
    void set_combo_box();
    char currentPrj[256];
};


#endif // LOADRECORDDIALOG_H
