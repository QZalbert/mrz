#ifndef MARKWIDGET_H
#define MARKWIDGET_H

#include <QWidget>
#include <cv.h>
#include "global.h"

class Camera;
class MarkView;
class FocusView;
class AppParam;
class ViewQt;
class PatternEdit;
class MarkDetect;
class MarkCnc;
class PswDialog;
#include "ui_markwidget.h"

namespace Ui {
class MarkWidget;
}

class MarkData
{
public:
    MarkData();
    CvPoint2D32f imagePos;
    CvPoint2D32f actualPos;
    CvPoint2D32f ncPos;
    bool ok;
};

class AdjustData{
   public:
    CvPoint2D32f imagePos;
    CvPoint2D32f axisPos;
    bool ok;
};

class MarkWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MarkWidget(bool withCnc, QWidget *parent = 0);
    int receiveInfo(const char* info);
    QWidget* getViceView();
    ~MarkWidget();

signals:
    void sendInfo(const char* info);

private slots:
    void updateCycle();

    //detect page
    void on_bt_selectPattern_clicked();
    void on_bt_patternOk_clicked();
    void on_bt_patternCancel_clicked();
    void on_bt_editPattern_clicked();
    void detect_clicked();

    //image page
    void on_bt_4x_toggled(bool val);
    void on_bt_8x_toggled(bool val);
    void cam_param_changed(int val);
    void saveImage();
    void loadImage();

    //position page
    void on_bt_showAdjustInf_clicked();

    //adjust page
    void adjustPosClicked();
    void on_bt_adjustClear_clicked();
    void on_bt_adjust_clicked();
    void on_bt_inputHole_clicked();
    void on_bt_detectHole_clicked();
    void on_bt_setOffset_clicked();
    void on_bt_adjustExit_clicked();

private:
    void startCapture();
    void initData(bool withCnc);
    void initUi();
    void loadPorject(const char *prjName=NULL);
    void viewUpdate();
    void cncUpdate();
    void ncMarkUpdate();
    void detect(int index);
    void manualDetect(int index);
    bool loadMarkPos(const char *ncFile, bool tip);


private:
    void setSelectBottonValid(bool sel);

private:
    Camera* m_cam;
    IplImage* m_srcImage;
    AppParam* m_param;
    MarkCnc* m_cnc;
    double m_axis[AXIS_NUM];
    MarkDetect* m_detector;
    MarkData m_markData[MAX_MARK_NUM];
    int m_focusTimes;
    AdjustData m_adjData[ADJUST_POS_NUM];
    int m_markNum;

private:
    MarkView* m_mainView;
    QWidget* m_viceViewWidget;
    MarkView* m_viceView;
    FocusView* m_focusView;
    ViewQt* m_patternView;
    PatternEdit* m_patternEdit;
    PswDialog* m_pswDialog;
    Ui::MarkWidget *ui;
    static const int AdjustPage=3;
    QPushButton* bt_detect[MAX_MARK_NUM];
    QPushButton* bt_goto[MAX_MARK_NUM];
    QCheckBox* cb_point[MAX_MARK_NUM];
    QLabel* lb_mx[MAX_MARK_NUM];
    QLabel* lb_my[MAX_MARK_NUM];
    QPushButton* bt_adjustPos[ADJUST_POS_NUM];
    QLabel* lb_adjPosX[ADJUST_POS_NUM];
    QLabel* lb_adjPosY[ADJUST_POS_NUM];
    QWidget* m_adjTable;
};

#endif // MARKWIDGET_H
