#include "pwm.h"
//////////////////////////////////////////////////varialbe declare
pwm_timer_st pwm_timerc[PWM_CHANNEL_NUM]={0};
//////////////////////////////////////////////////funtion declare
static void pwm_timer_task(void);
static short pwm_v_run_2_final(short final_v, short source_v);
static void pwm_run_2_final_handle(void);
//////////////////////////////////////////////////function complement
/**
 * @brief  
 * @param  
 * @retval 
 */
void pwm_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* GPIOB Clocks enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    //ÅäÖÃPWM
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = PWM_MAX_V * 10 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_Pulse = PWM_INIT_V;

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    TIM_Cmd(TIM3, ENABLE);

    t_systmr_insertQuickTask(pwm_timer_task, PWM_TASK_TIME, OSTMR_PERIODIC);
}
/**
 * @brief  ÒÀ¿¿Ê±¼ä»ºÂý¿ØÖÆPWMÖµ
 * @param  
 * @retval 
 */
void pwm_prcs(void)
{
    pwm_run_2_final_handle();
}
void pwm_set_dit(uint8_t ch, short value)
{
    if (ch == 1)
        THROW_PWM1(value);
    else if (ch == 2)
        THROW_PWM2(value);
}
short pwm_get_dit(uint8_t ch)
{
    if (ch == 1)
        return GET_PWM1;
    else
        return GET_PWM2;
}
/**
 * @brief  
 * @param  ch is 1~PWM_CHANNEL_NUM
 * @retval 
 */
bool pwm_set_soft(uint8_t ch, short value)
{
    if ((ch > PWM_CHANNEL_NUM) || (ch == 0) || (value > PWM_MAX_V) || (value < PWM_MIN_V))
        return false;
    pwm_timerc[ch - 1].final_v = value;
    pwm_timerc[ch - 1].run_v = pwm_get_dit(ch);
    pwm_timerc[ch - 1].run_v = pwm_v_run_2_final(pwm_timerc[ch - 1].final_v, pwm_timerc[ch - 1].run_v);
    pwm_set_dit(ch, pwm_timerc[ch - 1].run_v);
    pwm_timerc[ch - 1].flag = 0;
    pwm_timerc[ch - 1].timecnt = PWM_CHANGE_TIME;
    return true;
}
/**
 * @brief  
 * @param  
 * @retval 
 */
static short pwm_v_run_2_final(short final_v, short source_v)
{
    short new_v;
    if (final_v >= source_v)
    {
        new_v = (final_v - source_v) >= PWM_CHANGE_PERV ? (source_v + PWM_CHANGE_PERV) : final_v;
    }
    else
    {
        new_v = (source_v - final_v) >= PWM_CHANGE_PERV ? (source_v - PWM_CHANGE_PERV) : final_v;
    }
    return new_v;
}
/**
 * @brief  
 * @param  
 * @retval 
 */
static void pwm_run_2_final_handle(void)
{
    int i=0;
    for (i = 0; i < PWM_CHANNEL_NUM; i++)
    {
        if (pwm_timerc[i].flag)
        {
            pwm_timerc[i].flag = 0;
            pwm_timerc[i].run_v = pwm_v_run_2_final(pwm_timerc[i].final_v, pwm_timerc[i].run_v);
            pwm_set_dit(i+1, pwm_timerc[i].run_v);
            printf("pwm[%d]%d\n", i+1, pwm_timerc[i].run_v);
            if (pwm_timerc[i].run_v == pwm_timerc[i].final_v)  //ctrl over
                pwm_timerc[i].timecnt = 0;
            else
                pwm_timerc[i].timecnt = PWM_CHANGE_TIME;
        }
    }
}
/**
 * @brief  
 * @param  
 * @retval 
 */
static void pwm_timer_task(void)
{
    int i=0;
    for (i = 0; i < PWM_CHANNEL_NUM; i++)
    {
        if (pwm_timerc[i].timecnt)
        {
            pwm_timerc[i].timecnt--;
            if(pwm_timerc[i].timecnt==0)
                pwm_timerc[i].flag = 1;
        }
    }
}
