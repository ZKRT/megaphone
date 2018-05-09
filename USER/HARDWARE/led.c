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
	GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_DARK);
	GPIO_WriteBit(CAN_LED_PORT, CAN_LED_PIN, LED_DARK);
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
