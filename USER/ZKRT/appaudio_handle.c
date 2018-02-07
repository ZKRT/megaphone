/**
  ******************************************************************************
  * @file    appaudio_handle.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   
audio的音频控制接口，对外接口
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2s.h"
#include "wm8978.h"
#include "wavplay.h"
#include "recorder.h"
#include "appaudio.h"
#include "appfiles.h"
#include "appaudio_handle.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern appaudio_st _audio_handlest;
extern appaudio_st *audio_hdle_pst;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void appaudio_play_clear(void);
static void appaudio_record_clear_notsave(void);
static void appaudio_record_clear_save(void);

///////////////////////////////////////////////////////////////////////////////handle funciton interface
/**
  * @brief  enther_getlist_handle
  * @param  
  * @note   
  * @retval
  */
void enther_getlist_handle(rgetlist_plst *rother)
{
	rother->recnum = audiolist_pst->rec_num;
	rother->musicnum = audiolist_pst->music_num;
	rother->audiolistmap = audiolist_pst->id_mask;
	printf("getlist_ptf\n");
	printf("recnum:%d, musicnum:%d,listmap:%d\n",rother->recnum,rother->musicnum,rother->audiolistmap);
}
/**
  * @brief  enter_startrec_handle
  * @param  
  * @note   
  * @retval
  */
char enter_startrec_handle(const unsigned char *name, u8 flag, u8 *rid)
{
	u8 res=S_Success;
	bool ret;
	short namelen;
	u8 id;
	u32 bw;
	recorderState *state = &audio_hdle_pst->audiorec->rec_state;
	char *objname = audio_hdle_pst->audiorec->rec_objname;
	__WaveHeader *Wavhead = &audio_hdle_pst->audiorec->wavhead;
	
//	if(audio_hdle_pst->audioplay->play_state == PLAYING_S_APY)
//		res = S_FailRecordAsPlaying;
	
	//判读参数是否正确，解析sother，封装rother
	namelen = strlen((char*)name);
  if((namelen>= AUDIO_NAME_LEN)||(namelen <=0))
		res = S_FailParamInvalid;
	
	if(!IS_REC_FLAG(flag))
		res = S_FailParamInvalid;
	
	if((*state != RECORDING_S_REC)&&(*state != START_S_REC)&&(*state != PAUSE_S_REC)&&(*state != CONTINUE_S_REC))  //判断录音文件是否重名，不允许覆盖之前的录音文件，允许覆盖当前的录音文件
	{
		if(audio_item_check_name_repeat((u8*)name)==true)
			res = S_FailRecNameRepeat;
	}
	
	if(res ==S_Success)
	{
		I2S_Rec_Stop(); 			//停止录音 //zkrt_notice
		appaudio_record_clear_notsave(); //clear record state
		ret = audio_item_add(name, &id);
		if(ret==false)
			res = S_FailRecExceedLimit;
		else
		{
			recoder_wav_init(Wavhead);				//初始化wav数据	
			audio_hdle_pst->audiorec->wavsize = 0; //初始化wav数据	
			snprintf(objname, sizeof(audio_hdle_pst->audiorec->rec_objname), "%s/%s", SD_DIR_NAME[RECORD_DIR], name);
	 		if(f_open(&_audio_handlest.audiorec->rec_fil, (const TCHAR*)objname, FA_CREATE_ALWAYS | FA_WRITE)!=FR_OK)
			{
				res = S_FailSDFlash;
				appaudio_record_clear_notsave(); //clear recording state
			}
			else
			{
				if(f_write(&_audio_handlest.audiorec->rec_fil, &Wavhead, sizeof(__WaveHeader), &bw)!=FR_OK)	//写入头数据
				{
					res = S_FailSDFlash;
					appaudio_record_clear_notsave(); //clear record state					
				}
				else
				{
					_audio_handlest.rec_item = audio_item_get(id);
					_audio_handlest.rec_id = id;
					_audio_handlest.audiorec->rec_state = START_S_REC;
					_audio_handlest.audiorec->rec_out_flag = flag;			
					printf("startrec_ptf\n");
					printf("id:%d, objname:%s\n",id, objname);
				}
			}
		}
	}
	*rid = id;  //respond id set
	return res;
}
/**
  * @brief  enter_stoprec_handle 停止当前录音，有保存或放弃标志
  * @param  
  * @note   
  * @retval
  */
