#ifndef __PWM_H
#define __PWM_H

#include "sys.h"

//pwm value
#define PWM_MIN_V  500
#define PWM_MAX_V  2500
#define PWM_MEDIUM_V ((PWM_MAX_V+PWM_MIN_V)/2)
#define PWM_INIT_V PWM_MEDIUM_V

//scale remap, scale vale is 0~100
#define SCALE_MIN_V 0
#define SCALE_MAX_V 100
#define SCALE_MEDIUM_V ((SCALE_MAX_V+SCALE_MIN_V)/2)
#define SCALE_INIT_V SCALE_MEDIUM_V
#define SCALE2PWM_MAP_K ((PWM_MAX_V-PWM_MIN_V)/(SCALE_MAX_V-SCALE_MIN_V))
#define SCALE2PWM(scale_v) (SCALE2PWM_MAP_K*(scale_v)+(PWM_MAX_V-SCALE2PWM_MAP_K*SCALE_MAX_V))

#define PWM2SCALE_MAP_K ((SCALE_MAX_V-SCALE_MIN_V)/(PWM_MAX_V-PWM_MIN_V))
#define PWM2SCALE(pwm_v) (PWM2SCALE_MAP_K*(pwm_v)+(SCALE_MAX_V-PWM2SCALE_MAP_K*PWM_MAX_V))

#define THROW_PWM1(value) TIM_SetCompare1(TIM3, value)
#define THROW_PWM2(value) TIM_SetCompare2(TIM3, value)

#define GET_PWM1 TIM_GetCapture1(TIM3)
#define GET_PWM2 TIM_GetCapture2(TIM3)

void pwm_init(void);

#endif
