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
#include "wavplay.h"
#include "recorder.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//volume value mapping
//#define VOLUME_MAP(V)                (u8)(0.57*(V))   //zkrt_notice: 0.57 by wm8978 value limit  //0.57 is max 0db gain, reg value is 0b111001,see wm8978 doc
#define VOLUME_MAP(V)                (u8)(0.29*(V)+28)   //zkrt_notice: 0.30 by wm8978 value limit //减小衰弱分贝阈值       
#define VOLUME_INIT_VALUE            5  //zkrt_debug

/* Exported types ------------------------------------------------------------*/

//recorder state
typedef enum {
	IDLE_S_REC = 0,         //空闲
	START_S_REC,            //开始录音
	RECORDING_S_REC,        //录音中
	PAUSE_S_REC,            //暂停录音中
	CONTINUE_S_REC,         //继续录音
	STOP_S_REC,             //停止录音
	FAIL_S_REC,             //录音失败，录音过程中出现错误
	OVER_S_REC              //录音结束
} recorderState;

//audio play state
typedef enum {
	IDLE_S_APY = 0,         //空闲
	START_S_APY,            //开始播放
	PLAYING_S_APY,          //播放中
	PAUSE_S_APY,            //暂停中
	CONTINUE_S_APY,         //继续播放
	STOP_S_APY,             //停止播放
	OVER_S_APY              //播放结束
} audioPlayState;

//audio play mode
typedef enum {
	NORMAL_MAPY = 0,        //普通模式，播放完成就不再播放
	SINGLELOOP_MAPY,        //单曲循环
	LISTLOOP_MAPY           //列表循环
} audioPlayMode;

//aduio play struct,include app level and low tx level and file
typedef struct {
//app level
	audioPlayState play_state; //播放状态  //when play over must reset
	audioPlayMode play_mode;	 //播放模式
	u8 out_flag;           //是否输出bypass

//file
	char play_objname[DIR_NAME_MAXLEN + AUDIO_NAME_LEN + 2]; //正在播放的绝对路径名
	FIL play_fil;              //when play over must be clear
	__wavctrl wavctrl;		     //播放的wav文件的WAV控制结构体

//low tx level
	u8 i2sbuf1[WAV_I2S_TX_DMA_BUFSIZE];     //2个I2S解码的BUF
	u8 i2sbuf2[WAV_I2S_TX_DMA_BUFSIZE];
	u8 tbuf[WAV_I2S_TX_DMA_BUFSIZE];				//零时数组,仅在24bit解码的时候需要用到
	vu8 wavtransferend;	                    //i2s传输完成标志
	vu8 wavwitchbuf;		                    //i2sbufx指示标志

} audioplay_st;

//audio record struct,include app level and low rx level and file
typedef struct {
//app level
	recorderState rec_state;   //录音状态
	u8 rec_out_flag;                        //录音输出标记，是否输出bypass

//file
	char rec_objname[DIR_NAME_MAXLEN + AUDIO_NAME_LEN + 2]; //绝对路径名
	FIL rec_fil;               //when rec over must be clear
	__WaveHeader wavhead;                   //录音头

//low rx level
	vu8 i2srecbuf1[I2S_RX_DMA_BUF_SIZE];    //录音IIS DMA存储buffer，双buffer
	vu8 i2srecbuf2[I2S_RX_DMA_BUF_SIZE];
	vu32 wavsize;                           //wav数据大小(字节数,不包括文件头!!) 在录音时不断记录
} audiorec_st;

//audio info struct use in application
typedef struct {
	/************音频处理标记**************************************/
	u8 volume;                 //音量
	/************录音处理相关标记**********************************/
	u8 rec_id;                 //当前录音的ID码
	audioinfo_st *rec_item;    //当前录音的ID目标，使用时指向infolist_pst的某个item
	bool rec_enable;           //是否允许录音标记
	audioplay_st *audioplay;   //need point
	/************播放音频相关标记**********************************/
	u8 play_id;                //当前播放的音频ID码
	audioinfo_st *play_item;   //当前播放的ID目标，使用时指向infolist_pst的某个item
	bool play_enable;          //是否允许播放标记
	audiorec_st *audiorec;
} appaudio_st;


/* Exported functions ------------------------------------------------------- */
void appaudio_prcs(void);
void appaudio_init(void);

//wav
extern bool wav_play_start(void);
extern u8 wav_playing(void);
extern void wav_play_over(void);
extern void wav_play_stop(void);
extern void wav_play_pause(void);
extern void wav_play_continue(void);
extern void wav_play_over_check_mode(appaudio_st* audio);
#endif /* __APPAUDIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
