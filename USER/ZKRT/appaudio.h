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
	START_S_APY,            //��ʼ����
	PLAYING_S_APY,          //������
	PAUSE_S_APY,            //��ͣ��
	CONTINUE_S_APY,         //��������
	STOP_S_APY,             //ֹͣ����
	OVER_S_APY              //���Ž���
}audioPlayState;

//audio play mode
typedef enum
{ 
	NORMAL_MAPY =0,         //��ͨģʽ��������ɾͲ��ٲ���
	SINGLELOOP_MAPY,        //����ѭ��
	LISTLOOP_MAPY           //�б�ѭ��
}audioPlayMode;

//audio info struct use in application 
typedef struct
{
/************��Ƶ������**************************************/	
	u8 volume;                 //����
	audioPlayMode play_mode;	 //����ģʽ
	
/************¼��������ر��**********************************/		
	recorderState rec_state;   //¼��״̬
	u8 rec_id;                 //��ǰ¼����ID��
	audioinfo_st *rec_item;    //��ǰ¼����IDĿ�꣬need point
	bool rec_enable;           //�Ƿ�����¼�����
	
/************������Ƶ��ر��**********************************/	
	audioPlayState play_state; //����״̬
	u8 play_id;                //��ǰ���ŵ���ƵID��
	audioinfo_st *play_item;   //��ǰ���ŵ�IDĿ�꣬need point
	bool play_enable;          //�Ƿ������ű��
	
	
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
