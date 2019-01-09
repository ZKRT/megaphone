#ifndef __PWM_H
#define __PWM_H

#include "sys.h"

#define PWM_CHANNEL_NUM 2
//pwm low hardware control
#define THROW_PWM1(value) TIM_SetCompare1(TIM3, value)
#define THROW_PWM2(value) TIM_SetCompare2(TIM3, value)
#define GET_PWM1 TIM_GetCapture1(TIM3)
#define GET_PWM2 TIM_GetCapture2(TIM3)
//pwm value
#define PWM_MIN_V  500
#define PWM_MAX_V  2500
#define PWM_MEDIUM_V ((PWM_MAX_V+PWM_MIN_V)/2)
#define PWM_INIT_V PWM_MEDIUM_V
//min or max pwm value for program restriction
#define PWM_MIN_V_ACTUAL  PWM_MEDIUM_V 
#define PWM_MAX_V_ACTUAL  2200 
//scale remap, scale vale is 0~100
#define SCALE_MIN_V 0
#define SCALE_MAX_V 100
#define SCALE_MEDIUM_V ((SCALE_MAX_V+SCALE_MIN_V)/2)
#define SCALE_INIT_V SCALE_MEDIUM_V
//scale to pwm map
#define SCALE2PWM_MAP_K ((PWM_MAX_V_ACTUAL-PWM_MIN_V_ACTUAL)/(SCALE_MAX_V-SCALE_MIN_V))
#define SCALE2PWM(scale_v) (SCALE2PWM_MAP_K*(scale_v)+(PWM_MAX_V_ACTUAL-SCALE2PWM_MAP_K*SCALE_MAX_V))
//pwm to scale map
#define PWM2SCALE_MAP_K ((SCALE_MAX_V-SCALE_MIN_V)/(PWM_MAX_V_ACTUAL-PWM_MIN_V_ACTUAL))
#define PWM2SCALE(pwm_v) (PWM2SCALE_MAP_K*(pwm_v)+(SCALE_MAX_V-PWM2SCALE_MAP_K*PWM_MAX_V_ACTUAL))
//scale init value
#define PWMSCALE_INIT_V 0

typedef struct{
#define PWM_TASK_TIME 2 //2ms per
#define PWM_CHANGE_TIME (6/PWM_TASK_TIME)  //per 6ms to change pwm value
#define PWM_CHANGE_PERV 2 //change value per cnt
    short final_v;
    short run_v;
    uint8_t flag;
    uint8_t timecnt;
}pwm_timer_st;

void pwm_init(void);
void pwm_prcs(void);
void pwm_set_dit(uint8_t ch, short value);
bool pwm_set_soft(uint8_t ch, short value);
#endif
