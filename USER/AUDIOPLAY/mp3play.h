/**
  ******************************************************************************
  * @file    mp3play.h
  * @author  ZKRT
  * @version V1.0
  * @date    23-March-2018
  * @brief   Header for mp3play.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MP3PLAY_H
#define __MP3PLAY_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "coder.h"
/* Exported macro ------------------------------------------------------------*/
//user decode macro
#define READBUF_SIZE         2048
#define MP3_OUTPUT_SIZE      (MAX_NCHAN*1152)

/* Exported constants --------------------------------------------------------*/
//control num enum
typedef enum {
	Normal_MP3S = 0,
	Error_MP3S,
	ReadOver_MP3S,
	ReadFileFail_MP3S,
	OpenFileFail_MP3S,
	FrameNotSupport_MP3S,
	DecodeFail_MP3S,
	SampleRateNotSupport_MP3S
} mp3_user_dec_state_em;
typedef enum {
	None_Trans = 0,
	I2SInit_Trans,
	JustTrans_Trans,
	I2SStop_Trans
} mp3_user_trans_state_em;
/* Exported types ------------------------------------------------------------*/

//mp3 user decode struct
typedef struct {
	u8  readBuf[READBUF_SIZE];  //mp3原始数据，MP3文件里读取到的
	u8 *readPtr;                //指向帧头，根据offset指向readBuf的位置
	int bytesLeft;              //剩余有效帧数据大小
	s32 offset;                 //帧头偏移位置，有效帧在readBuf里的偏移
	bool foudStartOfFrame;      //是否找到有效帧标记
	short *output;              //mp3解析得到的帧，指针指向bufferX
	short *buffer1;             //双缓存发送，指向_audioplay.i2sbuf1
	short *buffer2;	            //双缓存发送，指向_audioplay.i2sbuf2
	bool get_frame_info;        //第一次解析需要获取帧信息，包括采样频率等等信息
	u32 frame_cnt;              //帧计数
	u16 decode_fail_cnt;        //解码出错计数
	mp3_user_dec_state_em state;//状态
	bool need_wait_transcomle;  //需要等待传输完成
} mp3_user_dec_st;

//mp3 app user struct //用户可能需要关心的参数
typedef struct {
	u8 decoder_status;          //解析器状态, 0-不可用,1-可用，初始化状态获取

} mp3_user_app_st;

/* Exported functions ------------------------------------------------------- */
extern HMP3Decoder _mp3decoder;
extern MP3FrameInfo _mp3FrameInfo;

u8 mp3_get_frame_info_by_file(MP3FrameInfo *mp3FrameInfo, const char *fname);
void mp3_prcs(void);
u8 mp3_play_start(void);
void mp3_playing(void);
#endif /* __APPAUDIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
