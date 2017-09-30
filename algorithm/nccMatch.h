//模板匹配算法

#pragma once

#include <vector>
#include <cv.h>

using std::vector;

class NccMatch
{
public:
	NccMatch();
    typedef struct{
        CvPoint2D32f box[4];
    }Box;
	void setPattern(const IplImage* temp);
    bool findPattern(const IplImage* img,  CvRect* roi, float minSore, CvPoint2D32f box[4],
		float& angle, float& score, int *cosMs=NULL);

    bool findPattern(const IplImage* img,  CvRect* roi, float minSore, int maxNum,
                     vector<Box>& boxes, vector<float>& angles, vector<float>& scores,
                     int *cosMs=NULL);
private:
    bool preProcess(const IplImage* img, CvRect* roi);
	float topMatch(CvPoint& pos);
    void topMatch(vector<CvPoint>& pos, vector<float>& score, int maxNum, float minScore);
	float matchUp(CvPoint& pos);
	static const int MaxLayer=10;
	int  m_minLen;
	int m_setPatten;
	int m_layerNum;
	IplImage* m_patternImg[MaxLayer];
	IplImage* m_srcImg[MaxLayer];


};
