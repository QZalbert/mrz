#include "markparam.h"
#define MACHINE_INI "/home/u/cnc/configs/ppmc/ppmc.ini"
#define PRJ_DIR "/home/u/cnc/CCD定位存档"
#include <highgui.h>

AppParam::AppParam():Param(MACHINE_INI, PRJ_DIR)
{
    paramInit();
    loadMachineIni();
}

void AppParam::savePatternImage()
{
    rwPatternImage(false);
}

void AppParam::paramInit()
{
    imageFile[0]=0;
    camRelx=0;
    camRely=0;
    kxx=0.05;
    kxy=0;
    kyy=0.05;
    kyx=0;

    arrayRowNum=1;
    arrayColNum=1;
    arrayRowDist=100;
    arrayColDist=100;
    autoExposure=true;

    //param from project
    pattern.similar=0.6;
    pattern.image=NULL;


    markNum=2;
    for(int i=0;i<MAX_MARK_NUM;i++){
        MarkParam* p=&markParam[i];
        p->markX=0;
        p->markY=0;
        p->valid=false;
        p->camBrightness=50;
        p->camContrast=50;
        p->camExposure=20;
        for(int j=0;j<AXIS_NUM;j++)
            p->detectAxis[j]=0;
    }
    xoffset=0;
    yoffset=0;
    rotateDis=0;
    rotateLen=100;
}

void AppParam::rwMachineParam(bool r)
{
    RWParam(r, imageFile);
    RWParam(r, camRelx);
    RWParam(r, camRely);
    RWParam(r, kxx);
    RWParam(r, kxy);
    RWParam(r, kyx);
    RWParam(r, kyy);
    RWParam(r, arrayRowNum);
    RWParam(r, arrayColNum);
    RWParam(r, arrayRowDist);
    RWParam(r, arrayColDist);
    RWParam(r, autoExposure);
}

void AppParam::rwProjectParam(bool r)
{
    RWParam(r, pattern.similar);
    rwPatternImage(r);
    rwParam(r, target.type, "targetType");
    rwParam(r, target.angle, "targetAngle");
    rwParam(r, target.x, "targetX");
    rwParam(r, target.y, "targetY");
    rwParam(r, target.width, "targetW");
    rwParam(r, target.length, "targetL");
    rwParam(r, target.white, "targetWhite");
    rwParam(r, target.gradient, "targetGradient");
    rwParam(r, target.gradStep, "targetGradStep");
    rwParam(r, target.range, "targetRange");

    RWParam(r, markNum);
    for(int i=0;i<MAX_MARK_NUM;i++){
        MarkParam* p=&markParam[i];
        RWParam1(r, p->markX, i);
        RWParam1(r, p->markY, i);
        RWParam1(r, p->valid, i);
        RWParam1(r, p->camBrightness, i);
        RWParam1(r, p->camContrast, i);
        RWParam1(r, p->camExposure, i);
        for(int j=0;j<AXIS_NUM;j++)
            RWParam2(r, p->detectAxis[j], i, j);
    }
    RWParam(r, xoffset);
    RWParam(r, yoffset);
    RWParam(r, rotateDis);
    RWParam(r, rotateLen);
}

TargetParam::TargetParam()
{
    type=Point;
    angle=0;
    length=50;
    width=10;
    x=25;
    y=25;
    white=false;
    gradient=15;
    gradStep=3;
    range=20;
}

void AppParam::rwPatternImage(bool r)
{
    char patternFile[256];
    sprintf(patternFile, "%s/pattern.bmp", m_prjDir);
    if(r){
        pattern.image=cvLoadImage(patternFile, 0);
        if(!pattern.image){
            pattern.image= cvCreateImage(cvSize(50, 50), 8, 1);
            cvZero(pattern.image);
        }
    }
    else{
        if(pattern.image)
            cvSaveImage(patternFile, pattern.image);
    }

}
