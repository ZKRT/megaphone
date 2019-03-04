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
#define DEV_MODEL              "M-SP2"
#define DEV_HW                 "010203"
#define DEV_SW                 "000304"

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
#define AUDIO_NAME_LEN                40

//index
#define CTRLNUM_INDEX                 0   //start from data[0]
#define RESPOND_STATUS_INDEX          1   //respond index


/**
  * @}
  */

/** @defgroup COMMAND2
  * @{
  */
//type num enum
typedef enum {
	TN_None = 0,
	TN_HEARTBEAT,
	TN_GETDEVINFO,
	TN_MAX
} TypeNumEnum;
//hb flag
#define TNHB_FLAG                     0xaabbccdd
#define THHB_FIXED_LEN                5

//respond flag
#define NEED_RETRUN                   1
#define NOTNEED_RETRUN                0

/* Exported types ------------------------------------------------------------*/
/** @defgroup COMMAND1
  * @{
  */
//control num enum
typedef enum {
	CN_None = 0,
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
} ControlNumEnum;

//respond status enum
typedef enum {
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
} ResStatusEnum;
/**
  * @}
  */
//////////////////////////////////////////////////////////////////////////protocol start
#pragma pack(1)
///////////////////////////////command=common, heartbeat data
//////common command
typedef struct {
	u8 type_num;
	u8 type_data[ZK_DATA_MAX_LEN - 1];
} common_data_plst;
//heartbeat data
typedef struct {
	u32 hb_flag;
	u8 hb_data[ZK_DATA_MAX_LEN - THHB_FIXED_LEN];
} common_hbd_plst;
//dev info data
typedef struct {
	u8 status;
	u8 model[10];
	u8 hw_version[6];
	u8 sw_version[6];
} common_get_devinfo_plst;
///////////////////////////////command=specify, send and respond struct
//////send

typedef struct {
	u8 control_num;
	u8 other_data[ZK_DATA_MAX_LEN - 1];
} send_plst;

//////
#define RES_HEADER_LEN  2  //equal to control num+status
typedef struct {
	u8 control_num;
	u8 status;
	u8 other_data[ZK_DATA_MAX_LEN - 2];
} respond_plst;

////send sub
typedef struct {
	u8 id;
} id_plst;
//get list -none
//start record
typedef struct {
	u8 name[AUDIO_NAME_LEN];
	u8 flag;
} startRec_plst;
//stop record
typedef struct {
	u8 flag;
} stopRec_plst;
//paly song
typedef struct {
	u8 id;
	u8 flag;
} playSong_plst;
//play control
typedef struct {
	u8 option;
} playCtrl_plst;
//volume control
typedef struct {
	u8 vol;
} volumeCtrl_plst;
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
typedef struct {
	u8 id;
	u8 name[AUDIO_NAME_LEN];
} modifyAudio_plst;
//rec control
typedef struct {
	u8 option;
} recCtrl_plst;
//////respond
//get list
typedef struct {
	u8 recnum;
	u8 musicnum;
	u32 audiolistmap;
} rgetlist_plst;
//start record
//typedef struct
//{
//	u8 id;
//}rstartRec_plst;
typedef id_plst rstartRec_plst;
//stop record
typedef struct {
	u8 id;
	u32 time;
	u32 size;
} rstopRec_plst;
typedef id_plst rplaySong_plst;
//play control -none
//volume control -none
//check audio info
typedef struct {
	u8 id;
	u8 name[AUDIO_NAME_LEN];
	u32 time;
	u32 size;
	u8 attr;
	u8 format;
	u32 samplerate;
	u32 bitrate;
	u16 bps;
	u8 nchannels;
} rgetAudioInfo_plst;
typedef struct {
	char play_state;
	u8 play_id;
	char rec_state;
	u8 rec_id;
	u8 vol;
} rgetAudioState_plst;
typedef id_plst rdelAudio_plst;
typedef id_plst rmodifyAudio_plst;
///////////////////////////////////////////////////////////////////////////////////////protocol end
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