char enter_stoprec_handle(u8 flag, rstopRec_plst *rother)
{
	u8 res=S_Success;
	u32 bw;
	recorderState *state = &audio_hdle_pst->audiorec->rec_state;
	__WaveHeader *Wavhead = &audio_hdle_pst->audiorec->wavhead;
	FIL *file = &audio_hdle_pst->audiorec->rec_fil;
	
	//判读参数是否正确，解析sother，封装rother
	if(!IS_REC_STOPFLAG(flag))
		res = S_FailParamInvalid;
	
	if(res ==S_Success)
	{
		if((*state ==RECORDING_S_REC)||(*state ==START_S_REC)||(*state ==PAUSE_S_REC))
		{
			printf("stoprec_ptf,flag:%d\n", flag);
			I2S_Rec_Stop(); 			//停止录音 //zkrt_notice
			
			if(flag == REC_STOP_NOSAVE)
			{
				appaudio_record_clear_notsave(); //clear recording state and info
			}
			else
			{
//				*state = STOP_S_REC; //zkrt_question: may delete this statement
				//计算wavhead size
				Wavhead->riff.ChunkSize = audio_hdle_pst->audiorec->wavsize+36;	//整个文件的大小-8;
        Wavhead->data.ChunkSize = audio_hdle_pst->audiorec->wavsize;			//数据大小
				//更新文件头
				f_lseek(file, 0);						//偏移到文件头.
				f_write(file, (const void*)Wavhead, sizeof(__WaveHeader), &bw);//写入头数据
				//填写剩余item信息	
				audio_hdle_pst->rec_item->size = Wavhead->data.ChunkSize;
				audio_hdle_pst->rec_item->time = (audio_hdle_pst->rec_item->size)/((audio_hdle_pst->rec_item->bitrate)/8);
				//respond info
				rother->id = audio_hdle_pst->rec_id;
				rother->size = audio_hdle_pst->rec_item->size;
				rother->time = audio_hdle_pst->rec_item->time;
			  //清除所有录音状态
				appaudio_record_clear_save();
				
				printf("id:%d, size:%d, time%d\n", rother->id, rother->size, rother->time);
			}
		}
		else
		{
			printf("stop fail as not recording\n");
			res = S_FailNoValidRec;
		}
	}

	return res;
}
/**
  * @brief  enter_playsong_handle 停止当前播放的文件再开始新的播放
  * @param  
  * @note   
  * @retval
  */
char enter_playsong_handle(const playSong_plst *sother)
{
	u8 res=S_Success;
	char *objname = audio_hdle_pst->audioplay->play_objname;
	audioinfo_st *item;
	
//	if(audio_hdle_pst->audiorec->rec_state == RECORDING_S_REC)
//		res = S_FailPlayAsRecording;
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(sother->id)==false)
		res = S_FailIdInvalid;
	
	if(!IS_REC_FLAG(sother->flag))
		res = S_FailParamInvalid;
	
	if(res ==S_Success)
	{
		item = audio_item_get(sother->id);

		if(item->attr ==ATTR_MUSIC)
			snprintf(objname, sizeof(audio_hdle_pst->audioplay->play_objname), "%s/%s", SD_DIR_NAME[MUSIC_DIR], item->name);  //获取文件名的绝对路径
		else
			snprintf(objname, sizeof(audio_hdle_pst->audioplay->play_objname), "%s/%s", SD_DIR_NAME[RECORD_DIR], item->name);  //获取文件名的绝对路径
		
		memset(&audio_hdle_pst->audioplay->wavctrl, 0x00, sizeof(__wavctrl));
		res = wav_decode_init((u8*)objname, &audio_hdle_pst->audioplay->wavctrl); //判断文件格式是否有效
		if(!res)
		{
			printf("playsong_ptf, out_flag=%d, file:%s\n", sother->flag, objname);
			appaudio_play_clear(); //clear last play song
			_audio_handlest.play_id = sother->id;
		  _audio_handlest.play_item = item;
		  _audio_handlest.audioplay->play_state = START_S_APY;
			_audio_handlest.audioplay->out_flag = sother->flag;
			res = S_Success;
		}
		else
		{
			printf("play audio fail, wav files invalid.\n");
			res = S_FailInvalidAudioFiles;
		}
	}
	
	return res;
}
/**
  * @brief  enter_playctrl_handle 暂停/继续播放/停止
  * @param  
  * @note   
  * @retval
  */
