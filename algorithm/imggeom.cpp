#include "imggeom.h"

ImageAdjust::ImageAdjust(){

}

void ImageAdjust::add(CvPoint2D32f imageXY, CvPoint2D32f axisXY){
    centers.push_back(imageXY);
    camPos.push_back(axisXY);
}

void ImageAdjust::clear(){
    if(centers.size()>0)
        centers.clear();
    if(camPos.size()>0)
        camPos.clear();
}

bool ImageAdjust:: figure_up(double& kxx, double&kxy, double& kyx, double& kyy, bool inverse)
{
    if(centers.size()<3){
        printf("there is no enough adjustment data, at least 3 groups\n");
        clear();
        return false;
    }
    vector<CvPoint2D32f>::iterator cenIt, posIt;
    cenIt=centers.begin();
    posIt=camPos.begin();

    double cx0=cenIt->x;
    double cy0=cenIt->y;
    double px0=posIt->x;
    double py0=posIt->y;

    double sum_xz1=0;
    double sum_xz2=0;
    double sum_yz1=0;
    double sum_yz2=0;
    double sum_xx=0;
    double sum_yy=0;
    double sum_xy=0;
    double x,y,z1,z2;
    for(cenIt++, posIt++;cenIt != centers.end(); cenIt++, posIt++){
        x=cenIt->x-cx0;
        y=cenIt->y-cy0;
        if(inverse){
            z1=-(posIt->x-px0);
            z2=-(posIt->y-py0);
        }
        else{
            z1=posIt->x-px0;
            z2=posIt->y-py0;
        }
        sum_xx+=x*x;
        sum_yy+=y*y;
        sum_xy+=x*y;
        sum_xz1+=x*z1;
        sum_yz1+=y*z1;
        sum_xz2+=x*z2;
        sum_yz2+=y*z2;
    }

    double den=sum_xx*sum_yy-sum_xy*sum_xy;
    kxx=(sum_xz1*sum_yy-sum_xy*sum_yz1)/den;
    kxy=(sum_xx*sum_yz1-sum_xz1*sum_xy)/den;
    kyx=(sum_xz2*sum_yy-sum_xy*sum_yz2)/den;
    kyy=(sum_xx*sum_yz2-sum_xz2*sum_xy)/den;
    clear();
    return true;
}

//double in_2pi(double angle)
//{
//    int n=floor(angle/2.0/CV_PI);
//    return angle-n*2.0*CV_PI;
//}

double arctan2(double x, double y)
{
        if(x<0.0000001 && x>-0.0000001)
        {
            if(y>0)
                return 0.5*CV_PI;
            else
                return 1.5*CV_PI;
        }
        else if(x>=0.0000001)
            return atan(y/x);
        else
            return atan(y/x)+CV_PI;
}

//TransformMatrix::TransformMatrix(double kxx, double kxy, double kyx, double kyy)
//    :m_kxx(kxx),m_kxy(kxy),m_kyx(kyx),m_kyy(kyy)
//{
//    double det=m_kxx*m_kyy-m_kxy*m_kyx;
//    assert(det>0.0000000001 || det<-0.0000000001);
//    in_kxx=m_kyy/det;
//    in_kxy=-m_kxy/det;
//    in_kyx=-m_kyx/det;
//    in_kyy=m_kxx/det;
//    rotateAngle=(arctan2(m_kyy, -m_kxy)+arctan2(m_kxx, m_kyx))/2;
//}

//CvPoint2D32f TransformMatrix::transform(double srcx, double srcy) const{
//    double x, y;
//    transform(srcx, srcy, x, y);
//    return cvPoint2D32f(x, y);
//}

//void TransformMatrix::transform(double srcx, double srcy, double& detx, double& dety) const
//{
//    detx=m_kxx*srcx+m_kxy*srcy;
//    dety=m_kyx*srcx+m_kyy*srcy;
//}
//void  TransformMatrix::inv_transform(double srcx, double srcy, double &detx, double& dety) const
//{
//    detx=in_kxx*srcx+in_kxy*srcy;
//    dety=in_kyx*srcx+in_kyy*srcy;
//}

//double TransformMatrix::k() const{
//    return sqrt((m_kxx*m_kxx+m_kxy*m_kxy+m_kyx*m_kyx+m_kyy*m_kyy)/2);
//}

