#include "markview.h"
#include <QMouseEvent>

MarkView::MarkView(QWidget *parent)
    :ViewQt(parent)
{
    m_fps=-1;
    m_fx=width()/2;
    m_fy=height()/2;
    m_fw=m_fh=0;
    m_showFocus=false;
    m_showSel=false;
    m_press=false;
    m_patternOk=false;
    m_targetOk=false;
    m_showCircle=false;
    m_showCross=false;
    m_showPoint=false;
    reset();
    m_changeType= NoChange;
    m_selRect.setRect(width()/2-50, height()/2-50, 100, 100);
    setMouseTracking(true);
}

void MarkView::showFps(float fps)
{
    m_fps=fps;
}

void MarkView::showFocusRect(bool show, int w, int h)
{
    m_showFocus=show;
    m_fw=w*m_wRatio;
    m_fh=h*m_hRatio;
}

void MarkView::getFocusPos(int &fx, int &fy)
{
    fx=m_fx/m_wRatio;
    fy=m_fy/m_hRatio;
}

void MarkView::selectPattern()
{
    m_showSel=true;
}

void MarkView::selectDone(CvRect& rect)
{
    m_showSel=false;
    rect.x=m_selRect.x()/m_wRatio;
    rect.y=m_selRect.y()/m_hRatio;
    rect.width=m_selRect.width()/m_wRatio;
    rect.height=m_selRect.height()/m_hRatio;
}

void MarkView::reset()
{
    for(int i=0;i<MAX_MARK_NUM;i++){
        m_detectOk[i]=0;
    }
    m_patternOk=false;
    m_targetOk=false;
}

void MarkView::mousePressEvent(QMouseEvent *event)
{
    int x=event->x(), y=event->y();
    if(m_showFocus){
        m_fx=x;
        m_fy=y;
    }
    if(m_showSel)
        m_press=true;
}

void MarkView::mouseMoveEvent(QMouseEvent *event)
{
    static int oldx, oldy;
    int x=event->x(), y=event->y();
    if(m_showSel){
        if(!m_press){
            m_changeType=NoChange;
            for(int i=0;i<4;i++){
                if(abs(x-m_selPx[i])<SensitiveRange && abs(y-m_selPy[i])<SensitiveRange){
                    m_changeType=ChangeType(i);
                    if(i%2==0)
                        setCursor(Qt::SizeVerCursor);
                    else
                        setCursor(Qt::SizeHorCursor);
                    break;
                }
            }
            if(m_changeType==NoChange){
                if(x>m_selRect.x() && y>m_selRect.y() &&
                        x<m_selRect.bottomRight().x() &&
                        y<m_selRect.bottomRight().y()){
                    m_changeType=Move;
                    setCursor(Qt::SizeAllCursor);
                }
            }
            if(m_changeType==NoChange)
                setCursor(Qt::ArrowCursor);
        }
        else{
            if(m_changeType==Move){
                m_selRect.setRect(m_selRect.x()+x-oldx, m_selRect.y()+y-oldy,
                                  m_selRect.width(), m_selRect.height());
            }
            else if(m_changeType==Left)
                m_selRect.setLeft(x);
            else if(m_changeType==Up)
                m_selRect.setTop(y);
            else if(m_changeType==Right)
                m_selRect.setRight(x);
            else if(m_changeType==Down)
                m_selRect.setBottom(y);

            if(m_changeType!=NoChange && (x!=oldx || y!=oldy))
                update();
        }
    }
    oldx=x;
    oldy=y;
}

void MarkView::mouseReleaseEvent(QMouseEvent *event)
{
    m_press=false;
}

void MarkView::paintImage(QImage *img)
{
    drawCamerState(img);
    if(m_showFocus)
        drawFocusRect(img);
    if(m_showSel)
        drawSelectRect(img);
    if(m_patternOk)
        drawPattern(img);
    if(m_targetOk)
        drawTarget(img);
    drawResult(img);
}

void MarkView::drawFocusRect(QImage *img)
{
    QPainter painter(img);
    painter.setPen(QColor(255, 0, 0));
    painter.drawRect(m_fx-m_fw/2, m_fy-m_fh/2, m_fw, m_fh);
}

void MarkView::drawSelectRect(QImage *image)
{
    QPainter painter(image);
    painter.setPen(QColor(0, 255, 0));
    painter.drawRect(m_selRect);
    float vx[4]={0.5, 1, 0.5, 0};
    float vy[4]={0, 0.5, 1, 0.5};
    for(int i=0;i<4;i++){
        int x=m_selRect.x()+vx[i]*m_selRect.width();
        int y=m_selRect.y()+vy[i]*m_selRect.height();
        m_selPx[i]=x;
        m_selPy[i]=y;
        painter.fillRect(x-2, y-2, 5, 5, QColor(0, 255, 0));
    }
}

