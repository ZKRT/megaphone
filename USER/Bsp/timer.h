/**
  ******************************************************************************
  * @file    timer.h
  * @author  ZKRT
  * @version V1.0
  * @date    2018-07-24
  * @brief
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/
#define TIMER_UT 100
////timer unit is ms
#define PSDK_PER_CNT (1000 / TIMER_UT)
#define RUN_LED_CNT (500 / TIMER_UT)
#define APITX_LED_CNT (100 / TIMER_UT)
#define APIRX_LED_CNT (100 / TIMER_UT)
/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8_t psdk_peroid_upload;
    uint8_t run_led;
    uint8_t api_tx;
    uint8_t api_rx;
} app_timer_flag_st;
typedef struct
{
    uint8_t psdk_peroid_upload;
    uint8_t run_led;
    uint8_t api_tx;
    uint8_t api_rx;
} app_timer_cnt_st;
/* Exported constants --------------------------------------------------------*/
extern volatile app_timer_flag_st app_timer_flag;
extern volatile app_timer_cnt_st app_timer_cnt;
/* Exported functions ------------------------------------------------------- */
void timer_init(void);

#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
