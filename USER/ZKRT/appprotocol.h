/**
  ******************************************************************************
  * @file    appprotocol.h 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   
	*					 + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPPROTOCOL_H
#define __APPPROTOCOL_H 
/* Includes ------------------------------------------------------------------*/
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup COMMAND1 
  * @{
  */ 
//id range
#define IS_AUDIOID(ID)                ((ID) >= 0 && (ID) <= 31)
#define AUDIOID_NONE                  0xff     
#define ID_LIST_MAX                   32

//start record param
#define REC_FLAG_OUTEN                1               
#define REC_FLAG_OUTDISE              2
#define IS_REC_FLAG(FLAG)             (((FLAG) == REC_FLAG_OUTEN) || \
                                       ((FLAG) == REC_FLAG_OUTDISE))

//stop record param
#define REC_STOP_SAVE                 1               
#define REC_STOP_NOSAVE               2
#define IS_REC_STOPFLAG(FLAG)         (((FLAG) == REC_STOP_SAVE) || \
                                       ((FLAG) == REC_STOP_NOSAVE)) 

//play control param
#define PLAY_CTRL_PAUSE               1               
#define PLAY_CTRL_CONTINUE            2
#define PLAY_CTRL_STOP                3
#define IS_PLAYOPTION(OPTION)         (((OPTION) == PLAY_CTRL_PAUSE)    || \
                                       ((OPTION) == PLAY_CTRL_CONTINUE) || \
                                       ((OPTION) == PLAY_CTRL_STOP))

//audio info param
#define ATTR_MUSIC                    1
#define ATTR_RECORD                   2
#define FORMAT_WAV                    1
#define FORMAT_MP3                    2

//volume control param
#define IS_VOLUME(VOL)                (((VOL) >= 0) && ((VOL) <= 100))

//record control param
#define REC_CTRL_PAUSE               1               
#define REC_CTRL_CONTINUE            2
#define IS_RECOPTION(OPTION)         (((OPTION) == REC_CTRL_PAUSE)    || \
                                       ((OPTION) == REC_CTRL_CONTINUE))

//audio name length
#define AUDIO_NAME_LEN                20

//index 
#define CTRLNUM_INDEX                 0   //start from data[0]
#define RESPOND_STATUS_INDEX          1   //respond index


/**
  * @}
  */ 
	
/** @defgroup COMMAND2
  * @{
  */ 	
//type num
#define TN_HEARTBEAT                  1
//hb flag
#define TNHB_FLAG                     0xaabbccdd
#define HB_LENGTH                     5

//respond flag
#define NEED_RETRUN                   1
#define NOTNEED_RETRUN                0

/* Exported types ------------------------------------------------------------*/
/** @defgroup COMMAND1 
  * @{
  */ 
//control num enum
typedef enum
{ 
	CN_None =0,         
	CN_GetList,           
	CN_StartRecord,      
	CN_StopRecord,           
	CN_PlaySong,
  CN_PlayCtrl,
  CN_VolumeCtrl,
  CN_GetFileInfo,
  CN_DelRecFile,
  CN_ModifyFilePram,
	CN_RecCtrl,
	CN_GetAudioStae,
	CN_MAX
}ControlNumEnum;

//respond status enum
typedef enum
{ 
	S_Success,
	S_Fail,          
	S_FailRecExceedLimit,       
	S_FailNoValidRec,           
	S_FailIdInvalid,
  S_FailNoPlaySong,
	S_FailVolumeExceed,
	S_FailParamInvalid,
	S_FailSDFlash,
	S_FailRecNameRepeat,
	S_FailInvalidAudioFiles,
	S_FailRecCtrlAsNoRec
//	S_FailPlayAsRecording,
//	S_FailRecordAsPlaying
}ResStatusEnum;
/**
  * @}
  */ 
///////////////////////////////send and respond struct

//////send
#pragma pack(1)
typedef struct
{
	u8 control_num;
	u8 other_data[ZK_DATA_MAX_LEN-1];
}send_plst;

//////respond
#define RES_HEADER_LEN  2  //equal to control num+status
typedef struct
{
	u8 control_num;
	u8 status;
	u8 other_data[ZK_DATA_MAX_LEN-2];
}respond_plst;

////send sub 
typedef struct
{
	u8 id;
}id_plst;
//get list -none
//start record
typedef struct
{
	u8 name[AUDIO_NAME_LEN];
	u8 flag;
}startRec_plst;
//stop record
typedef struct
{
	u8 flag;
}stopRec_plst;
//paly song
typedef struct
{
	u8 id;
	u8 flag;
}playSong_plst;
//play control
typedef struct
{
	u8 option;
}playCtrl_plst;
//volume control
typedef struct
{
	u8 vol;
}volumeCtrl_plst;
//check audio info
//typedef struct
//{
//	u8 id;
//}getAudioInfo_plst;
typedef id_plst getAudioInfo_plst;
//del record audio
//typedef struct
//{
//	u8 id;
//}delAudio_plst;
typedef id_plst delAudio_plst;
//modify audio
typedef struct
{
	u8 id;
	u8 name[AUDIO_NAME_LEN];
}modifyAudio_plst;
//rec control
typedef struct
{
	u8 option;
}recCtrl_plst;
//////respond
//get list
typedef struct
{
	u8 recnum;
	u8 musicnum;
	u32 audiolistmap;
}rgetlist_plst;
//start record
//typedef struct
//{
//	u8 id;
//}rstartRec_plst;
typedef id_plst rstartRec_plst;
//stop record
typedef struct
{
	u8 id;
	u32 time;
	u32 size;
}rstopRec_plst;
//paly song -none
//play control -none
//volume control -none
//check audio info
typedef struct
{
	u8 id;
	u8 name[AUDIO_NAME_LEN];
	u32 time;
	u32 size;
	u8 attr;
	u8 format;
	u32 samplerate;
	u32 bitrate;
	u16 bps;
}rgetAudioInfo_plst;
typedef struct
{
	char play_state;
	u8 play_id;
	char rec_state;
	u8 rec_id;
}rgetAudioState_plst;
//del record audio -none
//modify audio -none

#pragma pack() 


/**
*   @brief  protocl handle funciton
  * @param  void* source data
  * @param  void* respond data
  * @param  u8 source data length
  * @param  u8* respond data length
  * @retval NEED_RETURN: need send respond msg, NOTNEED_RETRUN: no need send respond msg
  */
typedef char (*PTCOL_FUN)(void*, void*, u8, u8*);

/* Exported functions ------------------------------------------------------- */
extern const PTCOL_FUN ptcol_fun[];

#endif /* __APPCAN_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

