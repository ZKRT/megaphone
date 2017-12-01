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
#include "appfiles.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  appaudio_prcs
  * @param  
  * @note   
  * @retval
  */
char appaudio_prcs(void)
{
	
}



/*cmd app function ---------------------------------------------------------*/
/**
  * @brief  cancmd_rec_ctrl��ʼ¼��
  * @param  
  * @note   
  * @retval
  */
char cancmd_rec_ctrl(const u8* name, u8 rec_play_flag, u8* id)
{
	
}
/**
  * @brief  cancmd_stop_rec ֹͣ��ǰ¼�����б���������־
  * @param  save_flag: save or get rid of
  * @note   
  * @retval
  */
char cancmd_stop_rec(u8 id, u8 save_flag)
{
	
}
/**
  * @brief  cancmd_volume_ctrl ��������
  * @param  
  * @note   
  * @retval
  */
char cancmd_volume_ctrl(u8 vol)
{
	
}
/**
  * @brief  cancmd_modify_file_param 
  * @param  
  * @note   
  * @retval
  */
char cancmd_modify_file_param(u8 id, const u8* name)
{
	
}
/**
  * @brief  cancmd_audioplay_setting ��ͣ/��������/ֹͣ
  * @param  
  * @note   
  * @retval
  */
char cancmd_audioplay_setting(u8 play_option)
{
	
}
/**
  * @brief  cancmd_audioplay_start ��ͣ��ǰ���ŵ��ļ��ٿ�ʼ�µĲ���
  * @param  
  * @note   
  * @retval
  */
char cancmd_audioplay_start(u8 playid)
{
	
}
/**
  * @brief  cancmd_check_fileinfo
  * @param  
  * @note   
  * @retval
  */
char cancmd_get_audiofileinfo(u8 id, void* fileinfo)
{
	
}
/**
  * @brief  cancmd_get_audiolist 
  * @param  list is 4 byte 
  -------- -------- -------- --------
0~15 bit is music id flag, 1 is enable, 0 is disable
16~31 bit is recorder id flag, 1 is enable, 0 is disable
  * @note   
  * @retval
  */
char cancmd_get_audiolist(u8 *list)
{
	
}
/**
  * @brief  cancmd_del_audio
  * @param  
  * @note   
  * @retval
  */
char cancmd_del_audio(u8 id)
{
	
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
