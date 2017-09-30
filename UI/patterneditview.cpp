#include "patterneditview.h"
#include "markparam.h"
#include <QMouseEvent>

PatternEditView::PatternEditView(QWidget* parent):ViewQt(parent)
{
    m_target= new TargetParam;
    m_pressed=false;
    setMouseTracking(true);
}

PatternEditView::~PatternEditView()
{
    delete m_target;
}

void PatternEditView::setTarget(const TargetParam &target)
{
    *m_target=target;
    m_target->x*=m_wRatio;
    m_target->y*=m_hRatio;
    m_target->width*=m_hRatio;
    m_target->length*=m_wRatio;
}

void PatternEditView::getTarget(TargetParam &target)
{
    target=*m_target;
    target.x/=m_wRatio;
    target.y/=m_hRatio;
    target.width/=m_hRatio;
    target.length/=m_wRatio;
}

void PatternEditView::changeToCircle()
{
    m_target->type=TargetParam::Circle;
    update();
}

void PatternEditView::changeToCross()
{
    m_target->type=TargetParam::Cross;
    update();
}

void PatternEditView::changeToPoint()
{
    m_target->type=TargetParam::Point;
    update();
}




void PatternEditView::mousePressEvent(QMouseEvent *event)
{
    m_pressed=true;
}

void PatternEditView::mouseMoveEvent(QMouseEvent *event)
{
    static int oldx=0, oldy=0;
    int x=event->x(), y=event->y();

    if(!m_pressed)
        noPressMouseMove(x, y);
    else
        pressedMouseMove(x, y, x-oldx, y-oldy);
    oldx=x;
    oldy=y;
}

void PatternEditView::paintImage(QImage *image)
{
    if(m_target->type==TargetParam::Cross)
        drawCross(image);
    else if(m_target->type==TargetParam::Circle)
        drawCircle(image);
    else if(m_target->type==TargetParam::Point)
        drawPoint(image);

}

void PatternEditView::drawCross(QImage *image)
{
    QPainter painter(image);
    int l=m_target->length/2;
    int w=m_target->width/2;
    float vx[3]={l, l, w};
    float vy[3]={w, -w, -w};
    float x=m_target->x;
    float y=m_target->y;
    QPointF ps[13];
    int n=0;
    for(int i=0;i<4;i++){
        float angle=m_target->angle+CV_PI/2*i;
        float cosa=cos(angle);
        float sina=-sin(angle);
        for(int j=0;j<3;j++){
            ps[n].rx()=x+cosa*vx[j]-sina*vy[j];
            ps[n].ry()=y+sina*vx[j]+cosa*vy[j];
            n++;
        }
        if(i==0){
            m_crossChangePoint[0].rx()=x+cosa*l;
            m_crossChangePoint[0].ry()=y+sina*l;
            float wl=l-(l-w)/2;
            m_crossChangePoint[1].rx()=x+cosa*wl-sina*-w;
            m_crossChangePoint[1].ry()=y+sina*wl+cosa*-w;
            m_crossChangePoint[2].rx()=x+sina*l;
            m_crossChangePoint[2].ry()=y-cosa*l;
        }

    }
    ps[12]=ps[0];
    painter.setPen(QColor(0, 255, 0));
    painter.drawPolyline(ps, 13);

    for(int i=0;i<3;i++){
        int x=m_crossChangePoint[i].x();
        int y=m_crossChangePoint[i].y();
        if(i<2)
            painter.fillRect(x-2, y-2, 5, 5, QColor(0, 255, 0));
        else
            painter.drawPie(x-2, y-2, 5, 5, 0, 360*16);
    }

}

void PatternEditView::drawCircle(QImage *image)
{
    QPainter painter(image);
    painter.setPen(QColor(0, 255, 0));
    float r=m_target->length/2;
    painter.drawEllipse(QPointF(m_target->x, m_target->y), r, r);

    float vx[4]={0, r, 0, -r};
    float vy[4]={-r, 0, r, 0};
    for(int i=0;i<4;i++){
        int x=m_target->x+vx[i];
        int y=m_target->y+vy[i];
        m_circleChangePoint[i].setX(x);
        m_circleChangePoint[i].setY(y);
        painter.fillRect(x-2, y-2, 5, 5, QColor(0, 255, 0));
    }
}

