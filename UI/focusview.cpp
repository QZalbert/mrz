#include "focusview.h"
#include <cv.h>

FocusView::FocusView(QWidget *parent)
    :ViewQt(parent)
{
    m_times=4;
}

void FocusView::showImage(IplImage *image, int fx, int fy)
{
    int w, h;
    getViewSize(w, h);
    IplImage* focusImage=cvCreateImage(cvSize(w, h), 8, 1);
    IplImage* tmpImage=cvCreateImage(cvSize(width(), height()), 8, 1);
    cvSetImageROI(image, cvRect(fx-w/2, fy-h/2, w, h));
    cvCopy(image, focusImage);
    cvResetImageROI(image);
    cvResize(focusImage, tmpImage, CV_INTER_NN);
    receive_image(tmpImage);
    cvReleaseImage(&focusImage);
    cvReleaseImage(&tmpImage);
}


void FocusView::setTimes(int val)
{
    m_times=val;
}

void FocusView::getViewSize(int &w, int &h)
{
    w=width()/m_times;
    h=height()/m_times;
}