RelPos::RelPos(const CvPoint2D32f& p1, const CvPoint2D32f& p2, const CvPoint2D32f& pt){
    m_p1=p1;
    m_p2=p2;
    m_pt=pt;
    readyCal();
}

void RelPos::readyCal(){
    m_dx=m_p2.x-m_p1.x;
    m_dy=m_p2.y-m_p1.y;
    m_det=m_dx*m_dx+m_dy*m_dy;
}


void RelPos::setBasePos(int index, const CvPoint2D32f &p){
    if(index==0)
        m_p1=p;
    else
        m_p2=p;
    readyCal();
}

void RelPos::setBasePt(const CvPoint2D32f &p){
    m_pt=p;
}

/*
|k1  -k2  mx| |x|
|k2   k1  my| |y|
|0    0   1 | |1|

k1=k*cosa
k2=k*sina

*/

double RelPos::calTargetPos(const CvPoint2D32f &p1, const CvPoint2D32f &p2, CvPoint2D32f& pt){
    double dx=p2.x-p1.x;
    double dy=p2.y-p1.y;
    double k1=(dx*m_dx+dy*m_dy)/m_det;
    double k2=(m_dx*dy-m_dy*dx)/m_det;
    double mx=p1.x-(k1*m_p1.x-k2*m_p1.y);
    double my=p1.y-(k2*m_p1.x+k1*m_p1.y);
    pt.x=k1*m_pt.x-k2*m_pt.y+mx;
    pt.y=k2*m_pt.x+k1*m_pt.y+my;
    float angle=arctan2(k1, k2);
    while(angle>CV_PI)
        angle-=CV_PI;
    while(angle<-CV_PI)
        angle+=CV_PI;
    return angle;
}

XyaOffset::XyaOffset(){
    m_1valid=false;
    m_std1=cvPoint2D32f(0, 0);
    m_act1=cvPoint2D32f(0, 0);
}

CvPoint3D32f XyaOffset::setPonit(int index, CvPoint2D32f stdVal, CvPoint2D32f actVal){
    if(index==0){
        m_1valid=true;
        m_std1=stdVal;
        m_act1=actVal;
        return cvPoint3D32f(stdVal.x-actVal.x, stdVal.y-actVal.y, 0);
    }
    else{
        if(!m_1valid)
            return cvPoint3D32f(0, 0, 0);
        else{
            float offsetx=m_std1.x-m_act1.x;
            float offsety=m_std1.y-m_act1.y;
            CvPoint2D32f stdLine[2], actLine[2];
            stdLine[0]=m_std1;
            stdLine[1]=stdVal;
            actLine[0]=m_act1;
            actLine[1]=actVal;
            float angle=two_lines_angle(actLine, stdLine);
            return cvPoint3D32f(offsetx, offsety, angle/CV_PI*180);
        }
    }
}


static int toInt(double x){
    if(x>=0)
        return int(x+0.5);
    else
        return int(x-0.5);
}

ImagePointer::ImagePointer(unsigned char *p, int stepWidth, double dx, double dy)
    :m_p0(p), m_sw(stepWidth), m_dx(dx), m_dy(dy),m_x(0),m_y(0), m_nx(0), m_ny(0){
    m_p=m_p0;
}

unsigned char* ImagePointer::operator+(int incr)const{
    int colIncr=toInt(m_x+incr*m_dx);
    int rowIncr=toInt(m_y+incr*m_dy);
    return m_p0+rowIncr*m_sw+colIncr;
}

unsigned char* ImagePointer::operator -(int decr)const{
    return operator+(-decr);
}

unsigned char* ImagePointer::operator++(int){
    unsigned char* tmp=m_p;
    m_x+=m_dx;
    m_y+=m_dy;
    float xIncr=m_x-m_nx;
    float yIncr=m_y-m_ny;
    if(xIncr>0.5){
        m_nx++;
        m_p++;
    }
    else if(xIncr<-0.5){
        m_nx--;
        m_p--;
    }
    if(yIncr>0.5){
        m_ny++;
        m_p+=m_sw;
    }
    else if(yIncr<-0.5){
        m_ny--;
        m_p-=m_sw;
    }
    return tmp;
}