void PatternEditView::drawPoint(QImage *image)
{
    QPainter painter(image);
    painter.setPen(QColor(255, 0, 0));
    painter.drawLine(QPointF(0, m_target->y), QPointF(width(), m_target->y));
    painter.drawLine(QPointF(m_target->x, 0), QPointF(m_target->x, height()));
}

void PatternEditView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed=false;
}

bool PatternEditView::inRange(const QPoint &p, int x, int y)
{
    return abs(x-p.x())<SensitiveRange && abs(y-p.y())<SensitiveRange;
}

void PatternEditView::noPressMouseMove(int x, int y)
{
    m_changeType=NoChange;
    if(m_target->type==TargetParam::Cross){
        if(inRange(m_crossChangePoint[0], x, y)){
            m_changeType=Lenght;
            setCursor(Qt::SizeHorCursor);
        }
        else if(inRange(m_crossChangePoint[1], x, y)){
            m_changeType=Width;
            setCursor(Qt::SizeVerCursor);
        }
        else if(inRange(m_crossChangePoint[2], x, y)){
            m_changeType=Rotate;
            setCursor(Qt::ForbiddenCursor);
        }
        else if(abs(x-m_target->x)<m_target->width/2 && abs(y-m_target->y)<m_target->width/2){
            m_changeType=Move;
            setCursor(Qt::SizeAllCursor);
        }
        else if(abs(x-m_target->x)<SensitiveRange && abs(y-m_target->y)<SensitiveRange){
            m_changeType=Move;
            setCursor(Qt::SizeAllCursor);
        }
    }
    else if(m_target->type==TargetParam::Circle){
        for(int i=0;i<4;i++){
            if(inRange(m_circleChangePoint[i], x, y)){
                m_changeType=ChangeType(Up+i);
                if(i%2==0)
                    setCursor(Qt::SizeVerCursor);
                else
                    setCursor(Qt::SizeHorCursor);
            }
        }
        if(m_changeType==NoChange){
            int dx=x-m_target->x;
            int dy=y-m_target->y;
            int r=m_target->length/2;
            if(dx*dx+dy*dy<r*r){
                m_changeType=Move;
                setCursor(Qt::SizeAllCursor);
            }
        }

    }
    else{
        if(abs(x-m_target->x)<SensitiveRange && abs(y-m_target->y)<SensitiveRange){
            m_changeType=Move;
            setCursor(Qt::SizeAllCursor);
        }
    }
    if(m_changeType==NoChange)
        setCursor(Qt::ArrowCursor);
}

void PatternEditView::pressedMouseMove(int x, int y, int dx, int dy)
{
    if(m_changeType==Move){
        m_target->x+=dx;
        m_target->y+=dy;
    }
    else if(m_target->type==TargetParam::Circle){
        if(m_changeType==Up){
            m_target->y+=dy/2.0;
            m_target->length+=-dy;
        }
        else if(m_changeType==Down){
            m_target->y+=dy/2.0;
            m_target->length+=dy;
        }
        else if(m_changeType==Left){
            m_target->x+=dx/2.0;
            m_target->length+=-dx;
        }
        else if(m_changeType==Right){
            m_target->x+=dx/2.0;
            m_target->length+=dx;
        }
    }
    else if(m_target->type==TargetParam::Cross){
        if(m_changeType==Lenght){
            m_target->length+=2*dx;
            if(m_target->length<m_target->width)
                m_target->length=m_target->width+1;
        }
        else if(m_changeType==Width){
            m_target->width+=2*-dy;
            if(m_target->width>m_target->length)
                m_target->width=m_target->length-1;
            else if(m_target->width<2)
                m_target->width=2;
        }
        else if(m_changeType==Rotate){
            m_target->angle=-(cvFastArctan(y-m_target->y, x-m_target->x)/180*CV_PI-(-CV_PI/2));
        }
    }

    if(m_changeType!=NoChange && (dx!=0 || dy!=0))
        update();
}