char enter_playctrl_handle(u8 option)
{
	u8 res=S_Success;

	//判读参数是否正确，解析sother，封装rother
	if(!IS_PLAYOPTION(option))
		res = S_FailParamInvalid;
	
	if(res ==S_Success)
	{
		if(audio_hdle_pst->play_id == AUDIOID_NONE)  //当前无播放ID
		{
			res = S_FailNoPlaySong;
		}
    else
    {
			if(option ==PLAY_CTRL_PAUSE)
				_audio_handlest.audioplay->play_state = PAUSE_S_APY;
			if(option ==PLAY_CTRL_CONTINUE)
				_audio_handlest.audioplay->play_state = CONTINUE_S_APY;
			if(option ==PLAY_CTRL_STOP)
				_audio_handlest.audioplay->play_state = STOP_S_APY;	
      printf("playctrl_ptf, option:%d,state:%d\n", option, _audio_handlest.audioplay->play_state);	
		}
	}
	
	return res;
}
/**
  * @brief  enter_volctrl_handle 音量控制
  * @param  
  * @note   
  * @retval
  */
char enter_volctrl_handle(u8 vol)
{
	u8 res=S_Success;
	//判读参数是否正确，解析sother，封装rother
	if(!IS_VOLUME((int)vol))
		res = S_FailVolumeExceed;
	
	if(res ==S_Success)
	{
		_audio_handlest.volume = vol;
		WM8978_HPvol_Set(VOLUME_MAP(vol), VOLUME_MAP(vol));	//耳机音量设置
		WM8978_SPKvol_Set(VOLUME_MAP(vol));		              //喇叭音量设置	
		printf("volctrl_ptf, vol:%d\n", vol);
	}

	return res;
}
/**
  * @brief  enter_getaudioinfo_handle
  * @param  
  * @note   
  * @retval
  */
char enter_getaudioinfo_handle(u8 id, rgetAudioInfo_plst *rother)
{
	u8 res=S_Success;

	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(id)==false)
		res = S_FailIdInvalid;
	
	if(res ==S_Success)
	{
		printf("getaudioinfo_ptf\n");
		*rother = *(rgetAudioInfo_plst*)(audio_item_get(id));
		printf_audio_item((audioinfo_st *)rother);   //zkrt_printf
	}

	return res;
}
/**
  * @brief  enter_delrecaudio_handle 
  * @param  
  * @note   
  * @retval
  */
char enter_delrecaudio_handle(u8 id)
{
	u8 res=S_Success;
	audioinfo_st* item = audio_item_get(id);
	char name[DIR_NAME_MAXLEN+AUDIO_NAME_LEN+2];
	
	//判读参数是否正确，解析sother，封装rother
	if(audio_item_exist(id)==false)
		res = S_FailIdInvalid;
	
	if(res ==S_Success)
	{
		snprintf((void*)name, sizeof(name), "%s/%s", SD_DIR_NAME[RECORD_DIR], item->name);  //获取文件名的绝对路径
		if(audio_item_del(id)==true)  //del item
		{
			//del file
			f_unlink((TCHAR*)name); //delete file
			printf("delrecaudio_ptf, id:%d, file:%s\n", id, name);
		}
		else
		{
			res = S_FailIdInvalid;
		}
	}

	return res;
}
/**
  * @brief  enter_modifyaudio_handle
  * @param  
  * @note   
  * @retval
  */
