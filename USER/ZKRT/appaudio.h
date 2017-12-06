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
#include "appfiles.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//volume value mapping
#define VOLUME_MAP(V)                (u8)(0.63*(V))   //zkrt_notice: 0.63 by wm8978 value limit           

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
	START_S_APY,            //开始播放
	PLAYING_S_APY,          //播放中
	PAUSE_S_APY,            //暂停中
	CONTINUE_S_APY,         //继续播放
	STOP_S_APY,             //停止播放
	OVER_S_APY              //播放结束
}audioPlayState;

//audio play mode
typedef enum
{ 
	NORMAL_MAPY =0,         //普通模式，播放完成就不再播放
	SINGLELOOP_MAPY,        //单曲循环
	LISTLOOP_MAPY           //列表循环
}audioPlayMode;

//audio info struct use in application 
typedef struct
{
/************音频处理标记**************************************/	
	u8 volume;                 //音量
	audioPlayMode play_mode;	 //播放模式
	
/************录音处理相关标记**********************************/		
	recorderState rec_state;   //录音状态
	u8 rec_id;                 //当前录音的ID码
	audioinfo_st *rec_item;    //当前录音的ID目标，need point
	bool rec_enable;           //是否允许录音标记
	
/************播放音频相关标记**********************************/	
	audioPlayState play_state; //播放状态
	u8 play_id;                //当前播放的音频ID码
	audioinfo_st *play_item;   //当前播放的ID目标，need point
	bool play_enable;          //是否允许播放标记
	
	
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
