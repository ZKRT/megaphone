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
#define VOLUME_MAP(V)                (u8)(0.29*(V)+28)   //zkrt_notice: 0.30 by wm8978 value limit //��С˥���ֱ���ֵ       
#define VOLUME_INIT_VALUE            5  //zkrt_debug

/* Exported types ------------------------------------------------------------*/

//recorder state
typedef enum {
	IDLE_S_REC = 0,         //����
	START_S_REC,            //��ʼ¼��
	RECORDING_S_REC,        //¼����
	PAUSE_S_REC,            //��ͣ¼����
	CONTINUE_S_REC,         //����¼��
	STOP_S_REC,             //ֹͣ¼��
	FAIL_S_REC,             //¼��ʧ�ܣ�¼�������г��ִ���
	OVER_S_REC              //¼������
} recorderState;

//audio play state
typedef enum {
	IDLE_S_APY = 0,         //����
	START_S_APY,            //��ʼ����
	PLAYING_S_APY,          //������
	PAUSE_S_APY,            //��ͣ��
	CONTINUE_S_APY,         //��������
	STOP_S_APY,             //ֹͣ����
	OVER_S_APY              //���Ž���
} audioPlayState;

//audio play mode
typedef enum {
	NORMAL_MAPY = 0,        //��ͨģʽ��������ɾͲ��ٲ���
	SINGLELOOP_MAPY,        //����ѭ��
	LISTLOOP_MAPY           //�б�ѭ��
} audioPlayMode;

//aduio play struct,include app level and low tx level and file
typedef struct {
//app level
	audioPlayState play_state; //����״̬  //when play over must reset
	audioPlayMode play_mode;	 //����ģʽ
	u8 out_flag;           //�Ƿ����bypass

//file
	char play_objname[DIR_NAME_MAXLEN + AUDIO_NAME_LEN + 2]; //���ڲ��ŵľ���·����
	FIL play_fil;              //when play over must be clear
	__wavctrl wavctrl;		     //���ŵ�wav�ļ���WAV���ƽṹ��

//low tx level
	u8 i2sbuf1[WAV_I2S_TX_DMA_BUFSIZE];     //2��I2S�����BUF
	u8 i2sbuf2[WAV_I2S_TX_DMA_BUFSIZE];
	u8 tbuf[WAV_I2S_TX_DMA_BUFSIZE];				//��ʱ����,����24bit�����ʱ����Ҫ�õ�
	vu8 wavtransferend;	                    //i2s������ɱ�־
	vu8 wavwitchbuf;		                    //i2sbufxָʾ��־

} audioplay_st;

//audio record struct,include app level and low rx level and file
typedef struct {
//app level
	recorderState rec_state;   //¼��״̬
	u8 rec_out_flag;                        //¼�������ǣ��Ƿ����bypass

//file
	char rec_objname[DIR_NAME_MAXLEN + AUDIO_NAME_LEN + 2]; //����·����
	FIL rec_fil;               //when rec over must be clear
	__WaveHeader wavhead;                   //¼��ͷ

//low rx level
	vu8 i2srecbuf1[I2S_RX_DMA_BUF_SIZE];    //¼��IIS DMA�洢buffer��˫buffer
	vu8 i2srecbuf2[I2S_RX_DMA_BUF_SIZE];
	vu32 wavsize;                           //wav���ݴ�С(�ֽ���,�������ļ�ͷ!!) ��¼��ʱ���ϼ�¼
} audiorec_st;

//audio info struct use in application
typedef struct {
	/************��Ƶ������**************************************/
	u8 volume;                 //����
	/************¼��������ر��**********************************/
	u8 rec_id;                 //��ǰ¼����ID��
	audioinfo_st *rec_item;    //��ǰ¼����IDĿ�꣬ʹ��ʱָ��infolist_pst��ĳ��item
	bool rec_enable;           //�Ƿ�����¼�����
	audioplay_st *audioplay;   //need point
	/************������Ƶ��ر��**********************************/
	u8 play_id;                //��ǰ���ŵ���ƵID��
	audioinfo_st *play_item;   //��ǰ���ŵ�IDĿ�꣬ʹ��ʱָ��infolist_pst��ĳ��item
	bool play_enable;          //�Ƿ������ű��
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
