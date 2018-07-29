/**
  ******************************************************************************
  * @file    timer.c
  * @author  ZKRT
  * @version V1.0
  * @date    2018-07-24
  * @brief   
	*					 + (1) init
	*
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "timer.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile app_timer_flag_st app_timer_flag = {0};
volatile app_timer_cnt_st app_timer_cnt = {0};
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  app_100ms_task.
  * @param  None
  * @retval None
  */
void app_100ms_task(void)
{
    if (app_timer_cnt.psdk_peroid_upload)
    {
        app_timer_cnt.psdk_peroid_upload--;
        if (!app_timer_cnt.psdk_peroid_upload)
        {
            app_timer_flag.psdk_peroid_upload = 1;
        }
    }
    if (app_timer_cnt.run_led)
    {
        app_timer_cnt.run_led--;
        if (!app_timer_cnt.run_led)
        {
            app_timer_flag.run_led = 1;
        }
    }
    if (app_timer_cnt.api_tx)
    {
        app_timer_cnt.api_tx--;
        if (!app_timer_cnt.api_tx)
        {
            app_timer_flag.api_tx = 1;
        }
    }
    if (app_timer_cnt.api_rx)
    {
        app_timer_cnt.api_rx--;
        if (!app_timer_cnt.api_rx)
        {
            app_timer_flag.api_rx = 1;
        }
    }            
}
/**
  * @brief  timer_init.
  * @param  None
  * @retval None
  */
void timer_init(void)
{
    app_timer_cnt.psdk_peroid_upload = PSDK_PER_CNT;
    app_timer_cnt.run_led = RUN_LED_CNT;
    t_ostmr_insertTask(app_100ms_task, TIMER_UT, OSTMR_PERIODIC); //100ms task
}