unsigned char* ImagePointer::operator --(int){
    unsigned char* tmp=m_p;
    m_x-=m_dx;
    m_y-=m_dy;
    float xIncr=m_x-m_nx;
    float yIncr=m_y-m_ny;
    if(xIncr>0.5){
        m_nx++;
        m_p++;
    }
    else if(xIncr<-0.5){
        m_nx--;
        m_p--;
    }
    if(yIncr>0.5){
        m_ny++;
        m_p+=m_sw;
    }
    else if(yIncr<-0.5){
        m_ny--;
        m_p-=m_sw;
    }
    return tmp;
}

unsigned char& ImagePointer::operator *()const{
    return *m_p;
}

unsigned char& ImagePointer::operator [](int index)const{
    return *(operator +(index));
}



CvPoint2D32f figureRotateCenter(CvPoint2D32f p1, CvPoint2D32f p2, double angle){
    double cx=(p1.x+p2.x)/2;
    double cy=(p1.y+p2.y)/2;
    double dx=p2.x-p1.x;
    double dy=p2.y-p1.y;
    double dist=sqrt(dx*dx+dy*dy);
    double len=dist/2/tan(fabs(angle/2));
    double ex=dx/dist;
    double ey=dy/dist;
    double vx=-ey*len;
    double vy=ex*len;
    return cvPoint2D32f(cx+vx, cy+vy);
}

CvPoint2D32f two_lines_intersection(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2])
{

    //assert two lines are not parallel
    double angle=fabs(two_lines_angle(l1, l2));
    assert(angle/CV_PI*180>0.1);

    //transform to ax+by=c
    double a1=l1[1].y-l1[0].y;
    double b1=l1[0].x-l1[1].x;
    double c1=l1[0].x*(l1[1].y-l1[0].y)+l1[0].y*(l1[0].x-l1[1].x);

    double a2=l2[1].y-l2[0].y;
    double b2=l2[0].x-l2[1].x;
    double c2=l2[0].x*(l2[1].y-l2[0].y)+ l2[0].y*(l2[0].x-l2[1].x);

    //figure out intersection point by determinant
    double detAB=a1*b2-a2*b1;
    double detAC=a1*c2-a2*c1;
    double detCB=c1*b2-c2*b1;
    double x=detCB/detAB;
    double y=detAC/detAB;
    return cvPoint2D32f(x, y);
}

void  mid_line(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2], CvPoint2D32f ml[2]){
    double mx1, my1, mx2, my2, k;

    CvPoint2D32f p1p1[2]={l1[0], l2[0]};
    CvPoint2D32f p2p2[2]={l1[1], l2[1]};

    double ang1=two_lines_angle(l1, p1p1);
    double ang2=two_lines_angle(l2, p1p1);


    if(fabs(ang1)>1.0E-6){
        k=sin(ang2)/sin(ang1);
        mx1=(l1[0].x+k*l2[0].x)/(1+k);
        my1=(l1[0].y+k*l2[0].y)/(1+k);
    }
    else{
        mx1=l2[0].x;
        my1=l2[0].y;
    }
    ang1=two_lines_angle(l1, p2p2);
    ang2=two_lines_angle(l2, p2p2);
    if(fabs(ang1)>1.0E-6){
        k=sin(ang2)/sin(ang1);
        mx2=(l1[1].x+k*l2[1].x)/(1+k);
        my2=(l1[1].y+k*l2[1].y)/(1+k);
    }
    else{
        mx2=l2[1].x;
        my2=l2[1].y;
    }
    ml[0].x=mx1;
    ml[0].y=my1;
    ml[1].x=mx2;
    ml[1].y=my2;
}


