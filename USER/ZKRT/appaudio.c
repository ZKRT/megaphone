/**
  ******************************************************************************
  * @file    appaudio.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "appaudio.h"
#include "appprotocol.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
appaudio_st _audio_handlest;                     //��Ƶ��ؿ��ƴ���ṹ��
appaudio_st *audio_hdle_pst = &_audio_handlest;  
extern appfile_st _appfiles_handlest;                   //��Ƶ�ļ�������ؽṹ��
extern appfile_st *appfiles_hdle_pst;
extern audioinfolist_st _audioinfolist_st;              //��Ƶ��Ϣ�б�ṹ��
extern audioinfolist_st *_audioinfolist_pst;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void apprecord_handle(void);
static void appplay_handle(void);
///////////////////////////////////////////////////////////////////////////////process funciton
/**
  * @brief  appaudio_init
  * @param  
  * @note   
  * @retval
  */
void appaudio_init(void)
{
/************init global parameter**********************************/		
  //��Ƶ����	
  _audio_handlest.volume = VOLUME_MAP(100);
	_audio_handlest.play_mode = NORMAL_MAPY;
	//¼������
	_audio_handlest.rec_state = IDLE_S_REC;
	_audio_handlest.rec_id = AUDIOID_NONE;
	_audio_handlest.rec_enable = true;
	_audio_handlest.rec_item = NULL;
	//���Ų���
	_audio_handlest.play_state = IDLE_S_APY;
	_audio_handlest.play_id = AUDIOID_NONE;
	_audio_handlest.play_enable = true;
	_audio_handlest.play_item = NULL;
	
}
/**
  * @brief  appaudio_prcs
  * @param  
  * @note   
  * @retval
  */
void appaudio_prcs(void)
{
	apprecord_handle();
	appplay_handle();
}
/**
  * @brief  apprecord_handle
  * @param  
  * @note   
  * @retval
  */
static void apprecord_handle(void)
{
	if(audio_hdle_pst->rec_enable == false)
		return;
	
	switch(audio_hdle_pst->rec_state)
	{
		case IDLE_S_REC:
			break;
		case START_S_REC:
			break;
		case RECORDING_S_REC:
			break;
		case STOP_S_REC:
			break;
		case OVER_S_REC:
			break;
		default:break;
	}
}
/**
  * @brief  appplay_handle
  * @param  
  * @note   
  * @retval
  */
static void appplay_handle(void)
{
	if(audio_hdle_pst->play_enable == false)
		return;	
	
	switch(audio_hdle_pst->play_state)
	{
		case IDLE_S_APY:
			break;
		case START_S_APY:
			break;
		case PLAYING_S_APY:
			break;
		case PAUSE_S_APY:
			break;
		case CONTINUE_S_APY:
			break;
		case STOP_S_APY:
			break;
		case OVER_S_APY:
			break;		
		default:break;
	}
}

///////////////////////////////////////////////////////////////////////////////protocl funciton
static char none_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getlist_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char startrec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char stoprec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char playsong_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char playctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char volctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getaudioinfo_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char delrecaudio_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char modifyaudio_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);

const PTCOL_FUN ptcol_fun[] = {
	none_ptf,
	getlist_ptf,
	startrec_ptf,
	stoprec_ptf,
	playsong_ptf,
	playctrl_ptf,
	volctrl_ptf,
	getaudioinfo_ptf,
	delrecaudio_ptf,
	modifyaudio_ptf
}; 
/**
  * @brief  none_ptf
  * @param  
  * @note   
  * @retval
  */
static char none_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NOTNEED_RETRUN;
}
/**
  * @brief  getlist_ptf
  * @param  
  * @note   
  * @retval
  */
static char getlist_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	
	return NEED_RETRUN;
}
/**
  * @brief  startrec_ptf��ʼ¼��
  * @param  
  * @note   
  * @retval
  */
static char startrec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  stoprec_ptf ֹͣ��ǰ¼�����б���������־
  * @param  
  * @note   
  * @retval
  */
static char stoprec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  playsong_ptf ��ͣ��ǰ���ŵ��ļ��ٿ�ʼ�µĲ���
  * @param  
  * @note   
  * @retval
  */
static char playsong_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  playctrl_ptf ��ͣ/��������/ֹͣ
  * @param  
  * @note   
  * @retval
  */
static char playctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  volctrl_ptf ��������
  * @param  
  * @note   
  * @retval
  */
static char volctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  getaudioinfo_ptf
  * @param  
  * @note   
  * @retval
  */
static char getaudioinfo_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  delrecaudio_ptf 
  * @param  
  * @note   
  * @retval
  */
static char delrecaudio_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}
/**
  * @brief  modifyaudio_ptf
  * @param  
  * @note   
  * @retval
  */
static char modifyaudio_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NEED_RETRUN;
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
