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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
appaudio_st _audio_handlest;                     //音频相关控制处理结构体
appaudio_st *audio_hdle_pst = &_audio_handlest;  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void apprecord_handle(void);
static void appplay_handle(void);
static void appaudio_play_clear(void);
static void appaudio_record_clear_notsave(void);
static void appaudio_record_clear_save(void);
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
  //音频参数	
  _audio_handlest.volume = VOLUME_MAP(100);
	_audio_handlest.play_mode = NORMAL_MAPY;
	//录音参数
	_audio_handlest.rec_state = IDLE_S_REC;
	_audio_handlest.rec_id = AUDIOID_NONE;
	_audio_handlest.rec_enable = true;
	_audio_handlest.rec_item = NULL;
	//播放参数
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
			recoder_enter_rec_mode();
		  audio_hdle_pst->rec_state = RECORDING_S_REC;
			break;
		
		case RECORDING_S_REC:
			break;
		
		case STOP_S_REC:
			audio_hdle_pst->rec_state = OVER_S_REC;
			break;
		
		case OVER_S_REC:
			audio_hdle_pst->rec_state = IDLE_S_REC;
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
			recoder_enter_play_mode();
      if(wav_play_start(audio_hdle_pst) == true)
			{
				audio_hdle_pst->play_state = PLAYING_S_APY;
			}
			break;
			
		case PLAYING_S_APY:
			audio_hdle_pst->play_state = (audioPlayState)wav_playing(audio_hdle_pst);
			break;
		
		case PAUSE_S_APY:
			wav_play_pause(audio_hdle_pst);
			break;
		
		case CONTINUE_S_APY:
			wav_play_continue(audio_hdle_pst);
			audio_hdle_pst->play_state = PLAYING_S_APY;
			break;
		
		case STOP_S_APY:
			wav_play_stop(audio_hdle_pst);
			break;
		
		case OVER_S_APY:
			wav_play_over(audio_hdle_pst);
			break;		
		
		default:break;
	}
}
/**
  * @brief  appaudio_play_clear
  * @param  
  * @note   
  * @retval
  */
static void appaudio_play_clear(void)
{
	u8 state = audio_hdle_pst->play_state;
	if(state ==PLAYING_S_APY)  //播放中，清除必要的操作
	{
		f_close(&audio_hdle_pst->play_fil);
	}
	audio_hdle_pst->play_id = AUDIOID_NONE;
	audio_hdle_pst->play_state = IDLE_S_APY;
	audio_hdle_pst->play_item = NULL;
}
/**
  * @brief  appaudio_record_clear_notsave
清除当前录音状态和信息
  * @param  
  * @note   
  * @retval
  */
static void appaudio_record_clear_notsave(void)
{
	u8 state = audio_hdle_pst->rec_state;
	
	if((state ==START_S_REC)||(state == RECORDING_S_REC))  //录音中，清除当前录音参数
	{
		f_close(&audio_hdle_pst->rec_fil);
		f_unlink((TCHAR*)audio_hdle_pst->rec_objname); //delete file
		audio_item_del(audio_hdle_pst->rec_id);  //delete item
	}
	audio_hdle_pst->rec_id = AUDIOID_NONE;
	audio_hdle_pst->rec_state = IDLE_S_REC;
	audio_hdle_pst->rec_item = NULL;
}
/**
  * @brief  appaudio_record_clear_save
appaudio_record_clear_notsave与之差别是：不删除文件和item
  * @param  
  * @note   
  * @retval
  */