/*the least square method for straight line*/
bool line_LSM(vector<ValidImgPoint>& points, bool& yfx, double& k, double& b, double maxDistance, long minPN)
{
    static int recurTimes=0;
    double sum_x=0;
    double sum_y=0;
    double sum_xy=0;
    double sum_xx=0;
    double sum_yy=0;
    long sum_valid=0;
    long reLsm=0;


    recurTimes++;
    if(recurTimes>10){
//        printf("Function lineLsm recur too many times\n");
        recurTimes=0;
        return false;
    }
    double x=0,y=0,err=0;
    vector<ValidImgPoint>::iterator it;
    for(it=points.begin(); it!=points.end(); it++){
        if(it->valid){
            sum_valid++;
            x=it->x;
            y=it->y;
            sum_x+=x;
            sum_y+=y;
            sum_xy+=x*y;
            sum_xx+=x*x;
            sum_yy+=y*y;
        }
    }
    if(sum_valid<minPN){
//        printf("\nIn function lineLsm(), only have %ld valid data, too few\n",sum_valid);
//        printf("k=%lf, b=%lf \n", k, b);
        recurTimes=0;
        return false;
    }
    maxDistance=fabs(maxDistance);
    if(yfx){
        k=(sum_valid*sum_xy-sum_x*sum_y)/(sum_valid*sum_xx-sum_x*sum_x);
        b=(sum_y-k*sum_x)/sum_valid;

        //if not fit to use this type of lsm
        if(k>1.5 || k<-1.5){
            yfx=false;
            return line_LSM(points, yfx,k,b,maxDistance,minPN);
        }

        //check the error  to remove uncorrelated points
        for(it=points.begin();it!=points.end();it++)
        {
            if(it->valid)
            {
                err=it->y-(k*it->x+b);
                if(err>maxDistance|| err<-maxDistance)
                {
                    it->valid=false;
                    reLsm=1;
                }
            }
        }
    }

    else if(!yfx)
    {
        k=(sum_valid*sum_xy-sum_x*sum_y)/(sum_valid*sum_yy-sum_y*sum_y);
        b=(sum_x-k*sum_y)/sum_valid;

        //if not fit to use this type of lsm
        if(k>1.5 || k<-1.5)
        {
            yfx=true;
            //typeChange=1;
            return line_LSM(points,yfx,k,b,maxDistance,minPN);
        }

        //check error to remove uncorrelated points
        for(it=points.begin();it!=points.end();it++)
        {
            if(it->valid)
            {
                err=it->x-(k*it->y+b);
                if(err>maxDistance|| err<-maxDistance)
                {
                    it->valid=false;
                    reLsm=1;
                }
            }
        }
    }

    //after remove uncorrelated points , lsm again
    if(reLsm){
        return line_LSM(points,yfx,k,b,maxDistance,minPN);
    }
    else{// remain points are all correlated
        recurTimes=0;
        return true;
    }
}


