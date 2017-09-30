#ifndef MARKCNC_H
#define MARKCNC_H
#include "global.h"
#include <hal.h>
#include <rtapi.h>

typedef struct
{
    hal_s32_t *cvCmd;
    hal_bit_t* coverOpen;
    hal_float_t* xc;
    hal_float_t* yc;
    hal_float_t* angle;
    hal_s32_t * state;
}MarkHalPins;

class MarkCnc
{
public:
    MarkCnc(bool withCnc);
    void update();
    void getCmdAxis(double axis[AXIS_NUM]);
    void gotoPos(const double axis[AXIS_NUM]);
    bool isStop();

private:
    void halInit();
    static const float StopRange=0.005;
    double m_cmdAxis[AXIS_NUM];
    double m_actAxis[AXIS_NUM], m_lastActAxis[AXIS_NUM];
    bool m_stop;
    bool m_goto;
    int m_halId;
    bool m_withCnc;
    MarkHalPins* m_halpins;
};







#endif // MARKCNC_H
