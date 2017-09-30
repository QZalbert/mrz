#include "markwidget.h"
#include "markWidget2.h"


MarkWidget2::MarkWidget2(int argc, char **argv, QWidget *parent):QWidget(parent){
    widget=new MarkWidget(argc>1 ,this);
    connect(widget, SIGNAL(sendInfo(const char*)), SIGNAL(project_new(const char*)));
}

MarkWidget2::~MarkWidget2(){
    delete widget;
}

const char*  MarkWidget2::get_version(){
   // return widget->get_version();
    return NULL;
}

const char* MarkWidget2:: get_current_project_directory()const{
    //return widget->get_current_project_directory();
    return NULL;
}

QWidget* MarkWidget2::get_vice_view(int ){
    return widget->getViceView();
}

int MarkWidget2::load_project(const char*directory){
    return widget->receiveInfo(directory);
}

void MarkWidget2::emit_project_new(const char *directory){
}
