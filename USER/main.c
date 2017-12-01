/**
  ******************************************************************************
  * @file    main.c 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BSP.h"
#include "sys.h"
#include "osusart.h"
#include "appcan.h"
#include "led.h"
#include "key.h"
#include "exfuns.h"
#include "wm8978.h"
#include "recorder.h"
#include "myiic.h"
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
uint8_t testbuf[100]="HELLO";
uint16_t testbuf_len;
int haha = 1;
extern Key_st all_key[KEY_NUM];
float ff;RCC_ClocksTypeDef RCC_Clocks;
int res;
u32 total,_free;
int main(void)
{
	BSP_Init();
	memcpy(&ff, &haha, 4);
	appcan_init();
	appled_init();
	appkey_init();
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(60,60);	//耳机音量设置
	WM8978_SPKvol_Set(60);		//喇叭音量设置	
//	ostmr_wait(50);  //wait 5 senconds for sensor for ready
  /* Infinite Loop */
	RCC_GetClocksFreq(&RCC_Clocks);
	printf("hello man=, %f\n", ff);
	res=exf_getfree((u8 *)"0:",&total,&_free);
	if(res==0)
	{
		printf("sd Total Size:     %d MB\n", total>>10);	 
		printf("sd Free Size:     %d MB\n", _free>>10); 	    
	} 
	appfatfs_test();
  while(1)
  {
		/* product code start, the proposed process should not be delayed*/
		appled_prcs();                                       //LED流程
		appcan_prcs();
		appkey_prcs();   
//		wav_recorder();
//		ostmr_wait(5);
//		printf("key %d, %d, %d, %d\n", all_key[KEY1_NUM].jitter_val, all_key[KEY2_NUM].jitter_val, all_key[KEY3_NUM].jitter_val, all_key[KEY4_NUM].jitter_val);
//		appuart_test_prcs();
//		appaudio_prcs();
		/* product code end*/
		
//		/* test code start*/
		appfatfs_test();
		if(t_osscomm_ReceiveMessage(testbuf, &testbuf_len, USART_TEST_NUM)==SCOMM_RET_OK)
		{
			t_osscomm_sendMessage(testbuf, testbuf_len, USART_TEST_NUM);
		}	
		 
//		/* test code end*/
  }
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
