#ifndef FOCUSVIEW_H
#define FOCUSVIEW_H
#include "viewQt.h"

class FocusView : public ViewQt
{
public:
    FocusView(QWidget *parent);
    void showImage(IplImage* image, int fx, int fy);
    void setTimes(int val);
    void getViewSize(int& w, int& h);

private:
    int m_times;
};

#endif // FOCUSVIEW_H
