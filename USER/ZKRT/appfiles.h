/**
  ******************************************************************************
  * @file    appfiles.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for appfiles.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPFILES_H
#define __APPFILES_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "ff.h"
#include "appprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//dir path
//#define MUSIC_DIR                          "0:/MUSIC"
//#define RECORD_DIR                         "0:/RECORDER"
//#define CONFIG_DIR                         "0:/CONFIG"
//#define LOG_DIR                            "0:/LOG"

#define README_STRING                      "audio name length must be less than 20!"

//dir name arry index by SD_DIR_NAME
#define MUSIC_DIR                          0
#define RECORD_DIR                         1
#define CONFIG_DIR                         2
#define LOG_DIR                            3
#define MAX_DIR                            LOG_DIR+1

/* Exported types ------------------------------------------------------------*/

//global dir and files handle struct
typedef struct 
{
	DIR alldir[MAX_DIR];  //ALL DIR
	FIL tempfile;         //临时操作文件
}appfile_st;

#pragma pack(1)
//audio info struct
typedef rgetAudioInfo_plst audioinfo_st;
//typedef struct
//{
//	u8 id;                           //notice：when id == 0xff，代表无音频
//	u8 name[AUDIO_NAME_LEN];
//	u32 time;
//	u32 size;
//	u8 attr;
//	u8 format;
//	u32 samplerate;
//	u32 bitrate;
//	u16 bps;
//}rgetAudioInfo_plst;
//audio info list struct
typedef struct
{
	/********
  0x84218421 ( 0b1000 0100 0010 0001 1000 0100 0010 0001 ) 表示值1 6 11 16 17 22 27 32 存在音频文件
	*****************/
	u8 rec_num;                        //录音文件数目
	u8 music_num;                      //音乐文件数目
	u32 id_mask;                       //id值对应二进制的某位   
	audioinfo_st list[ID_LIST_MAX];    //index存储对应id的音频信息
}audioinfolist_st;
#pragma pack()

/* Exported functions ------------------------------------------------------- */
void appfile_init(void);
void appfile_prcs(void);


extern const char SD_DIR_NAME[][12];
#endif /* __APPAUDIO_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
