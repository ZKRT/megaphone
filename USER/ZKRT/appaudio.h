/**
  ******************************************************************************
  * @file    appaudio.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for appaudio.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPAUDIO_H
#define __APPAUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

//recorder state
typedef enum
{ 
	IDLE_S_REC =0,          //空闲
	START_S_REC,            //开始录音
	RECORDING_S_REC,        //录音中
	STOP_S_REC,             //停止录音
	OVER_S_REC              //录音结束
}recorderState;

//audio play state
typedef enum
{ 
	IDLE_S_APY =0,          //空闲
	PLAYING_S_APY,          //播放中
	PAUSE_S_APY,            //暂停中
	STOP_S_APY,             //停止播放
	OVER_S_APY              //播放结束
}audioPlayState;

//audio play mode
typedef enum
{ 
	NORMAL_MAPY =0,         //普通模式
	SINGLELOOP_MAPY,        //单曲循环
	LISTLOOP_MAPY           //列表循环
}audioPlayMode;

//audio info struct use in application 
typedef struct
{
	recorderState rec_state;   //录音状态
	audioPlayState play_state; //播放状态
	u8 volume;                 //音量
	
	
}appaudio_st;


/* Exported functions ------------------------------------------------------- */


#endif /* __APPAUDIO_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
