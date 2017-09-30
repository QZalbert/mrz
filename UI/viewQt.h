#ifndef VIEW_QT_H
#define VIEW_QT_H

#include <QWidget>
#include <QGraphicsView>

typedef struct _IplImage IplImage;

class ViewQt: public QWidget
{
    Q_OBJECT
public:
	ViewQt(QWidget *parent = 0);
	void receive_image(const IplImage* img);
	virtual ~ViewQt();

protected:
	void paintEvent(QPaintEvent *event);
    virtual void paintImage(QImage*){}
    void resizeEvent(QResizeEvent *);
	IplImage* m_imageBuf;
	IplImage* m_resizeImg;
	IplImage* m_rgbImg;
	QImage* m_paintQImage;
	float m_wRatio;
	float m_hRatio;
	bool m_imageChanged;
};

#endif


