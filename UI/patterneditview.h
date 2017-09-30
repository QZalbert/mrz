#ifndef PATTERNEDITVIEW_H
#define PATTERNEDITVIEW_H
#include "viewQt.h"
struct TargetParam;

class PatternEditView : public ViewQt
{
public:
    PatternEditView(QWidget* parent);
    ~PatternEditView();
    void setTarget(const TargetParam& target);
    void getTarget(TargetParam& target);
    void changeToCircle();
    void changeToCross();
    void changeToPoint();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    virtual void paintImage(QImage* image);
    void drawCross(QImage* image);
    void drawCircle(QImage* image);
    void drawPoint(QImage* image);
    void targetZoom(float ratio);
    bool inRange(const QPoint& p, int x, int y);
    void noPressMouseMove(int x, int y);
    void pressedMouseMove(int x, int y, int dx, int dy);

private:
    static const int SensitiveRange=10;
    TargetParam* m_target;
    QPoint m_crossChangePoint[3];
    QPoint m_circleChangePoint[4];
    typedef enum{NoChange=-1, Up, Right, Down, Left, Lenght, Width, Rotate, Move} ChangeType;
    ChangeType m_changeType;
    bool m_pressed;
};

#endif // PATTERNEDITVIEW_H