void MarkView::drawPattern(QImage *image)
{
    QPainter painter(image);
    painter.setPen(QColor(0, 255, 0));
    painter.drawRect(m_rect);
}

void MarkView::drawTarget(QImage *image)
{
    QPainter painter(image);
    if(m_showPoint||m_showCircle){
        painter.setPen(QColor(255, 0, 0));
        QPointF p1=m_pos;
        QPointF p2=m_pos;
        p1.rx()=p1.x()-20;
        p2.rx()=p2.x()+20;
        painter.drawLine(p1, p2);
        p1=m_pos;
        p2=m_pos;
        p1.ry()=p1.y()-20;
        p2.ry()=p2.y()+20;
        painter.drawLine(p1, p2);
        if(m_showCircle){
            painter.setPen(QColor(0, 255, 0));
            painter.drawEllipse(m_pos, m_hr, m_vr);
        }
    }
    else if(m_showCross){
        painter.setPen(QColor(0, 255, 0));
        for(int i=0;i<4;i++)
            painter.drawLine(m_edgeLine[i][0], m_edgeLine[i][1]);
        painter.setPen(QColor(255, 0, 0));
        for(int i=0;i<2;i++)
            painter.drawLine(m_midLine[i][0], m_midLine[i][1]);
    }
}

void MarkView::drawResult(QImage *image)
{
    char text[64];
    QPainter painter(image);
    painter.setFont(QFont("Sans",10,75));
    int px[MAX_MARK_NUM]={5, width()/2+5, 5, width()/2+5};
    int py[MAX_MARK_NUM]={20, 20, 50, 50};
    for(int i=0;i<MAX_MARK_NUM;i++){
        if(m_detectOk[i]==0){
            painter.setPen(QColor(250, 200, 0));
            sprintf(text, "Mark%d:未识别", i+1);
        }
        else if(m_detectOk[i]<0){
            painter.setPen(QColor(255, 0, 0));
            sprintf(text, "Mark%d:识别失败", i+1);
        }
        else{
            painter.setPen(QColor(0, 200, 50));
            sprintf(text, "Mark%d:(%.3f, %.3f)", i+1, m_actualPos[i].x, m_actualPos[i].y);
        }
        painter.drawText(px[i], py[i], QString::fromUtf8(text));
    }
}

void MarkView::drawCamerState(QImage* qImage){
    char buf[64];

    QPainter painter(qImage);
    painter.setFont(QFont("Sans",10,75));
    if(m_fps<0){
        painter.setPen(QColor(255,0,0));
        strcpy(buf, "相机未连接");
    }
    else if(m_fps<5){
        painter.setPen(QColor(255,0,0));
        strcpy(buf, "相机通信出错");
    }
    else{
        painter.setPen(QColor(0,255,50));
        sprintf(buf, "FPS:%.1f", m_fps);
    }
    painter.drawText(5, height()-5, QString::fromUtf8(buf));
}

void MarkView::setDetectOk(bool patternOk, bool targetOk)
{
    m_patternOk=patternOk;
    m_targetOk=targetOk;
}

void MarkView::showRect(const CvRect &rect)
{
    m_rect.setX(rect.x*m_wRatio);
    m_rect.setY(rect.y*m_hRatio);
    m_rect.setWidth(rect.width*m_wRatio);
    m_rect.setHeight(rect.height*m_hRatio);
}

void MarkView::showPointTarget(CvPoint2D32f point)
{
    m_showPoint=true;
    m_showCircle=false;
    m_showCross=false;
    m_pos.rx()=point.x*m_wRatio;
    m_pos.ry()=point.y*m_hRatio;
}

void MarkView::showCircleTarget(CvPoint2D32f center, double r)
{
    m_showPoint=false;
    m_showCircle=true;
    m_showCross=false;
    m_pos.rx()=center.x*m_wRatio;
    m_pos.ry()=center.y*m_hRatio;
    m_hr=r*m_wRatio;
    m_vr=r*m_hRatio;
}

void MarkView::showCrossTarget(CvPoint2D32f edgeLine[][2], CvPoint2D32f midLine[][2])
{
    m_showPoint=false;
    m_showCircle=false;
    m_showCross=true;
    for(int i=0;i<4;i++){
        for(int j=0;j<2;j++){
            m_edgeLine[i][j].rx()=edgeLine[i][j].x*m_wRatio;
            m_edgeLine[i][j].ry()=edgeLine[i][j].y*m_hRatio;
        }
    }
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            m_midLine[i][j].rx()=midLine[i][j].x*m_wRatio;
            m_midLine[i][j].ry()=midLine[i][j].y*m_hRatio;
        }
    }
}

void MarkView::showActualPos(int index, bool ok, CvPoint2D32f actualPos)
{
    m_detectOk[index]=ok?1:-1;
    m_actualPos[index]=actualPos;
}






