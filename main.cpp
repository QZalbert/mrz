#include <QtGui/QApplication>
#include "markWidget2.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MarkWidget2 w(argc, argv);
    w.show();
//    QWidget* viceView= w.get_vice_view();
//    viceView->show();
    return a.exec();
}
