#include "markcnc.h"
#include "cnc.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum CNC_TASK_MODE_ENUM {
    CNC_TASK_MODE_MANUAL = 0,
    CNC_TASK_MODE_AUTO = 1,
    CNC_TASK_MODE_MDI = 2
};

MarkCnc::MarkCnc(bool withCnc)
{
    m_withCnc=withCnc;
    m_goto=false;
    m_stop=true;
    for(int i=0;i<AXIS_NUM;i++){
        m_cmdAxis[i]=m_actAxis[i]=m_lastActAxis[i]=0;
    }
    if(withCnc)
        halInit();
    else
        m_halpins=NULL;
}

void MarkCnc::update()
{
    if(!m_withCnc)
        return;
    double axisDiff[AXIS_NUM];
    m_stop=true;
    for(int i=0;i<AXIS_NUM;i++){
        m_cmdAxis[i]=cnc_abs_cmd_pos(0, i);
        m_actAxis[i]=cnc_abs_act_pos(0, i);
        axisDiff[i]=m_actAxis[i]-m_lastActAxis[i];
        m_stop=m_stop && fabs(axisDiff[i])<StopRange;
        m_lastActAxis[i]=m_actAxis[i];
    }
    if(m_goto && m_stop){
        m_goto=false;
         cnc_mode(0,1, CNC_TASK_MODE_MANUAL);
    }
}

void MarkCnc::getCmdAxis(double axis[])
{
    for(int i=0;i<AXIS_NUM;i++)
        axis[i]=m_cmdAxis[i];
}

void MarkCnc::gotoPos(const double axis[])
{
    char mdiBuf[128];
    cnc_mode(0,1, CNC_TASK_MODE_MDI);
    cnc_mdi(0,"M09");
    cnc_mdi(0,"G69");
    cnc_mdi(0,"G90");
    cnc_mdi(0,"g0 g53 z0");
    sprintf(mdiBuf, "g0 g53 x%f y%f", axis[0], axis[1]);
    cnc_mdi(0,mdiBuf);
    m_goto=true;
}

void MarkCnc::halInit()
{
    int retval;

    /* STEP 1: initialise the hal component */
    m_halId= hal_init("mark");
    if (m_halId< 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "HALUI: ERROR: hal_init() failed\n");;
        return ;
    }

      /* STEP 2: allocate shared memory for hal pins data */
    m_halpins= (MarkHalPins *) hal_malloc(sizeof(MarkHalPins));
    if ( m_halpins== 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "HALUI: ERROR: hal_malloc() failed\n");
        hal_exit(m_halId);
        return;
    }

    /* STEP 3: export  pins*/
    retval = hal_pin_s32_new("mark.cvCmd", HAL_IO, &m_halpins->cvCmd, m_halId);
    if (retval != 0)
        return;
    *m_halpins->cvCmd=0;

    retval = hal_pin_bit_new("mark.cover-open", HAL_OUT, &m_halpins->coverOpen, m_halId);
    if (retval != 0)
        return ;
    *m_halpins->coverOpen=0;

    retval = hal_pin_float_new("mark.center-x", HAL_OUT, &m_halpins->xc, m_halId);
    if (retval != 0)
        return ;

    retval = hal_pin_float_new("mark.center-y", HAL_OUT, &m_halpins->yc, m_halId);
    if (retval != 0)
        return ;

    retval = hal_pin_float_new("mark.angle", HAL_OUT, &m_halpins->angle, m_halId);
    if (retval != 0)
        return ;

    retval = hal_pin_s32_new("mark.state", HAL_OUT, &m_halpins->state, m_halId);
    if (retval != 0)
        return ;
    *m_halpins->state=0;

    hal_ready(m_halId);

    //run config file
    retval=system("halcmd -i /home/u/cnc/configs/ppmc/ppmc.ini -f /home/u/cnc/configs/ppmc/mark.hal ");
    if (retval != 0) {
        printf("ERROR:halcmd -i /home/u/cnc/configs/ppmc/ppmc.ini -f /home/u/cnc/configs/ppmc/mark.hal fail\n");
        exit(-1);
    }
}
