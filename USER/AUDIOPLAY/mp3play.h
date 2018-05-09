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
	u8  readBuf[READBUF_SIZE];  //mp3ԭʼ���ݣ�MP3�ļ����ȡ����
	u8 *readPtr;                //ָ��֡ͷ������offsetָ��readBuf��λ��
	int bytesLeft;              //ʣ����Ч֡���ݴ�С
	s32 offset;                 //֡ͷƫ��λ�ã���Ч֡��readBuf���ƫ��
	bool foudStartOfFrame;      //�Ƿ��ҵ���Ч֡���
	short *output;              //mp3�����õ���֡��ָ��ָ��bufferX
	short *buffer1;             //˫���淢�ͣ�ָ��_audioplay.i2sbuf1
	short *buffer2;	            //˫���淢�ͣ�ָ��_audioplay.i2sbuf2
	bool get_frame_info;        //��һ�ν�����Ҫ��ȡ֡��Ϣ����������Ƶ�ʵȵ���Ϣ
	u32 frame_cnt;              //֡����
	u16 decode_fail_cnt;        //����������
	mp3_user_dec_state_em state;//״̬
	bool need_wait_transcomle;  //��Ҫ�ȴ��������
} mp3_user_dec_st;

//mp3 app user struct //�û�������Ҫ���ĵĲ���
typedef struct {
	u8 decoder_status;          //������״̬, 0-������,1-���ã���ʼ��״̬��ȡ

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