static void appaudio_record_clear_save(void)
{
	u8 state = audio_hdle_pst->rec_state;
	if((state ==START_S_REC)||(state == RECORDING_S_REC))  //录音中，清除当前录音参数
	{
		f_close(&audio_hdle_pst->rec_fil);
	}
	audio_hdle_pst->rec_id = AUDIOID_NONE;
	audio_hdle_pst->rec_state = IDLE_S_REC;
	audio_hdle_pst->rec_item = NULL;
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
	send_plst *s = (send_plst*)sdata;
//	startRec_plst *sother = (id_plst*)(s->other_data);
	respond_plst *r = (respond_plst*)rdata;
	rgetlist_plst *rother = (rgetlist_plst*)r->other_data;

	rother->recnum = audiolist_pst->rec_num;
	rother->musicnum = audiolist_pst->music_num;
	rother->audiolistmap = audiolist_pst->id_mask;
	r->control_num = s->control_num;
	r->status = S_Success;	
	*rlen = sizeof(rgetlist_plst) + RES_HEADER_LEN;
	printf("getlist_ptf\n");
	printf("recnum:%d, musicnum:%d,listmap:%d\n",rother->recnum,rother->musicnum,rother->audiolistmap);
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
	bool ret;
	short namelen;
	u8 id;
	u32 bw;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	startRec_plst *sother = (startRec_plst*)(s->other_data);
	rstartRec_plst *rother = (rstartRec_plst*)r->other_data;
	
	//判读参数是否正确，解析sother，封装rother
	namelen = strlen((char*)sother->name);
  if((namelen>= AUDIO_NAME_LEN)||(namelen <=0))
		res = S_FailParamInvalid;
	if(!IS_REC_FLAG(sother->flag))
		res = S_FailParamInvalid;
	
	if((_audio_handlest.rec_state != RECORDING_S_REC)&&(_audio_handlest.rec_state != START_S_REC))  //判断录音文件是否重名，不允许覆盖之前的录音文件，允许覆盖当前的录音文件
	{
		if(audio_item_check_name_repeat(sother->name)==true)
			res = S_FailRecNameRepeat;
	}
	
	if(res ==S_Success)
	{
		I2S_Rec_Stop(); 			//停止录音 //zkrt_debug
		appaudio_record_clear_notsave(); //clear record state
		ret = audio_item_add(sother->name, &id);
		if(ret==false)
			res = S_FailRecExceedLimit;
		else
		{
			rother->id = id;  //respond id set
			
			printf("id:%d\n",id);
			printf("name:%s\n",sother->name);
			snprintf(_audio_handlest.rec_objname, sizeof(_audio_handlest.rec_objname), "%s/%s", SD_DIR_NAME[RECORD_DIR], sother->name);  //获取文件名的绝对路径			
		  printf("objname:%s\n",_audio_handlest.rec_objname);
			recoder_wav_init(&_audio_handlest.waviis.wavhead);				//初始化wav数据	
			audio_hdle_pst->waviis.wavsize = 0;
	 		if(f_open(&_audio_handlest.rec_fil, (const TCHAR*)_audio_handlest.rec_objname, FA_CREATE_ALWAYS | FA_WRITE)!=FR_OK)
			{
				res = S_FailSDFlash;
				appaudio_record_clear_notsave(); //clear recording state
			}
			else
			{
				if(f_write(&_audio_handlest.rec_fil, &_audio_handlest.waviis.wavhead, sizeof(__WaveHeader), &bw)!=FR_OK)	//写入头数据
				{
					res = S_FailSDFlash;
					appaudio_record_clear_notsave(); //clear record state					
				}
				else
				{
					_audio_handlest.rec_item = audio_item_get(id);
					_audio_handlest.rec_id = id;
					_audio_handlest.rec_state = START_S_REC;
					_audio_handlest.waviis.rec_out_flag = sother->flag;			
					printf("startrec_ptf\n");
				}
			}
		}
	}
	
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
	u32 bw;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	stopRec_plst *sother = (stopRec_plst*)(s->other_data);
	rstopRec_plst *rother = (rstopRec_plst*)r->other_data;
	
	//判读参数是否正确，解析sother，封装rother
	if(!IS_REC_STOPFLAG(sother->flag))
		res = S_FailParamInvalid;
	
	if(res ==S_Success)
	{
		if((audio_hdle_pst->rec_state ==RECORDING_S_REC)||(audio_hdle_pst->rec_state ==START_S_REC))
		{
			I2S_Rec_Stop(); 			//停止录音 //zkrt_debug
			printf("stoprec_ptf,flag:%d\n", sother->flag);
			if(sother->flag == REC_STOP_NOSAVE)
			{
				appaudio_record_clear_notsave(); //clear recording state and info
			}
			else
			{
				//关闭录音
				audio_hdle_pst->rec_state = STOP_S_REC;
				//计算wavhead size
				audio_hdle_pst->waviis.wavhead.riff.ChunkSize = audio_hdle_pst->waviis.wavsize+36;	//整个文件的大小-8;
				audio_hdle_pst->waviis.wavhead.data.ChunkSize = audio_hdle_pst->waviis.wavsize;			//数据大小
				//保存文件
				f_lseek(&audio_hdle_pst->rec_fil, 0);						//偏移到文件头.
				f_write(&audio_hdle_pst->rec_fil, (const void*)&audio_hdle_pst->waviis.wavhead, sizeof(__WaveHeader), &bw);//写入头数据
				f_close(&audio_hdle_pst->rec_fil);
				//填写剩余item信息	
//				audio_hdle_pst->rec_item->bitrate = audio_hdle_pst->waviis.wavhead.fmt.ByteRate;  //default
//				audio_hdle_pst->rec_item->bps = audio_hdle_pst->waviis.wavhead.fmt.BitsPerSample;
//				audio_hdle_pst->rec_item->samplerate = audio_hdle_pst->waviis.wavhead.fmt.SampleRate;
				audio_hdle_pst->rec_item->size = audio_hdle_pst->waviis.wavhead.data.ChunkSize;
				audio_hdle_pst->rec_item->time = (audio_hdle_pst->rec_item->size)/((audio_hdle_pst->rec_item->bitrate)/8);
				//respond info
				rother->id = audio_hdle_pst->rec_id;
				rother->size = audio_hdle_pst->rec_item->size;
				rother->time = audio_hdle_pst->rec_item->time;
				//清除所有录音状态
				audio_hdle_pst->rec_id = AUDIOID_NONE;
				audio_hdle_pst->rec_item = NULL;
				printf("id:%d, size:%d, time%d\n", rother->id, rother->size, rother->time);
			}
		}
		else
		{
			res = S_FailNoValidRec;
		}
	}
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
	audioinfo_st *item;
	u8 id;
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(sother->id)==false)
		res = S_FailIdInvalid;
	if(res ==S_Success)
	{
		
		id = sother->id;
		item = audio_item_get(sother->id);

		if(item->attr ==ATTR_MUSIC)
			snprintf(audio_hdle_pst->play_objname, sizeof(audio_hdle_pst->play_objname), "%s/%s", SD_DIR_NAME[MUSIC_DIR], item->name);  //获取文件名的绝对路径
		else
			snprintf(audio_hdle_pst->play_objname, sizeof(audio_hdle_pst->play_objname), "%s/%s", SD_DIR_NAME[RECORD_DIR], item->name);  //获取文件名的绝对路径
		
		res = wav_decode_init((u8*)audio_hdle_pst->play_objname, &audio_hdle_pst->wavctrl); //判断文件格式是否有效
		if(!res)
		{
			//clear last play song
			printf("playsong_ptf, file:%s\n", audio_hdle_pst->play_objname);
			appaudio_play_clear();
			_audio_handlest.play_id = id;
		  _audio_handlest.play_item = item;
		  _audio_handlest.play_state = START_S_APY;
			res = S_Success;
		}
		else
		{
			printf("play audio fail, wav files invalid.\n");
			res = S_Fail;
		}
	}
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
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
	
	//判读参数是否正确，解析sother，封装rother
	if(!IS_PLAYOPTION(sother->option))
		res = S_FailParamInvalid;
	if(res ==S_Success)
	{
		if(audio_hdle_pst->play_id == AUDIOID_NONE)  //当前无播放ID
		{
			res = S_FailNoPlaySong;
		}
    else
    {
			if(sother->option ==PLAY_CTRL_PAUSE)
				_audio_handlest.play_state = PAUSE_S_APY;
			if(sother->option ==PLAY_CTRL_CONTINUE)
				_audio_handlest.play_state = CONTINUE_S_APY;
			if(sother->option ==PLAY_CTRL_STOP)
				_audio_handlest.play_state = STOP_S_APY;			
      printf("playctrl_ptf, option:%d,state:%d\n", sother->option, _audio_handlest.play_state);	
		}
	}
	
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
	
	//判读参数是否正确，解析sother，封装rother
	if(!IS_VOLUME((int)sother->vol))
		res = S_FailVolumeExceed;
	if(res ==S_Success)
	{
		_audio_handlest.volume = sother->vol;
		WM8978_HPvol_Set(VOLUME_MAP(sother->vol), VOLUME_MAP(sother->vol));	//耳机音量设置
		WM8978_SPKvol_Set(VOLUME_MAP(sother->vol));		                      //喇叭音量设置	
		printf("volctrl_ptf, vol:%d\n", sother->vol);
	}
	
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
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(sother->id)==false)
		res = S_FailIdInvalid;
	if(res ==S_Success)
	{
		printf("getaudioinfo_ptf\n");
		*rother = *(rgetAudioInfo_plst*)(audio_item_get(sother->id));
		printf_audio_item((audioinfo_st *)rother); //zkrt_printf
	}
	
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
	audioinfo_st* item = audio_item_get(sother->id);
	char name[DIR_NAME_MAXLEN+AUDIO_NAME_LEN+2];
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(sother->id)==false)
		res = S_FailIdInvalid;
	if(res ==S_Success)
	{
		snprintf((void*)name, sizeof(name), "%s/%s", SD_DIR_NAME[RECORD_DIR], item->name);  //获取文件名的绝对路径
		if(audio_item_del(sother->id)==true)  //del item
		{
			//del file
			f_unlink((TCHAR*)name); //delete file
			printf("delrecaudio_ptf, id:%d, file:%s\n", sother->id, name);
		}
		else
		{
			res = S_FailIdInvalid;
		}
	}
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
	
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
	short namelen;
	send_plst *s = (send_plst*)sdata;
	respond_plst *r = (respond_plst*)rdata;
	modifyAudio_plst *sother = (modifyAudio_plst*)(s->other_data);
	char nameold[DIR_NAME_MAXLEN+AUDIO_NAME_LEN+2];
	char namenew[DIR_NAME_MAXLEN+AUDIO_NAME_LEN+2];
	audioinfo_st* item;
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(sother->id)==false)
		res = S_FailIdInvalid;
	namelen = strlen((char*)sother->name);
  if((namelen>= AUDIO_NAME_LEN)||(namelen <=0))
		res = S_FailParamInvalid;	
	if(res ==S_Success)
	{
		item = audio_item_get(sother->id);
		if(item->attr ==ATTR_MUSIC)
		{
			snprintf((void*)nameold, sizeof(nameold), "%s/%s", SD_DIR_NAME[MUSIC_DIR], item->name);  
			snprintf((void*)namenew, sizeof(namenew), "%s/%s", SD_DIR_NAME[MUSIC_DIR], sother->name);
		}
		else
		{
			snprintf((void*)nameold, sizeof(nameold), "%s/%s", SD_DIR_NAME[RECORD_DIR], item->name);  
			snprintf((void*)namenew, sizeof(namenew), "%s/%s", SD_DIR_NAME[RECORD_DIR], sother->name);			
		}
		//modify name in file
		f_rename(nameold, namenew);	/* Rename/Move a file or directory */
		//modify name in item
		strcpy((char*)item->name, (char*)sother->name);
		printf("modifyaudio_ptf, oldname:%s, newname:%s\n", nameold, namenew);
	}
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = RES_HEADER_LEN;
	
	return NEED_RETRUN;
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