/*the least square method for circle*/
static bool circle_direct_LSM(vector<ValidImgPoint>& points, double& x0, double& y0, double& R, double rmDistance , long minPN)
{
    static int recurTimes=0;
    double sum_z=0;
    double sum_x=0;
    double sum_y=0;
    double sum_xy=0;
    double sum_xx=0;
    double sum_yy=0;
    double sum_xz=0;
    double sum_yz=0;
    int sum_valid=0;
    double x,y,z,xx,yy;

    recurTimes++;
    if(recurTimes>10)
    {
        printf("Function circle_LSM recur too many times\n");
        recurTimes=0;
        return false;
    }

    vector<ValidImgPoint>::iterator it;
    for(it=points.begin(); it!=points.end(); it++){
        if(it->valid){
            sum_valid++;
            x=it->x;
            y=it->y;
            sum_x+=x;
            sum_y+=y;
            sum_xy+=x*y;
            xx=x*x;
            sum_xx+=xx;
            yy=y*y;
            sum_yy+=yy;
            z=xx+yy;
            sum_z+=z;
            sum_xz+=x*z;
            sum_yz+=y*z;
        }
    }
    if(sum_valid<minPN){
        printf("In function lineLsm(), only have %d valid data, too few\n",sum_valid);
        recurTimes=0;
        return false;
    }
    //printf("lsm %d\n", sum_valid);
    double Denominator=sum_valid*sum_xx*sum_yy+2.0*sum_x*sum_xy*sum_y-sum_x*sum_x*sum_yy-sum_valid*sum_xy*sum_xy-sum_xx*sum_y*sum_y;
    double x0_numerator=0.5*(sum_valid*sum_xz*sum_yy+sum_xy*sum_y*sum_z+sum_x*sum_y*sum_yz-sum_x*sum_yy*sum_z-sum_valid*sum_xy*sum_yz-sum_xz*sum_y*sum_y);
    double y0_numerator=0.5*(sum_valid*sum_xx*sum_yz+sum_x*sum_xz*sum_y+sum_x*sum_xy*sum_z-sum_x*sum_x*sum_yz-sum_xx*sum_y*sum_z-sum_valid*sum_xy*sum_xz);
    double r_numerator=sum_xx*sum_yy*sum_z+sum_x*sum_xy*sum_yz+sum_xy*sum_xz*sum_y-sum_x*sum_xz*sum_yy-sum_xx*sum_y*sum_yz-sum_xy*sum_xy*sum_z;

    x0=x0_numerator/Denominator;
    y0=y0_numerator/Denominator;
    R=sqrt(x0*x0+y0*y0+r_numerator/Denominator);
    double maxRR=(R+fabs(rmDistance))*(R+fabs(rmDistance));
    double tmp=R-fabs(rmDistance);
    if(tmp<0)
        tmp=0;
    double minRR=tmp*tmp;
    double RR;
    bool reLsm=false;
    for(it=points.begin(); it!=points.end(); it++){
        if(it->valid){
            RR=(it->y-y0)*(it->y-y0)+(it->x-x0)*(it->x-x0);
            if(RR>maxRR || RR<minRR){
                it->valid=false;
                reLsm=true;
            }
        }
    }
    if(reLsm){
        return circle_direct_LSM(points, x0, y0, R, rmDistance, minPN);
    }
    else{
        recurTimes=0;
        return true;
    }
}

 bool circle_LSM(vector<ValidImgPoint>& points, double& x0, double& y0, double& R, double rmDistance , long minPN)
{
    vector<ValidImgPoint>::iterator it;
    double sumx=0,  sumy=0, avgx, avgy;
    long num=0;
    bool res;

    //figure the average of x and y
    for(it=points.begin(); it!=points.end();it++){
        if(it->valid){
            num++;
            sumx+=it->x;
            sumy+=it->y;
        }
    }
    if(num==0)
        return false;
    avgx=sumx/num;
    avgy=sumy/num;

    //sub the average of x and y to avaid overflowing in LSM
    for(it=points.begin(); it!=points.end();it++){
        if(it->valid){
            it->x-=avgx;
            it->y-=avgy;
        }
    }
    //implement the LSM
    res=circle_direct_LSM( points,  x0,  y0,  R, rmDistance, minPN);

    //recover the original points
    for(it=points.begin(); it!=points.end();it++){
        if(it->valid){
            it->x+=avgx;
            it->y+=avgy;
        }
    }
    x0+=avgx;
    y0+=avgy;
    return res;
}

 double two_lines_angle(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2])
{
    double vct1x=l1[1].x-l1[0].x;
    double vct1y=l1[1].y-l1[0].y;
    double vct2x=l2[1].x-l2[0].x;
    double vct2y=l2[1].y-l2[0].y;
    double len1=sqrt(vct1x*vct1x+vct1y*vct1y);
    double len2=sqrt(vct2x*vct2x+vct2y*vct2y);
    if(len1<0.1 || len2<0.1)
        return 0;
    double rad=acos(fabs(vct1x*vct2x+vct1y*vct2y)/(len1*len2));
    if(vct1x*vct2y-vct1y*vct2x <0)
        rad*=-1;

    return rad;
}

// void  lsm_order2(const vector<CvPoint2D32f> & xyArry, double& a, double& b, double& c)
// {
//     long n=xyArry.size();
//     assert(n>=3);
//     vector<CvPoint2D32f>::const_iterator it;
//     double  x, y, xx, xy;
//     double sx=0, sx2=0, sx3=0, sx4=0, sy=0, sxy=0, sx2y=0;
//     double det, numa, numb, numc;
//     for(it=xyArry.begin(); it!=xyArry.end(); it++){
//         xx=x=it->x; xy=y=it->y;
//         sx+=x; sy+=y;
//         xx*=x; xy*=x;
//         sx2+=xx; sxy+=xy;
//         xx*=x; xy*=x;
//         sx3+=xx;  sx2y+=xy;
//         xx*=x;
//         sx4+=xx;
//     }

//     det=sx4*sx2*n + sx3*sx2*sx+sx3*sx2*sx-sx2*sx2*sx2-sx3*sx3*n-sx4*sx*sx;
//     numa=sx2y*sx2*n + sx3*sx*sy + sxy*sx*sx2 - sy*sx2*sx2 - sxy*sx3*n - sx2y*sx*sx;
//     numb=sx4*sxy*n + sx2y*sx*sx2+ sx3*sy*sx2 -sx2*sxy*sx2 - sx2y*sx3*n - sx*sy*sx4;
//     numc= sx4*sx2*sy+ sx3*sxy*sx2+ sx3*sx*sx2y - sx2y*sx2*sx2- sxy*sx4*sx- sy*sx3*sx3;

//     a=numa/det;
//     b=numb/det;
//     c=numc/det;
// }



