#include "markdetect.h"
#include "markparam.h"
#include "imggeom.h"

typedef enum {DIFF=0, WHITE, BLACK}BlackWhite;

static bool figureHorLine(const IplImage* image, int gradient, int gradStep, int edgRange,
                   CvPoint2D32f& p1, CvPoint2D32f& p2, BlackWhite type)
{
    if(p1.x>p2.x){
        CvPoint2D32f tmp=p1;
        p1=p2;
        p2=tmp;
    }
    float dRow=(p2.y-p1.y)/(p2.x-p1.x);
    int colBegin=p1.x+0.5;
    int colEnd=p2.x+0.5;

    if(colBegin<0)
        colBegin=0;
    if(colEnd>=image->width)
        colEnd=image->width-1;

    float fRow=(colBegin-p1.x)*dRow+p1.y;
    int maxRow;
    unsigned char* pPiexl=NULL;
    int ws=image->widthStep;
    int stepGrad, maxStepGrad;
  //  int width=image->width;
    int height=image->height;
    int gradThresh=gradStep*gradient;
    ValidImgPoint edgePoint;
    edgePoint.valid=true;

    vector<ValidImgPoint> points;
    for(int col=colBegin; col<=colEnd; col++, fRow+=dRow){
        int rowBegin=fRow-edgRange+0.5;
        int rowEnd=fRow+edgRange+0.5;
        if(rowBegin-gradStep<0)
            rowBegin=gradStep;
        if(rowEnd+gradStep>=height)
            rowEnd=height-gradStep-1;
        if(rowEnd<rowBegin)
            continue;

        int row=rowBegin;
        pPiexl=(unsigned char*)image->imageData+row*ws+col;
        stepGrad=0;
        for(int n=1;n<=gradStep;n++){
            stepGrad+=pPiexl[n*ws]-pPiexl[-n*ws];
        }
        maxStepGrad=stepGrad;
        maxRow=row;

        for(; row<=rowEnd; row++){
            if((type==WHITE && stepGrad>maxStepGrad) || (type==BLACK && stepGrad<maxStepGrad)
                || (type==DIFF && abs(stepGrad)>abs(maxStepGrad))){
                maxStepGrad=stepGrad;
                maxRow=row;
            }
            stepGrad+=(pPiexl[(gradStep+1)*ws]-pPiexl[ws])-(pPiexl[0]-pPiexl[-gradStep*ws]);
            pPiexl+=ws;
        }

        if((type==WHITE && maxStepGrad>gradThresh)||(type==BLACK && maxStepGrad<-gradThresh)
            ||(type==DIFF && abs(maxStepGrad)>gradThresh)){
                edgePoint.x=col;
                edgePoint.y=maxRow;
                points.push_back(edgePoint);
        }
    }
    //LSM
    bool yfx=true;
    double k=0, b=0;
    bool res;
    res=line_LSM(points, yfx, k, b, 3, (p2.x-p1.x)/3);
    if(res){
        p1.y=k*p1.x+b;
        p2.y=k*p2.x+b;
    }
    return res;
}

