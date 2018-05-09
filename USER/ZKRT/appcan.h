/**
  ******************************************************************************
  * @file    appcan.h
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPCAN_H
#define __APPCAN_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//message protocol handle flag and buffer struct
typedef struct {
  zkrt_packet_t recvpacket;
  zkrt_packet_t sendpacket;
  u8 data[ZK_MAX_LEN];
  u8 datalen;
} msg_handle_st;

/* Exported functions ------------------------------------------------------- */
void appcan_init(void);
void appcan_prcs(void);
#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

