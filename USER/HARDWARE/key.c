/**
  ******************************************************************************
  * @file    key.c
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
#include "key.h"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Key_st all_key[KEY_NUM];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void key_msTask(void);
static void key_scan(void);
static void key_handle(void);
static void user_key_handle(void);
/**
  * @brief  appkey_init.
  * @param  None
  * @retval None
  */
void appkey_init(void)
{
	int i;
	for(i=0; i<KEY_NUM; i++)
	{
		all_key[i].jitter_val = KEY_RELEASED;
		all_key[i].lastj_val = KEY_RELEASED;
		all_key[i].pres = KEY_RELEASED;
		all_key[i].timecnt = 0;
		all_key[i].timestart = 0;
	}
	t_ostmr_insertTask(key_msTask, 100, OSTMR_PERIODIC); //100ms task
}
/**
  * @brief  appkey_prcs
  * @param  None
  * @retval None
  */
void appkey_prcs(void)
{
	key_scan();
	key_handle(); 
	user_key_handle();
}
/**
  * @brief  key_msTask.
  * @param  None
  * @retval None
  */
static void key_msTask(void)
{
	int i;
	for(i=0; i<KEY_NUM; i++)
	{
		if(all_key[i].timestart == 1)
		{
				all_key[i].timecnt++;
		}
	}
}
/**
  * @brief  key_scan
  * @param  None
  * @retval None
  */
static void key_scan(void)
{
	int i;
	all_key[KEY1_NUM].jitter_val = KEY1_V;
	all_key[KEY2_NUM].jitter_val = KEY2_V;
	all_key[KEY3_NUM].jitter_val = KEY3_V;
	all_key[KEY4_NUM].jitter_val = KEY4_V;
	
	//key count control
	for(i=0; i<KEY_NUM; i++)
	{
		if(all_key[i].jitter_val == KEY_PRESED)
		{
			all_key[i].pres = KEY_RELEASED; //reset
			all_key[i].timestart = 1;  //continue count
			if(all_key[i].lastj_val == KEY_RELEASED)
				all_key[i].timecnt = 0;  //recount
		}
		else
		{
			all_key[i].timestart = 0; //stop count
			if((all_key[i].timecnt >= JITTER_CNT)&&(all_key[i].pres == KEY_RELEASED))  
			{
				all_key[i].pres = KEY_PRESED;  //this presed event must be reset when user handled
			}
		}
	}
	
	//set last key values
	for(i=0; i<KEY_NUM; i++)
	{
			all_key[i].lastj_val = all_key[i].jitter_val;
	}
	
}
/**
  * @brief  key_handle
  * @param  None
  * @retval None
  */
static void key_handle(void)
{//nothing to do temporary
}
/**
  * @brief  user_key_handle
  * @param  None
  * @retval None
  */
static void user_key_handle(void)
{
//	if(key_pressed(0, 0) == KEY_PRESED)
//	{
//	}
}
/**
* @brief  key_pressed: 
* @param  num: key number
* @param  sec: the time of key pressed, unit is second
  * @retval KEY_PRESED or KEY_RELEASED
  */
uint8_t key_pressed(uint8_t num, uint8_t sec)
{
	if((all_key[num].timecnt*100 >= sec*1000+JITTER_CNT)&&(all_key[num].pres == KEY_PRESED))  
	{
		all_key[num].pres = KEY_RELEASED;  //this presed event must be reset when user handled
		all_key[num].timecnt = 0;
		printf("key[%d] pressed\n", num);
		return KEY_PRESED;
	}
	return KEY_RELEASED;
}
/**
* @brief  which_key_is_pressed: 
* @param  num: key number
* @param  sec: the time of key pressed, unit is second
  * @retval KEY_PRESED or KEY_RELEASED
  */
uint8_t which_key_is_pressed(void)
{
	int i;
	key_scan();
	for(i=0; i<KEY_NUM; i++)
	{
		if(key_pressed(i, 0) ==KEY_PRESED)
				return i;
	}
	return 0xff;
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
