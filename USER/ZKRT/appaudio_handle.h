/**
  ******************************************************************************
  * @file    appaudio_handle.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for appaudio_handle.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPAUDIO_HANDLE_H
#define __APPAUDIO_HANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/	
/* Exported functions ------------------------------------------------------- */
void enther_getlist_handle(rgetlist_plst *rother);
char enter_startrec_handle(const unsigned char *name, u8 flag, u8 *rid);
char enter_stoprec_handle(u8 flag, rstopRec_plst *rother);
char enter_playsong_handle(const playSong_plst *sother);
char enter_playctrl_handle(u8 option);
char enter_volctrl_handle(u8 vol);
char enter_getaudioinfo_handle(u8 id, rgetAudioInfo_plst *rother);
char enter_delrecaudio_handle(u8 id);
char enter_modifyaudio_handle(const modifyAudio_plst *sother);
char enter_recctrl_handle(const recCtrl_plst *sother);
char enter_getaudiostate_handle(rgetAudioState_plst *rother);

#endif /* __APPAUDIO_HANDLE_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
