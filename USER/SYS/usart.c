/**
  ******************************************************************************
  * @file    usart.c
  * @author  ZKRT
  * @version V1.0
  * @date    13-December-2016
  * @brief   串口配置
  *          + (1) init
  *
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "hw_usart.h"
#include "usart.h"
#include "osusart.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void usart_config(void) {
  USART_InitTypeDef USART_InitStructure;
  uint8_t UsartIndex = 0;
  /* 8xUSARTs configuration --------------------------------------------------*/
  /* 8xUSARTs  configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Prepare all uart to receive a data packet */
  for (UsartIndex = 0; UsartIndex < USART_MAX_INDEX; UsartIndex++) {
    USART_Init(UsartInstance[UsartIndex], &USART_InitStructure);

    /* Enable 8xUSARTs Receive interrupts */
    USART_ITConfig(UsartInstance[UsartIndex], USART_IT_RXNE, ENABLE);
    USART_ITConfig(UsartInstance[UsartIndex], USART_IT_TXE, DISABLE);
    /* Enable the 8xUSARTs */
    USART_Cmd(UsartInstance[UsartIndex], ENABLE);
//    USART_ClearFlag(UsartInstance[UsartIndex], USART_FLAG_TC);
    USART_ClearFlag(UsartInstance[UsartIndex], USART_FLAG_RXNE);
  }
}

///////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  //加入以下代码,支持printf函数,而不需要选择use MicroLIB
  */
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE {
  int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x) {
  x = x;
}
int fputc(int ch, FILE *f) {
#ifdef PRINTF_OPEN
  while (USART_GetFlagStatus(USART_TEST_NUM, USART_FLAG_TC) == RESET)
    ;
  USART_SendData(USART_TEST_NUM, (unsigned char) ch);
//  t_osscomm_sendMessage((unsigned char*) &ch, 1, USART_TEST_NUM);
#endif
  return (ch);
}
#endif
///////////////////////////////////////////////////////////////////////////////////STM32 tutorial: Efficiently receive UART data using DMA
///* Include core modules */
//#include "stm32f4xx.h"
//#include "string.h"

///* Receive buffer for DMA */
//#define DMA_RX_BUFFER_SIZE          64
//uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];

///* Buffer after received data */
//#define UART_BUFFER_SIZE            256
//uint8_t UART_Buffer[UART_BUFFER_SIZE];
//size_t Write, Read;
//volatile int readflag;

//USART_InitTypeDef USART_InitStruct;
//DMA_InitTypeDef DMA_InitStruct;
//GPIO_InitTypeDef GPIO_InitStruct;
//NVIC_InitTypeDef NVIC_InitStruct;

//int main(void) {
//  /* Initialize system */
//  SystemInit();
//
//    /* Init GPIO pins for UART */
//    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_DMA2EN;
//    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
//    (void)RCC->AHB1ENR;
//    (void)RCC->APB2ENR;
//
//    /* Set alternate functions */
//    GPIO_PinAFConfig(USART_TEST_TX_PORT, USART_TEST_TX_PINSE, GPIO_AF_USART1);
//    GPIO_PinAFConfig(USART_TEST_RX_PORT, USART_TEST_RX_PINSE, GPIO_AF_USART1);
//
//    /* Init GPIO pins */
//    GPIO_StructInit(&GPIO_InitStruct);
//    GPIO_InitStruct.GPIO_Pin = USART_TEST_TX_PIN | USART_TEST_RX_PIN;
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(USART_TEST_TX_PORT, &GPIO_InitStruct);
//
//    /* Configure UART setup */
//    USART_StructInit(&USART_InitStruct);
//    USART_InitStruct.USART_BaudRate = 115200;
//    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//    USART_InitStruct.USART_Parity = USART_Parity_No;
//    USART_InitStruct.USART_StopBits = USART_StopBits_1;
//    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
//    USART_Init(USART_TEST_NUM, &USART_InitStruct);
//
//    /* Enable global interrupts for USART */
//    NVIC_InitStruct.NVIC_IRQChannel = USART_TEST_IRQ_CH;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
//    NVIC_Init(&NVIC_InitStruct);
//
//    /* Enable USART */
//    USART_Cmd(USART_TEST_NUM, ENABLE);
//    USART_DMACmd(USART_TEST_NUM, USART_DMAReq_Rx, ENABLE);
//    /* Enable IDLE line detection for DMA processing */
//    USART_ITConfig(USART_TEST_NUM, USART_IT_IDLE, ENABLE);
//
//    /* Configure DMA for USART RX, DMA1, Stream5, Channel4 */
//    DMA_StructInit(&DMA_InitStruct);
//    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
//    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)DMA_RX_Buffer;
//    DMA_InitStruct.DMA_BufferSize = DMA_RX_BUFFER_SIZE;
//    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART_TEST_NUM->DR;
//    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
//    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_Init(DMA2_Stream5, &DMA_InitStruct);
//
//    /* Enable global interrupts for DMA stream */
//    NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream5_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//    NVIC_Init(&NVIC_InitStruct);
//
//    /* Enable transfer complete interrupt */
//    DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);
//    DMA_Cmd(DMA2_Stream5, ENABLE);
//
//    while (1) {
//
//        if(readflag)
//        {
//           USART_TEST_NUM->DR = UART_Buffer[Read++];   /* Start byte transfer */
//           while (!(USART_TEST_NUM->SR & USART_SR_TXE));   /* Wait till finished */
//           if (Read == UART_BUFFER_SIZE) {     /* Check buffer overflow */
//                Read = 0;
//           }
//           if(Read == Write)
//           {
//             readflag = 0;
//           }
//        }
////        /**
////         * Loop data back to UART data register
////         */
////        while (Read != Write) {                 /* Do it until buffer is empty */
////            USART_TEST_NUM->DR = UART_Buffer[Read++];   /* Start byte transfer */
////            while (!(USART_TEST_NUM->SR & USART_SR_TXE));   /* Wait till finished */
////            if (Read == UART_BUFFER_SIZE) {     /* Check buffer overflow */
////                Read = 0;
////            }
////
////
////
////        }
//    }
//}

