/**
  ******************************************************************************
  * @file    led.c
  * @author  ZKRT
  * @version V1.0
  * @date    18-May-2017
  * @brief   led
  *
 ===============================================================================
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "led.h"
#include "timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  led_init.
  * @param  None
  * @retval None
  */
void appled_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA, ENABLE);
    /*LED Pins configuration  *************************************************/
    GPIO_InitStructure.GPIO_Pin = RUN_LED_PIN | APITX_LED_PIN | APIRX_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //all pin's port is gpiob
    GPIO_InitStructure.GPIO_Pin = SOS_LED_PIN; //sos led
    GPIO_Init(SOS_LED_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
    GPIO_WriteBit(APITX_LED_PORT, APITX_LED_PIN, LED_DARK);
    GPIO_WriteBit(APIRX_LED_PORT, APIRX_LED_PIN, LED_DARK);
    GPIO_WriteBit(SOS_LED_PORT, SOS_LED_PIN, LED_DARK);
}
/**
  * @brief  led_process. ledµÆ¿ØÖÆ
  * @param  None
  * @retval None
  */
void appled_prcs(void)
{
    if (app_timer_flag.run_led)
    {
        app_timer_flag.run_led = 0;
        app_timer_cnt.run_led = RUN_LED_CNT;
        if (GPIO_ReadInputDataBit(RUN_LED_PORT, RUN_LED_PIN))
            GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
        else
            GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_DARK);
    }
    if (app_timer_flag.api_tx)
    {
        app_timer_flag.api_tx = 0;
        GPIO_WriteBit(APITX_LED_PORT, APITX_LED_PIN, LED_DARK);
    }
    if (app_timer_flag.api_rx)
    {
        app_timer_flag.api_rx = 0;
        GPIO_WriteBit(APIRX_LED_PORT, APIRX_LED_PIN, LED_DARK);
    }
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
