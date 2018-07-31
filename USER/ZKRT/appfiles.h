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

// #define README_STRING                      "Audio name length must be less than 40!\n"
// #define README_STRING2                      "The number of music and record files must be less than 32!\n"

#define README_STRING                      "������Ƶ�ļ��Ĳ�����ʾ��\r\n1. ��Ƶ�ļ������ܳ���40���ֽ�;\r\n2. ���ֺ�¼����Ƶ��Ŀ�ܺͲ��ܳ���32��;\r\n3. ֻ֧��MP3��WAV��Ƶ��ʽ"
#define README_STRING2                      "The number of music and record files must be less than 32!\n"

//dir name arry index by SD_DIR_NAME
#define MUSIC_DIR                          0
#define RECORD_DIR                         1
#define CONFIG_DIR                         2
#define LOG_DIR                            3
#define MAX_DIR                            LOG_DIR+1
#define DIR_NAME_MAXLEN                    12

/* Exported types ------------------------------------------------------------*/

//global dir and files handle struct
typedef struct {
  DIR alldir[MAX_DIR];  //ALL DIR
  FIL tempfile;         //��ʱ�����ļ�
} appfile_st;

#pragma pack(1)
//audio info struct
typedef rgetAudioInfo_plst audioinfo_st;
//typedef struct
//{
//  u8 id;                           //notice��when id == 0xff����������Ƶ
//  u8 name[AUDIO_NAME_LEN];
//  u32 time;
//  u32 size;
//  u8 attr;
//  u8 format;
//  u32 samplerate;
//  u32 bitrate;
//  u16 bps;
//}rgetAudioInfo_plst;
//audio info list struct
typedef struct {
  /********
  0x84218421 ( 0b1000 0100 0010 0001 1000 0100 0010 0001 ) ��ʾֵ1 6 11 16 17 22 27 32 ������Ƶ�ļ�
  *****************/
  u8 rec_num;                        //¼���ļ���Ŀ
  u8 music_num;                      //�����ļ���Ŀ
  u32 id_mask;                       //idֵ��Ӧ�����Ƶ�ĳλ
  audioinfo_st list[ID_LIST_MAX];    //index�洢��Ӧid����Ƶ��Ϣ
} audioinfolist_st;
#pragma pack()

/* Exported functions ------------------------------------------------------- */
void appfile_init(void);
void appfile_prcs(void);
bool audio_item_exist(u8 id);
bool audio_item_add(const u8* name, u8 *newid);
bool audio_item_mofiy(u8 id, const u8* name);
bool audio_item_del(u8 id);
bool audio_item_check_name_repeat(const u8* name);
void printf_audio_item(const audioinfo_st *item);
u8 checkfileready(void);
u8 audio_item_nextid_loop(u8 id);
u8 audio_item_lastid_loop(u8 id);
audioinfo_st* audio_item_get(u8 id);


extern const char SD_DIR_NAME[][12];
extern appfile_st _appfiles_handlest;
extern appfile_st *appfiles;
extern appfile_st *appfiles_hdle_pst;
extern audioinfolist_st _audioinfolist_st;
extern audioinfolist_st *audiolist_pst;
extern audioinfo_st *infolist_pst;
extern u8 is_file_exsit;
#endif /* __APPAUDIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