static bool figureVelLine(const IplImage* image, int gradient, int gradStep, int edgRange,
                   CvPoint2D32f& p1, CvPoint2D32f& p2, BlackWhite type)
{

    if(p1.y>p2.y){
        CvPoint2D32f tmp=p1;
        p1=p2;
        p2=tmp;
    }
    double dCol=(p2.x-p1.x)/(p2.y-p1.y);

    int rowBegin=p1.y;
    int rowEnd=p2.y;
    if(rowBegin<0)
        rowBegin=0;
    if(rowEnd>=image->height)
        rowEnd=image->height-1;
    float fCol=(rowBegin-p1.y)*dCol+p1.x;

    int maxCol;
    unsigned char* pPiexl=NULL;
    int ws=image->widthStep;
    int stepGrad, maxStepGrad;
    int width=image->width;
   // int height=image->height;
    int gradThresh=gradStep*gradient;
    ValidImgPoint edgePoint;
    edgePoint.valid=true;

    vector<ValidImgPoint> points;
    for(int row=rowBegin; row<=rowEnd; row++, fCol+=dCol){
        int colBegin=fCol-edgRange+0.5;
        int colEnd=fCol+edgRange+0.5;
        if(colBegin-gradStep<0)
            colBegin=gradStep;
        if(colEnd+gradStep>=width)
            colEnd=width-gradStep-1;
        if(colEnd<colBegin)
            continue;

        int col=colBegin;
        pPiexl=(unsigned char*)image->imageData+row*ws+col;
        stepGrad=0;
        for(int n=1;n<=gradStep;n++){
            stepGrad+=pPiexl[n]-pPiexl[-n];
        }
        maxStepGrad=stepGrad;
        maxCol=col;

        for(; col<=colEnd; col++){
            if((type==WHITE && stepGrad>maxStepGrad) || (type==BLACK && stepGrad<maxStepGrad)
                ||(type==DIFF && abs(stepGrad)>abs(maxStepGrad))){
                maxStepGrad=stepGrad;
                maxCol=col;
            }
            stepGrad+=(pPiexl[gradStep+1]-pPiexl[1])-(pPiexl[0]-pPiexl[-gradStep]);
            pPiexl++;
        }
        if((type==WHITE && maxStepGrad>gradThresh)||(type==BLACK && maxStepGrad<-gradThresh)
                ||(type==DIFF && abs(maxStepGrad)>gradThresh)){
            edgePoint.x=maxCol;
            edgePoint.y=row;
            points.push_back(edgePoint);
        }
    }


    //LSM
    bool yfx=true;
    double k=0, b=0;
    bool res;
    yfx=false;
    res=line_LSM(points, yfx, k, b, 3, (p2.y-p1.y)/3);
    if(res){
        p1.x=k*p1.y+b;
        p2.x=k*p2.y+b;
    }

    return res;

}

static bool figureCircle(const IplImage* img, int gradient, int gradStep, int edgRange,
                         CvPoint2D32f& center, double& circleR, BlackWhite type){
    int numScanPoints =2*CV_PI*circleR;
    if(numScanPoints>100)
        numScanPoints=100;
    float angleIncr=2*CV_PI/numScanPoints;
    float scanAngle=0;
    float inR=circleR-edgRange;
    if(inR<0)
        inR=0;
    int scanW=2*edgRange;
    int widthStep=img->widthStep;
    unsigned char* imageHead=(unsigned char*)img->imageData;
    int headSum, tailSum, maxDiff, diff;
    vector<ValidImgPoint> edgePs;
    ValidImgPoint apoint;
    apoint.valid=true;
    for(int i=0;i<numScanPoints; i++,scanAngle+=angleIncr){
        int startx=center.x+inR*cos(scanAngle)+0.5;
        int starty=center.y+inR*sin(scanAngle)+0.5;
        unsigned char* p= imageHead+starty*widthStep+startx;
        ImagePointer imgp(p, widthStep, cos(scanAngle), sin(scanAngle));
        headSum=0;tailSum=0;
        for(int k=0;k<=gradStep;k++){
            headSum+=imgp[k];
            tailSum+=imgp[-k];
        }
        if(type==BLACK)
            maxDiff=headSum-tailSum;
        else if(type==WHITE)
            maxDiff=tailSum-headSum;
        else
            maxDiff=abs(tailSum-headSum);

        int maxCol=0, maxRow=0;
        for(int j=1;j<scanW;j++){
            imgp++;
            headSum+=imgp[gradStep]-imgp[-1];
            tailSum+=*imgp-imgp[-gradStep-1];

            if(type==BLACK)
                diff=headSum-tailSum;
            else if(type==WHITE)
                diff=tailSum-headSum;
            else
                diff=abs(tailSum-headSum);

            if(diff>maxDiff){
                maxDiff=diff;
                maxCol=imgp.colIncr();
                maxRow=imgp.rowIncr();
            }
        }
        if(maxDiff>gradient*gradStep){
            apoint.x=startx+maxCol;
            apoint.y=starty+maxRow;
            edgePs.push_back(apoint);
        }
    }
    double cx, cy, R;
    bool res=circle_LSM(edgePs, cx, cy, R, MIN(scanW, 10), 30);
    if(res){
        center.x=cx;
        center.y=cy;
        circleR=R;
    }
    return res;
}