char enter_modifyaudio_handle(const modifyAudio_plst *sother)
{
	u8 res=S_Success;
	short namelen;
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

	return res;
}
/**
  * @brief  enter_recctrl_handle 暂停/继续
  * @param  
  * @note   
  * @retval
  */
char enter_recctrl_handle(const recCtrl_plst *sother)
{
	u8 res=S_Success;

	//判读参数是否正确，解析sother，封装rother
	if(!IS_RECOPTION(sother->option))
		res = S_FailParamInvalid;
	
	if(res ==S_Success)
	{
		if(audio_hdle_pst->rec_id == AUDIOID_NONE)
		{
			res = S_FailRecCtrlAsNoRec;
		}
    else
    {
			if(sother->option ==REC_CTRL_PAUSE)
			{
				_audio_handlest.audiorec->rec_state = PAUSE_S_REC;
				I2S_Rec_Stop(); 
			}
			if(sother->option ==REC_CTRL_CONTINUE)
			{
				_audio_handlest.audiorec->rec_state = CONTINUE_S_REC;
				I2S_Rec_Start(); 		
			}
      printf("recctrl_ptf, option:%d, state:%d\n", sother->option, _audio_handlest.audiorec->rec_state);
		}
	}
	
	return res;
}
/**
  * @brief  
  * @param  
  * @note   
  * @retval
  */
char enter_getaudiostate_handle(rgetAudioState_plst *rother)
{
	u8 res=S_Success;
	
	rother->play_id = audio_hdle_pst->play_id;
	rother->rec_id = audio_hdle_pst->rec_id;
	rother->play_state = audio_hdle_pst->audioplay->play_state;
	rother->rec_state = audio_hdle_pst->audiorec->rec_state;

	printf("getaudiostate_ptf, playstate:%d, playid:%d, recstate:%d, recid:%d\n", rother->play_state, rother->play_id, rother->rec_state, rother->rec_id);

	return res;
}
/**
  * @brief  
  * @param  
  * @note   
  * @retval
  */
char enter_play_next_song(void)
{
	u8 res=S_Success;
	playSong_plst next_item={0, REC_FLAG_OUTEN};
	next_item.id = audio_item_nextid_loop(_audio_handlest.play_id);
	if(next_item.id !=AUDIOID_NONE)
	{
		enter_playsong_handle(&next_item);
		return res;
	}
	res = S_Fail;
	return res;
}
/////////////////////////////////////////////////////////////////////////////// record and play state clear
/**
  * @brief  appaudio_play_clear
  * @param  
  * @note   
  * @retval
  */
static void appaudio_play_clear(void)
{
	u8 state = audio_hdle_pst->audioplay->play_state;
	if(state ==PLAYING_S_APY)  //播放中，清除必要的操作
	{
		f_close(&audio_hdle_pst->audioplay->play_fil);
	}
	audio_hdle_pst->play_id = AUDIOID_NONE;
	audio_hdle_pst->play_item = NULL;
	audio_hdle_pst->audioplay->play_state = IDLE_S_APY;
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
	u8 state = audio_hdle_pst->audiorec->rec_state;
	
	if((state ==START_S_REC)||(state == RECORDING_S_REC))  //录音中，清除当前录音参数
	{
		f_close(&audio_hdle_pst->audiorec->rec_fil);
		f_unlink((TCHAR*)audio_hdle_pst->audiorec->rec_objname); //delete file
		audio_item_del(audio_hdle_pst->rec_id);  //delete item
	}
	audio_hdle_pst->audiorec->rec_state = STOP_S_REC;
	audio_hdle_pst->rec_id = AUDIOID_NONE;
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
	u8 state = audio_hdle_pst->audiorec->rec_state;
	if((state ==START_S_REC)||(state == RECORDING_S_REC))  //录音中，清除当前录音参数
	{
		f_close(&audio_hdle_pst->audiorec->rec_fil);
	}
	audio_hdle_pst->audiorec->rec_state = STOP_S_REC;
	audio_hdle_pst->rec_id = AUDIOID_NONE;
	audio_hdle_pst->rec_item = NULL;
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
