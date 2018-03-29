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
#include "wavplay.h"
#include "wm8978.h"
#include "i2s.h"
#include "appaudio_handle.h"
#include "mp3play.h"
#include "audioplay.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
appaudio_st _audio_handlest;                     //音频相关控制处理结构体
appaudio_st *audio_hdle_pst = &_audio_handlest;  
extern audioplay_st _audioplay;
extern audiorec_st _audiorec;
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
	_audio_handlest.audiorec = &_audiorec;	
	_audio_handlest.audioplay = &_audioplay;

  //音频参数	
  _audio_handlest.volume = VOLUME_MAP(VOLUME_INIT_VALUE);

	//录音参数
	_audio_handlest.rec_enable = true;
	_audio_handlest.rec_id = AUDIOID_NONE;
	_audio_handlest.rec_item = NULL;	
	_audio_handlest.audiorec->rec_state = IDLE_S_REC;
	_audio_handlest.audiorec->rec_out_flag = REC_FLAG_OUTEN;

	//播放参数
	_audio_handlest.play_enable = true;
	_audio_handlest.play_id = AUDIOID_NONE;
	_audio_handlest.play_item = NULL;	
	_audio_handlest.audioplay->play_state = IDLE_S_APY;
	_audio_handlest.audioplay->play_mode = NORMAL_MAPY;
	_audio_handlest.audioplay->out_flag = REC_FLAG_OUTDISE;
	
	//set vol
	enter_volctrl_handle(VOLUME_INIT_VALUE);
	//进入speaker模式
	audio_enter_speak_mode();
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
	
	switch(audio_hdle_pst->audiorec->rec_state)
	{
		case IDLE_S_REC:
			break;
		
		case START_S_REC:
			recode_start_handle();
			break;
		
		case RECORDING_S_REC:
			recoding_handle();
			break;
		
		case PAUSE_S_REC:
			recode_pause_handle();
			break;
		case CONTINUE_S_REC:
			recode_continue_handle();
			break;
		
		case STOP_S_REC:
			recode_stop_handle();
			break;
		
		case OVER_S_REC:
			recode_over_handle();
		  audio_workmode_adjust(); //音频工作模式调整（录音，播放，对讲）
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
	
	switch(audio_hdle_pst->audioplay->play_state)
	{
		case IDLE_S_APY:
			break;
		
		case START_S_APY:
			audio_enter_play_mode(); //zkrt_notice
			if(audio_hdle_pst->play_item->format ==FORMAT_MP3)
			{
				mp3_play_start();
			}
			else
			{
				wav_play_start();
			}
			break;
			
		case PLAYING_S_APY:
			if(audio_hdle_pst->play_item->format ==FORMAT_MP3)
			{
				mp3_playing();
			}
			else
			{
				wav_playing();
			}			
			
			break;
		
		case PAUSE_S_APY:
			audiocommon_play_pause();
			break;
		
		case CONTINUE_S_APY:
			audiocommon_play_continue();
			break;
		
		case STOP_S_APY:
			audiocommon_play_stop();
			audio_hdle_pst->play_id = AUDIOID_NONE;
			audio_hdle_pst->play_item = NULL;
			break;
		
		case OVER_S_APY:
			audiocommon_play_over();
			audiocommon_play_over_check_mode(audio_hdle_pst);
			audio_workmode_adjust(); //音频工作模式调整（录音，播放，对讲）
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
static char recctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getaudiostate_ptf(void *sdata, void *rdata, u8 slen, u8* rlen);

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
	modifyaudio_ptf,
	recctrl_ptf,
	getaudiostate_ptf
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
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	rgetlist_plst *rother = (rgetlist_plst*)r->other_data;

	enther_getlist_handle(rother);
	
	r->control_num = s->control_num;
	r->status = S_Success;	
	*rlen = sizeof(rgetlist_plst) + RES_HEADER_LEN;
	return NEED_RETRUN;
}
/**
  * @brief  startrec_ptf开始录音
  * @param  
  * @note   
  * @retval
  */
static char startrec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	startRec_plst *sother = (startRec_plst*)(s->other_data);
	rstartRec_plst *rother = (rstartRec_plst*)r->other_data;
	
	res = enter_startrec_handle(sother->name, sother->flag, &rother->id);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rstartRec_plst) + RES_HEADER_LEN;
	
	return NEED_RETRUN;
}
/**
  * @brief  stoprec_ptf 停止当前录音，有保存或放弃标志
  * @param  
  * @note   
  * @retval
  */
static char stoprec_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	stopRec_plst *sother = (stopRec_plst*)(s->other_data);
	rstopRec_plst *rother = (rstopRec_plst*)r->other_data;
	
	res = enter_stoprec_handle(sother->flag, rother);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rstopRec_plst) + RES_HEADER_LEN;

	return NEED_RETRUN;
}
/**
  * @brief  playsong_ptf 停止当前播放的文件再开始新的播放
  * @param  
  * @note   
  * @retval
  */
static char playsong_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	playSong_plst *sother = (playSong_plst*)(s->other_data);
	rplaySong_plst *rother = (rplaySong_plst*)r->other_data;
	
	res = enter_playsong_handle(sother);
	
	rother->id = sother->id;
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rplaySong_plst)+RES_HEADER_LEN;
	return NEED_RETRUN;
}
/**
  * @brief  playctrl_ptf 暂停/继续播放/停止
  * @param  
  * @note   
  * @retval
  */
static char playctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	playCtrl_plst *sother = (playCtrl_plst*)(s->other_data);
	
	res = enter_playctrl_handle(sother->option);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
	
	return NEED_RETRUN;
}
/**
  * @brief  volctrl_ptf 音量控制
  * @param  
  * @note   
  * @retval
  */
static char volctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	volumeCtrl_plst *sother = (volumeCtrl_plst*)(s->other_data);
	
	res = enter_volctrl_handle(sother->vol);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
	
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
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	getAudioInfo_plst *sother = (getAudioInfo_plst*)(s->other_data);
	rgetAudioInfo_plst *rother = (rgetAudioInfo_plst*)r->other_data;
	
	res = enter_getaudioinfo_handle(sother->id, rother);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rgetAudioInfo_plst) + RES_HEADER_LEN;
	
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
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	delAudio_plst *sother = (delAudio_plst*)(s->other_data);
	rdelAudio_plst *rother = (rdelAudio_plst*)r->other_data;
	
	res = enter_delrecaudio_handle(sother->id);
	
	rother->id = sother->id;
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rdelAudio_plst)+RES_HEADER_LEN;
	
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
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	modifyAudio_plst *sother = (modifyAudio_plst*)(s->other_data);
	rmodifyAudio_plst *rother = (rmodifyAudio_plst*)r->other_data;
	
	res = enter_modifyaudio_handle(sother);
	
	rother->id = sother->id;
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rmodifyAudio_plst)+RES_HEADER_LEN;
	
	return NEED_RETRUN;
}
/**
  * @brief  recctrl_ptf 暂停/继续录音
  * @param  
  * @note   
  * @retval
  */
static char recctrl_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	recCtrl_plst *sother = (recCtrl_plst*)(s->other_data);
	
	res = enter_recctrl_handle(sother);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
	
	return NEED_RETRUN;
}
/**
  * @brief  getaudiostate_ptf
  * @param  
  * @note   
  * @retval
  */
static char getaudiostate_ptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res=S_Success;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	rgetAudioState_plst *rother = (rgetAudioState_plst*)r->other_data;
	
	res = enter_getaudiostate_handle(rother);
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rgetAudioState_plst) + RES_HEADER_LEN;
	
	return NEED_RETRUN;
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
