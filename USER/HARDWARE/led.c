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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t _run_led_flag;
volatile uint8_t _run_led_cnt;
volatile uint8_t _can_led_flag;
volatile uint8_t _can_led_cnt;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  led_msTask.
  * @param  None
  * @retval None
  */
void led_msTask(void) {
	if (_run_led_cnt) {
		_run_led_cnt--;
		if (!_run_led_cnt) {
			_run_led_flag = 1;
		}
	}
	if (_can_led_cnt) {
		_can_led_cnt--;
		if (!_can_led_cnt) {
			_can_led_flag = 1;
		}
	}
}
/**
  * @brief  led_init.
  * @param  None
  * @retval None
  */
void appled_init(void) {
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
	GPIO_WriteBit(SOS_LED_PORT, SOS_LED_PIN, LED_DARK);
    GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
	GPIO_WriteBit(CAN_LED_PORT, CAN_LED_PIN, LED_DARK);
    GPIO_WriteBit(APITX_LED_PORT, APITX_LED_PIN, LED_DARK);
    GPIO_WriteBit(APIRX_LED_PORT, APIRX_LED_PIN, LED_DARK);	
	_run_led_flag = 0;
	_run_led_cnt = RUN_LED_INTERVAL;
	_can_led_cnt = 0;
	_can_led_flag = 0;
	t_ostmr_insertTask(led_msTask, 100, OSTMR_PERIODIC); //100ms task
}
/**
  * @brief  led_process. ledµÆ¿ØÖÆ
  * @param  None
  * @retval None
  */
void appled_prcs(void) {
	if (_can_led_flag) {
		_can_led_flag = 0;
		GPIO_WriteBit(CAN_LED_PORT, CAN_LED_PIN, LED_DARK);
	}
	if (_run_led_flag) {
		_run_led_flag = 0;
		GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_DARK);
	}
//	if(_run_led_flag)
//	{
//		_run_led_flag = 0;
//		_run_led_cnt = RUN_LED_INTERVAL;
//		if(GPIO_ReadInputDataBit(RUN_LED_PORT, RUN_LED_PIN))
//		{
//			GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
//		}
//		else
//		{
//			GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_DARK);
//		}
//	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