///**
// * \brief       Global interrupt handler for USART2
// */
//void USART1_IRQHandler(void) {
//    /* Check for IDLE flag */
//    if (USART_TEST_NUM->SR & USART_FLAG_IDLE) {         /* We want IDLE flag only */
//        /* This part is important */
//        /* Clear IDLE flag by reading status register first */
//        /* And follow by reading data register */
//        volatile uint32_t tmp;                  /* Must be volatile to prevent optimizations */
//        tmp = USART_TEST_NUM->SR;                       /* Read status register */
//        tmp = USART_TEST_NUM->DR;                       /* Read data register */
//        (void)tmp;                              /* Prevent compiler warnings */
//        DMA2_Stream5->CR &= ~DMA_SxCR_EN;       /* Disabling DMA will force transfer complete interrupt if enabled */
//      readflag = 1;
//    }
//}

///**
// * \brief       Global interrupt handler for DMA1 stream5
// * \note        Except memcpy, there is no functions used to
// */
//void DMA2_Stream5_IRQHandler(void) {
//    size_t len, tocopy;
//    uint8_t* ptr;
//
//    /* Check transfer complete flag */
//    if (DMA2->HISR & DMA_FLAG_TCIF5) {
//        DMA2->HIFCR = DMA_FLAG_TCIF5;           /* Clear transfer complete flag */
//
//        /* Calculate number of bytes actually transfered by DMA so far */
//        /**
//         * Transfer could be completed by 2 events:
//         *  - All data actually transfered (NDTR = 0)
//         *  - Stream disabled inside USART IDLE line detected interrupt (NDTR != 0)
//         */
//        len = DMA_RX_BUFFER_SIZE - DMA2_Stream5->NDTR;
//        tocopy = UART_BUFFER_SIZE - Write;      /* Get number of bytes we can copy to the end of buffer */
//
//        /* Check how many bytes to copy */
//        if (tocopy > len) {
//            tocopy = len;
//        }
//
//        /* Write received data for UART main buffer for manipulation later */
//        ptr = DMA_RX_Buffer;
//        memcpy(&UART_Buffer[Write], ptr, tocopy);   /* Copy first part */
//
//        /* Correct values for remaining data */
//        Write += tocopy;
//        len -= tocopy;
//        ptr += tocopy;
//
//        /* If still data to write for beginning of buffer */
//        if (len) {
//            memcpy(&UART_Buffer[0], ptr, len);      /* Don't care if we override Read pointer now */
//            Write = len;
//        }
//
//        /* Prepare DMA for next transfer */
//        /* Important! DMA stream won't start if all flags are not cleared first */
//        DMA2->HIFCR = DMA_FLAG_DMEIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5 | DMA_FLAG_TEIF5;
//        DMA2_Stream5->M0AR = (uint32_t)DMA_RX_Buffer;   /* Set memory address for DMA again */
//        DMA2_Stream5->NDTR = DMA_RX_BUFFER_SIZE;    /* Set number of bytes to receive */
//        DMA2_Stream5->CR |= DMA_SxCR_EN;            /* Start DMA transfer */
//    }
//}
///////////////////////////////////////////////////////////////////////////////////
/**
*@
*/
