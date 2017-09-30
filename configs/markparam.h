#ifndef MARKPARAM_H
#define MARKPARAM_H
#include "param.h"
#include "global.h"
#include "cv.h"

typedef struct{
    bool valid;
    double markX;
    double markY;
    int camContrast;
    int camBrightness;
    int camExposure;
    double detectAxis[AXIS_NUM];
}MarkParam;


class PatternParam{
public:
    float similar;
    IplImage* image;
};

class TargetParam
{
public:
    TargetParam();
    enum{Cross=0, Circle, Point};
    int type;
    float length;
    float width;
    float x;
    float y;
    float angle;
    bool white;
    int gradient;
    int gradStep;
    int range;
};

class AppParam : public Param
{
public:
    AppParam();
    void savePatternImage();

public:
    //common param
    char imageFile[256];
    double camRelx;
    double camRely;
    double kxx;
    double kxy;
    double kyy;
    double kyx;

    int arrayRowNum;
    int arrayColNum;
    double arrayRowDist;
    double arrayColDist;
    bool autoExposure;

    //param from project
    PatternParam pattern;
    TargetParam target;

    int markNum;
    MarkParam markParam[MAX_MARK_NUM];
    double xoffset;
    double yoffset;
    double rotateDis;
    double rotateLen;


private:
    void rwPatternImage(bool r);
    virtual void paramInit();
    virtual void rwMachineParam(bool r);
    virtual void rwProjectParam(bool r);
};

#endif // MARKPARAM_H
