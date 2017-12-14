/**
  ******************************************************************************
  * @file    appfiles.c 
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
#include "appfiles.h"
#include "wavplay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
const char SD_DIR_NAME[MAX_DIR][DIR_NAME_MAXLEN] = 
{
	"0:/MUSIC",
	"0:/RECORDER",
	"0:/CONFIG",
	"0:/LOG"
};
/* Private variables ---------------------------------------------------------*/
appfile_st _appfiles_handlest;                   //音频文件操作相关结构体
appfile_st *appfiles = &_appfiles_handlest;
appfile_st *appfiles_hdle_pst = &_appfiles_handlest;
audioinfolist_st _audioinfolist_st;              //音频信息列表结构体
audioinfolist_st *audiolist_pst = &_audioinfolist_st;
audioinfo_st *infolist_pst = _audioinfolist_st.list;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void printf_audio_list_info(void);

///////////////////////////////////////////////////////////////////////////////process funciton
/**
  * @brief  appfile_init
  * @param  
  * @note   
  * @retval
  */
void appfile_init(void)
{
	int i;
	u8 id_index=0;
	char res;
	FILINFO fileinfo;
	char *fn;  
	char lfname[_MAX_LFN];
	__wavctrl wavc;
	fileinfo.lfname = lfname;
	fileinfo.lfsize = _MAX_LFN;
	char obj_name[50];
	
	//init param
	audiolist_pst->id_mask =0;
	audiolist_pst->music_num = 0;
	audiolist_pst->rec_num = 0;
	for(i=0; i<ID_LIST_MAX; i++)
	{
		infolist_pst[i].id = AUDIOID_NONE;
	}
  //write readme info
	if(f_open(&appfiles->tempfile, "0:/readme.txt", FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
	{
		f_write(&appfiles->tempfile, README_STRING, sizeof(README_STRING), (UINT*)&i);
	}
	f_close(&appfiles->tempfile);
	
	//init all dir: create or open
	for(i=0; i<MAX_DIR; i++)
	{
		while(f_opendir(&appfiles->alldir[i], SD_DIR_NAME[i])!=FR_OK)
		{
			printf("[%s] dir open fail\n", SD_DIR_NAME[i]);
			ostmr_wait(5);
			f_mkdir(SD_DIR_NAME[i]);
		}
		printf("[%s] dir open ok\n", SD_DIR_NAME[i]);
	}
	
	//get all music files info
	while(1)
	{
		if(id_index >=ID_LIST_MAX)  //超过允许list
			break;
		
		res = f_readdir(&appfiles->alldir[MUSIC_DIR], &fileinfo); //读取目录下的一个文件
		if(res != FR_OK || fileinfo.fname[0] == 0) 
			break;  //错误了/到末尾了,退出
		if (fileinfo.fname[0] == '.') 
			continue;             //忽略上级目录
	  fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
		
		audiolist_pst->music_num++;
		setBit(audiolist_pst->id_mask, id_index);
		infolist_pst[id_index].id = id_index;
		strncpy((char*)infolist_pst[id_index].name, fn, AUDIO_NAME_LEN-1);
		infolist_pst[id_index].name[AUDIO_NAME_LEN-1] = '\0';
		infolist_pst[id_index].attr = ATTR_MUSIC;
		infolist_pst[id_index].format = FORMAT_WAV;
		snprintf(obj_name, sizeof(obj_name), "%s/%s", SD_DIR_NAME[MUSIC_DIR], infolist_pst[id_index].name);  //获取文件名的绝对路径
		res = wav_decode_init((u8*)obj_name, &wavc); //获取wav信息
		if(!res)
		{
			infolist_pst[id_index].bitrate = wavc.bitrate;
			infolist_pst[id_index].bps = wavc.bps;
			infolist_pst[id_index].samplerate = wavc.samplerate;
			infolist_pst[id_index].size = wavc.datasize;
			infolist_pst[id_index].time = wavc.datasize/(wavc.bitrate/8);	//歌曲总长度(单位:秒) 
		}
		id_index++;
	}
	
	//get all record files info
	while(1)
	{
		if(id_index >=ID_LIST_MAX)  //超过允许list
			break;
		
		res = f_readdir(&appfiles->alldir[RECORD_DIR], &fileinfo); //读取目录下的一个文件
		if(res != FR_OK || fileinfo.fname[0] == 0) 
			break;  //错误了/到末尾了,退出
		if (fileinfo.fname[0] == '.') 
			continue;             //忽略上级目录
	  fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
		
		audiolist_pst->rec_num++;
		setBit(audiolist_pst->id_mask, id_index);
		infolist_pst[id_index].id = id_index;
		strncpy((char*)infolist_pst[id_index].name, fn, AUDIO_NAME_LEN-1);
		infolist_pst[id_index].name[AUDIO_NAME_LEN-1] = '\0';
		infolist_pst[id_index].attr = ATTR_RECORD;
		infolist_pst[id_index].format = FORMAT_WAV;
		snprintf(obj_name, sizeof(obj_name), "%s/%s", SD_DIR_NAME[RECORD_DIR], infolist_pst[id_index].name);  //获取文件名的绝对路径
		res = wav_decode_init((u8*)obj_name, &wavc); //获取wav信息
		if(!res)
		{
			infolist_pst[id_index].bitrate = wavc.bitrate;
			infolist_pst[id_index].bps = wavc.bps;
			infolist_pst[id_index].samplerate = wavc.samplerate;
			infolist_pst[id_index].size = wavc.datasize;
			infolist_pst[id_index].time = wavc.datasize/(wavc.bitrate/8);	//歌曲总长度(单位:秒) 
		}
		id_index++;
	}
  printf_audio_list_info();
}
/**
  * @brief  appfile_prcs
  * @param  
  * @note   
  * @retval
  */
void appfile_prcs(void)
{

}
///////////////////////////////////////////////////////////////////////////////item funciton
/**
  * @brief  audio_item_exist
  * @param  
  * @note   
  * @retval 
  */
bool audio_item_exist(u8 id)
{
	u8 *id_v = &infolist_pst[id].id;
	if((IS_AUDIOID((int)id))&&(id == *id_v))
	{
		return true;
	}
	return false;
}
/**
  * @brief  audio_item_add
just for record audio
  * @param  
  * @note   
  * @retval 
  */
bool audio_item_add(const u8* name, u8 *newid)
{
	int i;
	for(i=0; i<ID_LIST_MAX; i++)
	{
		if(infolist_pst[i].id == AUDIOID_NONE)
		{
			infolist_pst[i].id = i;
			*newid = i;
			strcpy((char*)infolist_pst[i].name, (char*)name);
			infolist_pst[i].attr = ATTR_RECORD;
			infolist_pst[i].format = FORMAT_WAV;
			infolist_pst[i].bitrate = 16000*4*8;
			infolist_pst[i].bps = 16;
			infolist_pst[i].samplerate = 16000;
			infolist_pst[i].time =0;
			infolist_pst[i].size =0;
		  _audioinfolist_st.rec_num++; //add record num
			setBit(_audioinfolist_st.id_mask, i); //remap list mapping
			return true;
		}
	}
	return false;
}
/**
  * @brief  audio_item_mofiy
  * @param  
  * @note   
  * @retval 
  */
bool audio_item_mofiy(u8 id, const u8* name)
{
	u8 *idname = infolist_pst[id].name;
	if(audio_item_exist(id))
	{
		strcpy((char*)idname, (char*)name);
		return true;
	}
	return false;
}
/**
  * @brief  audio_item_del
  * @param  
  * @note   
  * @retval
  */
bool audio_item_del(u8 id)
{
	u8 *id_v = &infolist_pst[id].id;
	u8 arr = infolist_pst[id].attr;
	if(audio_item_exist(id))
	{
		if(arr == ATTR_RECORD)
		{
			_audioinfolist_st.rec_num--; //minus record num
			clearBit(_audioinfolist_st.id_mask, id); //remap list mapping
			*id_v = AUDIOID_NONE; //id reset to none
			return true;
		}
	}	
	return false;
}
/**
  * @brief  audio_item_get
获取目前id的信息指针
  * @param  
  * @note   
  * @retval
  */
audioinfo_st* audio_item_get(u8 id)
{
	if(audio_item_exist(id))
		return &infolist_pst[id];
	return NULL;
}
/**
  * @brief  audio_item_nextid_loop 
根据给定ID值，获取下一个有效ID值
  * @param  
  * @note   
  * @retval
  */
u8 audio_item_nextid_loop(u8 id)
{
  u8 newid = id+1;
	newid = newid>=ID_LIST_MAX? 0:newid;
	while(infolist_pst[newid].id == AUDIOID_NONE)
	{
		newid++;
		newid = newid>=ID_LIST_MAX? 0:newid;
	}
	return newid;
}
/**
  * @brief  audio_item_check_name_repeat
  * @param  
  * @note   
  * @retval 
  */
bool audio_item_check_name_repeat(const u8* name)
{
	int i;
	for(i=0; i<ID_LIST_MAX; i++)
	{
		if(infolist_pst[i].id != AUDIOID_NONE)
		{
			if(strcmp((char*)name, (char*)infolist_pst[i].name)==0)
				return true;
		}
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////file funciton



///////////////////////////////////////////////////////////////////////////////other funciton
/**
  * @brief  printf_audio_list_info
  * @param  
  * @note   
  * @retval
  */
static void printf_audio_list_info(void)
{
	int i;
	printf("-----------------------audio list info start-------------------\n");
	printf("[music num]:%d, [record num]:%d, [list map]:%d\n", audiolist_pst->music_num, audiolist_pst->rec_num, audiolist_pst->id_mask);
	for(i=0; i<ID_LIST_MAX; i++)
	{
		printf("[id]:%d\n", infolist_pst[i].id);
		printf("[name]:%s\n", infolist_pst[i].name);
		printf("[attr]:%d\n", infolist_pst[i].attr);
		printf("[bitrate]:%d\n", infolist_pst[i].bitrate);
		printf("[bps]:%d\n", infolist_pst[i].bps);
		printf("[format]:%d\n", infolist_pst[i].format);
		printf("[samplerate]:%d\n", infolist_pst[i].samplerate);
		printf("[size]:%d\n", infolist_pst[i].size);
		printf("[time]:%d\n", infolist_pst[i].time);
	}
	printf("-----------------------audio list info end-------------------\n");
}
/**
  * @brief  printf_audio_list_info
  * @param  
  * @note   
  * @retval
  */
void printf_audio_item(const audioinfo_st *item)
{
	printf("[id]:%d\n", item->id);
	printf("[name]:%s\n", item->name);
	printf("[attr]:%d\n", item->attr);
	printf("[bitrate]:%d\n", item->bitrate);
	printf("[bps]:%d\n", item->bps);
	printf("[format]:%d\n", item->format);
	printf("[samplerate]:%d\n", item->samplerate);
	printf("[size]:%d\n", item->size);
	printf("[time]:%d\n", item->time);
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
