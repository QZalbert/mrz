/********************************************************************
* Description: markWidget2.h
*   include file for libmarkWidget.
*
* Last change:
*   2013.06.18 create it for simple compile
*
********************************************************************/
#ifndef MARKWIDGET2_H
#define MARKWIDGET2_H

#include <QWidget>

class MarkWidget;

class MarkWidget2: public QWidget
{
    Q_OBJECT
public:
    MarkWidget2(int argc,  char **argv, QWidget* parent=0);
    const char* get_version();
    const char* get_current_project_directory()const;
    QWidget* get_vice_view(int CCDIndex=0);
    int load_project(const char*directory);
    ~MarkWidget2();

signals:
    void project_new(const char* directory);

private slots:
    void emit_project_new(const char* directory);

private:
    MarkWidget* widget;
};





#endif // MARKWIDGET2_H
