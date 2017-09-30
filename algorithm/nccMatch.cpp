#include "nccMatch.h"
#include "highgui.h"

// static void get_matrix_sub_pos(const CvMat* mat, const CvPoint& maxPos, CvPoint2D32f& subPos)
// {
// 	vector<CvPoint2D32f> subPixVect;	
// 	if(maxPos.x>0 && maxPos.x<mat->cols-1 && maxPos.y>0 && maxPos.y<mat->rows-1){
// 		double a, b, c;
// 		subPixVect.clear();
// 		subPixVect.push_back(cvPoint2D32f(maxPos.x, cvmGet(mat, maxPos.y,  maxPos.x)));
// 		subPixVect.push_back(cvPoint2D32f(maxPos.x-1, cvmGet(mat, maxPos.y,  maxPos.x-1)));
// 		subPixVect.push_back(cvPoint2D32f(maxPos.x+1, cvmGet(mat, maxPos.y,  maxPos.x+1)));
// 		lsm_order2(subPixVect, a, b, c);
// 		subPos.x=-0.5*b/a;
// 		subPixVect.clear();
// 		subPixVect.push_back(cvPoint2D32f(maxPos.y, cvmGet(mat, maxPos.y,  maxPos.x)));
// 		subPixVect.push_back(cvPoint2D32f(maxPos.y-1, cvmGet(mat, maxPos.y-1,  maxPos.x)));
// 		subPixVect.push_back(cvPoint2D32f(maxPos.y+1, cvmGet(mat, maxPos.y+1,  maxPos.x)));
// 		lsm_order2(subPixVect, a, b, c);
// 		subPos.y=-0.5*b/a;
// 	}
// 	else{
// 		subPos.x=maxPos.x;
// 		subPos.y=maxPos.y;
// 	}
// }


NccMatch::NccMatch()
{
	m_minLen=20;
	m_setPatten=false;
	m_layerNum=0;
	for(int i=0;i<MaxLayer;i++){
		m_patternImg[i]=NULL;
		m_srcImg[i]=NULL;
	}
}

static void pyrDown(const IplImage* src, IplImage* dst){
	assert(src->width/2==dst->width && src->height/2==dst->height);
	unsigned char *dstRow=(unsigned char *)dst->imageData, *pDst;
	const unsigned char *srcRow=(unsigned char *)src->imageData, *pSrc;
	int dstW=dst->width, dstH=dst->height;
	int dstWs=dst->widthStep;
	int srcWs=src->widthStep;
	int srcWs2=2*srcWs;
	int index11=srcWs+1;
	int sum;
	for(int row=0; row<dstH; row++){
		pDst=dstRow;
		pSrc=srcRow;
		for(int col=0;col<dstW; col++){
			sum=*pSrc+pSrc[1]+pSrc[srcWs]+pSrc[index11];
			sum >>=2;
			*pDst=sum;
			pDst++;
			pSrc+=2;
		}
		dstRow+=dstWs;
		srcRow+=srcWs2;
	}
}

void NccMatch::setPattern(const IplImage * temp){
	for(int i=0;i<MaxLayer;i++){
		if(m_patternImg[i])
			cvReleaseImage(&m_patternImg[i]);
	}
	m_patternImg[0]=cvCreateImage(cvGetSize(temp), 8, 1);
	cvSmooth(temp, m_patternImg[0]);
	m_layerNum=1;

	for(int i=1;i<MaxLayer;i++){
		int w=m_patternImg[i-1]->width/2;
		int h=m_patternImg[i-1]->height/2;
		if(w>m_minLen && h>m_minLen){
			m_patternImg[i]=cvCreateImage(cvSize(w, h), 8, 1);
			pyrDown(m_patternImg[i-1], m_patternImg[i]);
			m_layerNum++;
		}
		else{
			break;
		}
	}
	m_setPatten=true;
}



