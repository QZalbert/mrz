//相机校正，图像变换等等。

#ifndef IMGGEOM_H
#define IMGGEOM_H
#include <cv.h>
#include <vector>

using std::vector;

class ImageAdjust
{
public:
    ImageAdjust();
    void add(CvPoint2D32f imageXY, CvPoint2D32f axisXY);
    void clear();
    bool figure_up(double& kxx, double&kxy, double& kyx, double& kyy, bool cameraMove=true);
    int size() const{return centers.size();}
private:
    vector<CvPoint2D32f> centers;
    vector<CvPoint2D32f> camPos;
};


class TransformMatrix
{
public:
    TransformMatrix(double kxx=1, double kxy=0, double kyx=0, double kyy=1);
    void transform(double srcx, double srcy, double &detx, double& dety) const;
    CvPoint2D32f transform(double srcx, double srcy) const;
    void inv_transform(double srcx, double srcy, double &detx, double& dety) const;
    double kx() const{return m_kxx;}
    double ky() const{return m_kyy;}
    double kxx()const{return m_kxx;}
    double kxy()const{return m_kxy;}
    double kyx()const{return m_kyx;}
    double kyy()const{return m_kyy;}
    double k() const;
    double rotate_angle()const {return rotateAngle;}
protected:
    double m_kxx;
    double m_kxy;
    double m_kyx;
    double m_kyy;
    double in_kxx;
    double in_kxy;
    double in_kyx;
    double in_kyy;
    double rotateAngle;
};

class RelPos
{
public:
    RelPos(const CvPoint2D32f& p1, const CvPoint2D32f& p2, const CvPoint2D32f& pt);
    void setBasePos(int index, const CvPoint2D32f& p);
    void setBasePt(const CvPoint2D32f& p);
    double calTargetPos(const CvPoint2D32f& p1, const CvPoint2D32f& p2, CvPoint2D32f& pt);
private:
    void readyCal();
    CvPoint2D32f  m_p1, m_p2, m_pt;
    double m_dx, m_dy, m_det;
};

class XyaOffset
{
public:
    XyaOffset();
    CvPoint3D32f setPonit(int index, CvPoint2D32f stdVal, CvPoint2D32f actVal);
private:
    bool m_1valid;
    CvPoint2D32f m_std1, m_act1;
};


class ImagePointer{
public:
    ImagePointer(unsigned char *p, int stepWidth, double dx, double dy);
    unsigned char* operator++(int);
    unsigned char* operator--(int);
    unsigned char* operator+(int incr)const;
    unsigned char* operator-(int decr)const;
    unsigned char& operator*()const;
    unsigned char& operator [](int index)const;
    int rowIncr()const{return m_ny;}
    int colIncr()const{return m_nx;}
private:
    unsigned char* m_p0, *m_p;
    int m_sw;
    float m_dx, m_dy;
    float m_x, m_y;
    int m_nx,m_ny;
};


CvPoint2D32f figureRotateCenter(CvPoint2D32f p1, CvPoint2D32f p2, double angle);

CvPoint2D32f two_lines_intersection(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2]);

void  mid_line(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2], CvPoint2D32f ml[2]);

typedef struct ValidImgPoint{
    int x;
    int y;
    bool valid;
}ValidImgPoint;

bool line_LSM(vector<ValidImgPoint>& points, bool& yfx, double& k, double& b, double maxDistance, long minPN);

bool circle_LSM(vector<ValidImgPoint>& points, double& x0, double& y0, double& R, double rmDistance , long minPN);

double two_lines_angle(const CvPoint2D32f l1[2], const CvPoint2D32f l2[2]);

void  lsm_order2(const vector<CvPoint2D32f> & xyArry, double& a, double& b, double& c);

#endif // IMGGEOM_H
