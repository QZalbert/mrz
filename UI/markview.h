#ifndef MARKVIEW_H
#define MARKVIEW_H
#include "viewQt.h"
#include "cv.h"
#include "global.h"

class MarkView : public ViewQt
{
public:
    MarkView(QWidget *parent = 0);
    void showFps(float fps);
    void showFocusRect(bool show, int w=0, int h=0);
    void getFocusPos(int& fx, int& fy);
    void selectPattern();
    void selectDone(CvRect& rect);
    void reset();
    void showActualPos(int index, bool ok, CvPoint2D32f actualPos);
    void setDetectOk(bool patternOk, bool targetOk);
    void showRect(const CvRect& rect);
    void showPointTarget(CvPoint2D32f point);
    void showCircleTarget(CvPoint2D32f center, double r);
    void showCrossTarget(CvPoint2D32f edgeLine[4][2], CvPoint2D32f midLine[2][2]);


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    virtual void paintImage(QImage*);
    void drawFocusRect(QImage* img);
    void drawSelectRect(QImage* image);
    void drawPattern(QImage* image);
    void drawTarget(QImage* image);
    void drawResult(QImage* image);
    void drawCamerState(QImage *qImage);
    static const int SensitiveRange=10;

    float m_fps;
    bool m_showFocus;
    int m_fx, m_fy;
    float m_fw, m_fh;

    bool m_showSel;
    bool m_press;
    QRect m_selRect;
    typedef enum{NoChange=-1, Up, Right, Down, Left, Move} ChangeType;
    ChangeType m_changeType;
    int m_selPx[4];
    int m_selPy[4];

    int m_detectOk[MAX_MARK_NUM];
    CvPoint2D32f m_actualPos[MAX_MARK_NUM];


    bool m_patternOk;
    bool m_targetOk;
    bool m_showCircle;
    bool m_showCross;
    bool m_showPoint;

    QRectF m_rect;
    QPointF m_pos;
    double m_hr, m_vr;
    QPointF m_edgeLine[4][2];
    QPointF m_midLine[2][2];
};

#endif // MARKVIEW_H
