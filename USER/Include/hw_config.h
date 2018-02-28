/**
  ******************************************************************************
  * @file    hw_config.h 
  * @author  ZKRT
  * @version V1.0
  * @date    13-December-2016
  * @brief   hardware configure message
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported macro ------------------------------------------------------------*/
//whether printf data 
//#define PRINTF_OPEN          //zkrt_debug
#define USE_PRODUCT_AUDIO_OUPUT //zkrt_debug
//#define USE_TMIIC  //zkrt_todo: this driver can not work now.

//unit test debug
//#define UART_TEST_PROTOCOL   //zkrt_debug


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/** @defgroup clock label
  * @{
  */ 
#define SYSTEM_CLK	              168
//APB1-42, APB2-84
#define APB1_CLK		              42
#define APB2_CLK                  84
#define AHB_CLK                   168

/**
  * @}
  */ 

/** @defgroup fast timer task config
  * @{
  */ 
#define FTTC_RCC_CLK                               RCC_APB1Periph_TIM6
#define FTTC_TIM_NUM                               TIM6
#define FTTC_IRQ_CH                                TIM6_DAC_IRQn  
#define FTTC_PreemptionPriority                    0
#define FTTC_SubPriority                           1
#define FTTC_TIMER_CLK                             APB1_CLK*2 

/** @defgroup quick fast timer config
  * @{
  */ 
#define QTTC_RCC_CLK                                RCC_APB1Periph_TIM7
#define QTTC_TIM_NUM                                TIM7
#define QTTC_IRQ_CH                                 TIM7_IRQn
#define QTTC_PreemptionPriority                     0
#define QTTC_SubPriority                            2
#define QTTC_TIMER_CLK                              APB1_CLK*2

/** @defgroup usart
  */ 
#define USART_TEST_NUM                              USART1
#define USART_TEST_IRQ_CH                           USART1_IRQn  
#define USART_TEST_PreemptionPriority               1
#define USART_TEST_SubPriority                      1
#define USART_TEST_TX_PORT                          GPIOB
#define USART_TEST_RX_PORT                          GPIOB
#define USART_TEST_TX_PIN                           GPIO_Pin_6
#define USART_TEST_RX_PIN                           GPIO_Pin_7
#define USART_TEST_TX_PINSE                         GPIO_PinSource6
#define USART_TEST_RX_PINSE                         GPIO_PinSource7

/** @defgroup led
  */ 
#define LED_LIGHT                                   Bit_RESET             
#define LED_DARK                                    Bit_SET
#define RUN_LED_PORT                                GPIOB
#define CAN_LED_PORT                                GPIOB
#define RUN_LED_PIN                                 GPIO_Pin_1
#define CAN_LED_PIN                                 GPIO_Pin_0

/** @defgroup key
  */ 
#define KEY1_PORT                                   GPIOA
#define KEY2_PORT                                   GPIOA
#define KEY3_PORT                                   GPIOA
#define KEY4_PORT                                   GPIOA
#define KEY1_PIN                                    GPIO_Pin_0
#define KEY2_PIN                                    GPIO_Pin_1
#define KEY3_PIN                                    GPIO_Pin_2
#define KEY4_PIN                                    GPIO_Pin_3

/** @defgroup can (comunicatin with MainBoard)
  */
#define CAN_M_RCC_CLK                               RCC_APB1Periph_CAN1
#define CAN_M_T_PIN                                 GPIO_Pin_12
#define CAN_M_R_PIN                                 GPIO_Pin_11
#define CAN_M_PORT                                  GPIOA
#define CAN_M_T_PINSE                               GPIO_PinSource12
#define CAN_M_R_PINSE                               GPIO_PinSource11
#define CAN_M_NUM                                   CAN1
#define CAN_AF_NAME                                 GPIO_AF_CAN1
#define CAN_PreemptionPriority                      1
#define CAN_SubPriority                             0

/** @defgroup sdio
  */
#define SDIO_CMD_PORT                               GPIOD
#define SDIO_CMD_PIN                                GPIO_Pin_2
#define SDIO_CMD_PSE                                GPIO_PinSource2
#define SDIO_CLK_PORT                               GPIOC
#define SDIO_CLK_PIN                                GPIO_Pin_12
#define SDIO_CLK_PSE                                GPIO_PinSource12
#define SDIO_D0_PORT                                GPIOC
#define SDIO_D0_PIN                                 GPIO_Pin_8
#define SDIO_D0_PSE                                 GPIO_PinSource8
#define SDIO_D1_PORT                                GPIOC
#define SDIO_D1_PIN                                 GPIO_Pin_9
#define SDIO_D1_PSE                                 GPIO_PinSource9
#define SDIO_D2_PORT                                GPIOC 
#define SDIO_D2_PIN                                 GPIO_Pin_10
#define SDIO_D2_PSE                                 GPIO_PinSource10
#define SDIO_D3_PORT                                GPIOC
#define SDIO_D3_PIN                                 GPIO_Pin_11
#define SDIO_D3_PSE                                 GPIO_PinSource11
#define SDIO_RCC_CLK                                RCC_APB2Periph_SDIO
#define SDIO_DMA_CLK                                RCC_AHB1Periph_DMA2
#define SDIO_STREAM_X                               DMA2_Stream3
#define SDIO_CHANNEL_NUM                            DMA_Channel_4
#define SDIO_PreemptionPriority                     0
#define SDIO_SubPriority                            0

/** @defgroup iic for wm8978
  */
#define IIC_SDA_WM_PORT                             GPIOB
#define IIC_SDA_WM_PIN                              GPIO_Pin_9
#define IIC_SDA_WM_PSE                              GPIO_PinSource9
#define IIC_SCL_WM_PORT                             GPIOB
#define IIC_SCL_WM_PIN                              GPIO_Pin_8
#define IIC_SCL_WM_PSE                              GPIO_PinSource8

/** @defgroup iis for wm8978
  */
#define IIS_BCLK_WM_PORT                            GPIOB
#define IIS_BCLK_WM_PIN                             GPIO_Pin_13
#define IIS_BCLK_WM_PSE                             GPIO_PinSource13
#define IIS_LRC_WM_PORT                             GPIOB
#define IIS_LRC_WM_PIN                              GPIO_Pin_12
#define IIS_LRC_WM_PSE                              GPIO_PinSource12
#define IIS_MCLK_WM_PORT                            GPIOC
#define IIS_MCLK_WM_PIN                             GPIO_Pin_6
#define IIS_MCLK_WM_PSE                             GPIO_PinSource6
#define IIS_SDIN_WM_PORT                            GPIOC
#define IIS_SDIN_WM_PIN                             GPIO_Pin_3
#define IIS_SDIN_WM_PSE                             GPIO_PinSource3
#define IIS_SDOUT_WM_PORT                           GPIOC
#define IIS_SDOUT_WM_PIN                            GPIO_Pin_2
#define IIS_SDOUT_WM_PSE                            GPIO_PinSource2

/* Exported functions ------------------------------------------------------- */

#endif /* __HW_CONFIG_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

