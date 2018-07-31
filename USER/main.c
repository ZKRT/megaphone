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
//#include "appcan.h"
#include "led.h"
#include "key.h"
#include "exfuns.h"
#include "wm8978.h"
#include "recorder.h"
#include "myiic.h"
#include "appfiles.h"
#include "appaudio.h"
#include "mp3play.h"
#include "timer.h"
#include "application.h"
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    BSP_Init();
    // appcan_init();
    appled_init();
    appkey_init();
    checkfileready();
    if (is_file_exsit)
        appfatfs_test();
    if (is_file_exsit)
        appfile_init();
    appaudio_init();
    timer_init();
    psdk_init();
    while (1)
    {
        /* product code start, the proposed process should not be delayed*/
        appled_prcs(); //LEDÁ÷³Ì
        // appcan_prcs();
        appkey_prcs();
        appaudio_prcs();
        psdk_prcs();
        /* product code end*/
        //		/* test code start*/
        //  	mp3play();
        //		printf("key %d, %d, %d, %d\n", all_key[KEY1_NUM].jitter_val, all_key[KEY2_NUM].jitter_val, all_key[KEY3_NUM].jitter_val, all_key[KEY4_NUM].jitter_val);
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
