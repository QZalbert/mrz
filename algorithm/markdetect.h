#ifndef MARKDETECT_H
#define MARKDETECT_H
#include "nccMatch.h"

class TargetParam;

class MarkDetect : public NccMatch
{
public:
    MarkDetect();
    ~MarkDetect();
    void setTarget(const TargetParam& target);
    void setSimilar(float val);
    void detect(const IplImage* image, bool &patternOk, bool& targetOK, CvPoint2D32f& pos);
    void getRect(CvRect& rect);
    void getCircle(CvPoint2D32f& center, double& r);
    void getCrossLine(CvPoint2D32f edgeLine[4][2], CvPoint2D32f midLine[2][2]);

private:
    bool detectCross(CvPoint2D32f lt);
    bool detectCircle(CvPoint2D32f lt);
    IplImage* m_smooth;
    TargetParam* m_target;
    CvPoint2D32f m_box[4];
    CvPoint2D32f m_pos;
    float m_similar;
    double m_r;
    CvPoint2D32f m_edgeLine[4][2];
    CvPoint2D32f m_midLine[2][2];
};

#endif // MARKDETECT_H
