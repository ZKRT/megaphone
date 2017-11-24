/**
  ******************************************************************************
  * @file    commonzkrt.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for commonzkrt.c module
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMONZKRT_H
#define __COMMONZKRT_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup something
  * @{
  */ 
  
/**
  * @}
  */ 
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t CRC16_Cal(uint8_t* ptr, uint8_t len);

#endif /* __COMMONZKRT_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
