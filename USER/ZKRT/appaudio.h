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
	IDLE_S_REC =0,          //����
	START_S_REC,            //��ʼ¼��
	RECORDING_S_REC,        //¼����
	STOP_S_REC,             //ֹͣ¼��
	OVER_S_REC              //¼������
}recorderState;

//audio play state
typedef enum
{ 
	IDLE_S_APY =0,          //����
	PLAYING_S_APY,          //������
	PAUSE_S_APY,            //��ͣ��
	STOP_S_APY,             //ֹͣ����
	OVER_S_APY              //���Ž���
}audioPlayState;

//audio play mode
typedef enum
{ 
	NORMAL_MAPY =0,         //��ͨģʽ
	SINGLELOOP_MAPY,        //����ѭ��
	LISTLOOP_MAPY           //�б�ѭ��
}audioPlayMode;

//audio info struct use in application 
typedef struct
{
	recorderState rec_state;   //¼��״̬
	audioPlayState play_state; //����״̬
	u8 volume;                 //����
	
	
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