MarkDetect::MarkDetect()
{
    m_smooth=NULL;
    m_similar=0.6;
    m_target=new TargetParam;
}

MarkDetect::~MarkDetect()
{
    delete m_target;
}

void MarkDetect::setTarget(const TargetParam &target)
{
    *m_target=target;
}

void MarkDetect::setSimilar(float val)
{
    m_similar=val;
}

void MarkDetect::detect(const IplImage *image, bool &patternOk, bool &targetOK, CvPoint2D32f &pos)
{
    if(m_smooth)
        cvReleaseImage(&m_smooth);
    m_smooth=cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    cvSmooth(image, m_smooth);
    float angle, score;
    patternOk=findPattern(m_smooth, NULL, m_similar, m_box, angle, score);
    if(!patternOk)
        return;

    if(m_target->type==TargetParam::Point){
        targetOK=true;
        pos.x=m_box[0].x+m_target->x;
        pos.y=m_box[0].y+m_target->y;
    }
    else if(m_target->type==TargetParam::Circle){
        targetOK=detectCircle(m_box[0]);
        pos=m_pos;
    }
    else if(m_target->type==TargetParam::Cross){
        targetOK=detectCross(m_box[0]);
        pos=m_pos;
    }
}

void MarkDetect::getRect(CvRect &rect)
{
    rect.x=m_box[0].x;
    rect.y=m_box[0].y;
    rect.width=m_box[2].x-m_box[0].x+1;
    rect.height=m_box[2].y-m_box[0].y+1;
}

void MarkDetect::getCircle(CvPoint2D32f &center, double &r)
{
    center=m_pos;
    r=m_r;
}

void MarkDetect::getCrossLine(CvPoint2D32f edgeLine[][2], CvPoint2D32f midLine[][2])
{
    for(int i=0;i<4;i++){
        for(int j=0;j<2;j++){
            edgeLine[i][j]=m_edgeLine[i][j];
        }
    }
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            midLine[i][j]=m_midLine[i][j];
        }
    }
}

bool MarkDetect::detectCross(CvPoint2D32f lt)
{
    float cx=lt.x+m_target->x;
    float cy=lt.y+m_target->y;
    float l=m_target->length/2;
    float w=m_target->width/2;
    float vx[8]={-l, l, -l, l, -w, -w, w, w};
    float vy[8]={-w, -w, w, w, -l, l, -l, l};
    CvPoint2D32f p[8];
    float cosa=cos(m_target->angle);
    float sina=-sin(m_target->angle);
    for(int i=0;i<8;i++){
        p[i].x=cx+vx[i]*cosa-vy[i]*sina;
        p[i].y=cy+vx[i]*sina+vy[i]*cosa;
    }
    int gradient=m_target->gradient;
    int gradStep=m_target->gradStep;
    int range=m_target->range;
    bool white=m_target->white;
    bool ok=true;
    ok=ok&&figureHorLine(m_smooth, gradient, gradStep, range, p[0], p[1], white?WHITE:BLACK);
    ok=ok&&figureHorLine(m_smooth, gradient, gradStep, range, p[2], p[3], !white?WHITE:BLACK);
    ok=ok&&figureVelLine(m_smooth, gradient, gradStep, range, p[4], p[5], white?WHITE:BLACK);
    ok=ok&&figureVelLine(m_smooth, gradient, gradStep, range, p[6], p[7], !white?WHITE:BLACK);

    if(ok){
        for(int i=0;i<4;i++){
            for(int j=0;j<2;j++){
                m_edgeLine[i][j]=p[2*i+j];
            }
        }
        mid_line(m_edgeLine[0], m_edgeLine[1], m_midLine[0]);
        mid_line(m_edgeLine[2], m_edgeLine[3], m_midLine[1]);
        m_pos=two_lines_intersection(m_midLine[0], m_midLine[1]);
    }

    return ok;
}

bool MarkDetect::detectCircle(CvPoint2D32f lt)
{
    m_pos.x=lt.x+m_target->x;
    m_pos.y=lt.y+m_target->y;
    m_r=m_target->length/2;
    return figureCircle(m_smooth, m_target->gradient, m_target->gradStep, m_target->range,
                 m_pos, m_r, m_target->white?WHITE:BLACK);
}









