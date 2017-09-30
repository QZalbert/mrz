#include "viewQt.h"
#include <cv.h>


ViewQt::ViewQt(QWidget *parent)
	:QWidget(parent)
{
	m_imageBuf=NULL;
    setMinimumSize(parent->width(), parent->height());
	resize(parent->width()-2, parent->height()-2);
	m_resizeImg=cvCreateImage(cvSize(width(), height()), 8, 1);
	m_rgbImg=cvCreateImageHeader(cvSize(width(), height()), 8, 3);
	m_paintQImage=new QImage(width(), height(), QImage::Format_RGB888);
	m_rgbImg->imageData=(char*)m_paintQImage->bits();
    cvZero(m_resizeImg);
	m_wRatio=1;
	m_hRatio=1;
	m_imageChanged=false;
}

ViewQt::~ViewQt(){
	if(m_imageBuf)
		cvReleaseImage(&m_imageBuf);
	if(m_resizeImg)
		cvReleaseImage(&m_resizeImg);
	if(m_rgbImg)
		cvReleaseImageHeader(&m_rgbImg);
}


void ViewQt::receive_image(const IplImage *img){
	assert(img);
	assert(img->nChannels==1 && img->depth==8);

	if(!m_imageBuf)
		m_imageBuf=cvCreateImage(cvGetSize(img), 8, 1);
	else if(m_imageBuf->width!=img->width || m_imageBuf->height!=img->height){
		cvReleaseImage(&m_imageBuf);
		m_imageBuf=cvCreateImage(cvGetSize(img), 8, 1);
	}
	cvCopyImage(img, m_imageBuf);
	m_imageChanged=true;
    m_wRatio=1.0*width()/m_imageBuf->width;
    m_hRatio=1.0*height()/m_imageBuf->height;
	update();
}



void ViewQt::paintEvent(QPaintEvent *){

	if(m_imageChanged){
		if(m_resizeImg->width!=width() || m_resizeImg->height!=height()){
			cvReleaseImage(&m_resizeImg);
			cvReleaseImageHeader(&m_rgbImg);
			delete m_paintQImage;
			m_resizeImg=cvCreateImage(cvSize(width(), height()), 8, 1);
			m_rgbImg=cvCreateImageHeader(cvSize(width(), height()), 8, 3);
			m_paintQImage=new QImage(width(), height(), QImage::Format_RGB888);
			m_rgbImg->imageData=(char*)m_paintQImage->bits();
		}
		if(m_imageBuf){
			cvResize(m_imageBuf, m_resizeImg, CV_INTER_NN);
            m_wRatio=1.0*width()/m_imageBuf->width;
            m_hRatio=1.0*height()/m_imageBuf->height;
		}
		m_imageChanged=false;
	}

	//draw paintQImage
	cvCvtColor(m_resizeImg, m_rgbImg, CV_GRAY2RGB);

    //
    paintImage(m_paintQImage);

	//draw pattern-frame and search-frame 
	QPainter painter(this);
    painter.drawImage(QPoint(0,0),*m_paintQImage);
}

void ViewQt::resizeEvent(QResizeEvent *)
{

}