float NccMatch::topMatch(CvPoint& pos){
	
	IplImage* tempImage=m_patternImg[m_layerNum-1];
	IplImage* srcImage=m_srcImg[m_layerNum-1];
	int matchW=srcImage->width-tempImage->width+1;
	int matchH=srcImage->height-tempImage->height+1;
	CvMat* mathRes=cvCreateMat(matchH, matchW, CV_32FC1);
	double score=0;
	cvMatchTemplate(srcImage, tempImage, mathRes, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(mathRes, NULL, &score, NULL, &pos);
	cvReleaseMat(&mathRes);
	return score;
}

void NccMatch::topMatch(vector<CvPoint>& poses, vector<float>& scores,
                        int maxNum, float minScore){
    poses.clear();
    scores.clear();
    IplImage* tempImage=m_patternImg[m_layerNum-1];
    IplImage* srcImage=m_srcImg[m_layerNum-1];
    int matchW=srcImage->width-tempImage->width+1;
    int matchH=srcImage->height-tempImage->height+1;
    CvMat* mathRes=cvCreateMat(matchH, matchW, CV_32FC1);
    double score=0;
    cvMatchTemplate(srcImage, tempImage, mathRes, CV_TM_CCOEFF_NORMED);
    CvPoint pos;

    while(poses.size()<maxNum){
        cvMinMaxLoc(mathRes, NULL, &score, NULL, &pos);
        if(score>minScore){
            poses.push_back(pos);
            scores.push_back(score);
            int left=pos.x-tempImage->width;
            int right=pos.x+tempImage->width;
            int top=pos.y-tempImage->height;
            int bottom=pos.y+tempImage->height;
            if(left<0)
                left=0;
            if(right>mathRes->width)
                right=mathRes->width;
            if(top<0)
                top=0;
            if(bottom>mathRes->height)
                bottom=mathRes->height;
            for(int row=top; row<bottom; row++){
                for(int col=left; col<right; col++){
                    cvmSet(mathRes, row, col, 0);
                }
            }
        }
        else
            break;
    }
    cvReleaseMat(&mathRes);
}



float NccMatch::matchUp(CvPoint& pos){
	CvRect roi;
	double score;
	for(int i=m_layerNum-2;i>=0;i--){
		pos.x*=2;
		pos.y*=2;
		roi.x=pos.x-2;
		roi.y=pos.y-2;
		roi.width=m_patternImg[i]->width+4;
		roi.height=m_patternImg[i]->height+4;
		if(roi.x<0)
			roi.x=0;
		if(roi.y<0)
			roi.y=0;
		if(roi.width>m_srcImg[i]->width)
			roi.width=m_srcImg[i]->width;
		if(roi.height>m_srcImg[i]->height)
			roi.height=m_srcImg[i]->height;
		if(roi.x+roi.width>m_srcImg[i]->width)
			roi.x=m_srcImg[i]->width-roi.width;
		if(roi.y+roi.height>m_srcImg[i]->height)
			roi.y=m_srcImg[i]->height-roi.height;

		IplImage* subImage=cvCreateImage(cvSize(roi.width, roi.height), 8, 1);
		cvSetImageROI(m_srcImg[i], roi);
		cvCopyImage(m_srcImg[i], subImage);
		cvResetImageROI(m_srcImg[i]);

		int matchW=subImage->width-m_patternImg[i]->width+1;
		int matchH=subImage->height-m_patternImg[i]->height+1;
		CvMat* mathRes=cvCreateMat(matchH, matchW, CV_32FC1);

		cvMatchTemplate(subImage, m_patternImg[i], mathRes, CV_TM_CCOEFF_NORMED);
		cvMinMaxLoc(mathRes, NULL, &score, NULL, &pos);
		pos.x+=roi.x;
		pos.y+=roi.y;
		cvReleaseMat(&mathRes);
		cvReleaseImage(&subImage);
	}

	return score;	
}



bool NccMatch::preProcess(const IplImage* img, CvRect* roi){
    if(!m_setPatten){
        printf("must learn template before match\n");
        return false;
    }

    for(int i=0;i<MaxLayer;i++){
        if(m_srcImg[i])
            cvReleaseImage(&m_srcImg[i]);
    }

    if(roi==NULL)
        m_srcImg[0]=cvCloneImage(img);
    else{
        IplImage* bufImg=cvCloneImage(img);
        m_srcImg[0]=cvCreateImage(cvSize(roi->width, roi->height), 8, 1);
        cvSetImageROI(bufImg, *roi);
        cvCopyImage(bufImg, m_srcImg[0]);
        cvResetImageROI(bufImg);
        cvReleaseImage(&bufImg);
    }
    cvSmooth(m_srcImg[0], m_srcImg[0]);

    if(m_srcImg[0]->width<m_patternImg[0]->width || m_srcImg[0]->height<m_patternImg[0]->height){
        printf("pattern size is bigger than image size\n");
        return false;
    }

    for(int i=1;i<m_layerNum;i++){
        m_srcImg[i]=cvCreateImage(cvSize(m_srcImg[i-1]->width/2, m_srcImg[i-1]->height/2), 8, 1);
        pyrDown(m_srcImg[i-1], m_srcImg[i]);
    }
    return true;
}

bool NccMatch::findPattern(const IplImage* img,  CvRect* roi, float minSore, CvPoint2D32f box[4],
	float& angle, float& score, int *cosMs){

    bool res=preProcess(img, roi);
    if(!res)
        return false;

	CvPoint pos;
	score=topMatch(pos);

	score=matchUp(pos);

    if(roi){
        pos.x+=roi->x;
        pos.y+=roi->y;
    }

	box[0].x=box[3].x=pos.x;
	box[0].y=box[1].y=pos.y;
	box[1].x=box[2].x=pos.x+m_patternImg[0]->width;
	box[2].y=box[3].y=pos.y+m_patternImg[0]->height;

	angle=0;

	return score>minSore;
}

bool NccMatch::findPattern(const IplImage* img,  CvRect* roi, float minScore, int maxNum,
                 vector<Box>& boxes, vector<float>& angles, vector<float>& scores,
                 int *cosMs){
    bool res=preProcess(img, roi);
    if(!res)
        return false;

    vector<CvPoint> poses;
    vector<float> tmpScore;
    topMatch(poses, tmpScore, maxNum, minScore);


    boxes.clear();
    angles.clear();
    scores.clear();
    Box abox;
    CvPoint2D32f* box=abox.box;

    for(int i=0;i<poses.size();i++){
        tmpScore[i]=matchUp(poses[i]);

        if(tmpScore[i]>minScore){
            if(roi){
                poses[i].x+=roi->x;
                poses[i].y+=roi->y;
            }
            box[0].x=box[3].x=poses[i].x;
            box[0].y=box[1].y=poses[i].y;
            box[1].x=box[2].x=poses[i].x+m_patternImg[0]->width;
            box[2].y=box[3].y=poses[i].y+m_patternImg[0]->height;
            boxes.push_back(abox);
            angles.push_back(0);
            scores.push_back(tmpScore[i]);
        }
    }
    return boxes.size()>0;
}
