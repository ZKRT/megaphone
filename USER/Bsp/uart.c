
#include "uart.h"
#include "dji_ringbuffer.h"
//#include "cmsis_os.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "sys.h"
#include "timer.h"

//uart uart buffer size define
#define UART_READ_BUF_SIZE      2048
#define UART_WRITE_BUF_SIZE     2048

//uart uart priority define
#define UART1_IRQ_PRIO_PRE      USART_TEST_PreemptionPriority
#define UART1_IRQ_PRIO_SUB      USART_TEST_SubPriority

#define UART2_IRQ_PRIO_PRE      UART2_PreemptionPriority
#define UART2_IRQ_PRIO_SUB      UART2_SubPriority

#define UART3_IRQ_PRIO_PRE      1
#define UART3_IRQ_PRIO_SUB      1

//uart GPIO define
#define UART1_GPIO_TX       GPIO_Pin_6
#define UART1_TX_PIN_SOURCE GPIO_PinSource6
#define UART1_GPIO_RX       GPIO_Pin_7
#define UART1_RX_PIN_SOURCE GPIO_PinSource7
#define UART1_GPIO          GPIOB
#define UART1_GPIO_RCC      RCC_AHB1Periph_GPIOB
#define RCC_APBPeriph_UART1 RCC_APB2Periph_USART1

#define UART2_GPIO_TX       GPIO_Pin_2
#define UART2_TX_PIN_SOURCE GPIO_PinSource2
#define UART2_GPIO_RX       GPIO_Pin_3
#define UART2_RX_PIN_SOURCE GPIO_PinSource3
#define UART2_GPIO          GPIOA
#define UART2_GPIO_RCC      RCC_AHB1Periph_GPIOA
#define RCC_APBPeriph_UART2 RCC_APB1Periph_USART2

#define UART3_GPIO_TX       GPIO_Pin_8
#define UART3_TX_PIN_SOURCE GPIO_PinSource8
#define UART3_GPIO_RX       GPIO_Pin_9
#define UART3_RX_PIN_SOURCE GPIO_PinSource9
#define UART3_GPIO          GPIOD
#define UART3_GPIO_RCC      RCC_AHB1Periph_GPIOD
#define RCC_APBPeriph_UART3 RCC_APB1Periph_USART3

#ifdef USING_UART_PORT_1
static volatile T_RingBuffer s_uart1ReadRingBuffer;
static volatile T_RingBuffer s_uart1WriteRingBuffer;
static volatile uint8_t s_uart1ReadBuf[UART_READ_BUF_SIZE];
static volatile uint8_t s_uart1WriteBuf[UART_WRITE_BUF_SIZE];
#if PSDK_ARCH_USE_OS
static osMutexId s_uart1Mutex;
#endif
#endif

#ifdef USING_UART_PORT_2
static volatile T_RingBuffer s_uart2ReadRingBuffer;
static volatile T_RingBuffer s_uart2WriteRingBuffer;
static volatile uint8_t s_uart2ReadBuf[UART_READ_BUF_SIZE];
static volatile uint8_t s_uart2WriteBuf[UART_WRITE_BUF_SIZE];
#if PSDK_ARCH_USE_OS
static osMutexId s_uart2Mutex;
#endif
#endif

#ifdef USING_UART_PORT_3
static T_RingBuffer s_uart3ReadRingBuffer;
static T_RingBuffer s_uart3WriteRingBuffer;
static uint8_t s_uart3ReadBuf[UART_READ_BUF_SIZE];
static uint8_t s_uart3WriteBuf[UART_WRITE_BUF_SIZE];
#if PSDK_ARCH_USE_OS
static osMutexId s_uart3Mutex;
#endif
#endif

#define UART_RINGBUFFER_INIT(n)                                                         \
{                                                                                       \
RingBuf_Init(&s_uart##n##ReadRingBuffer, s_uart##n##ReadBuf, UART_READ_BUF_SIZE);       \
RingBuf_Init(&s_uart##n##WriteRingBuffer, s_uart##n##WriteBuf, UART_WRITE_BUF_SIZE);    \
}

#define RCC_CONFIG_UART(n)                                                      \
{                                                                               \
RCC_AHB1PeriphClockCmd(UART##n##_GPIO_RCC, ENABLE);                             \
RCC_APB2PeriphClockCmd(RCC_APBPeriph_UART##n, ENABLE);                          \
}

#define GPIO_CONFIG_UART(n)                                                     \
{                                                                               \
GPIO_InitTypeDef GPIO_InitStructure;                                            \
                                                                                \
GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                   \
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                  \
GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                   \
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                               \
GPIO_InitStructure.GPIO_Pin = UART##n##_GPIO_TX | UART##n##_GPIO_RX;            \
GPIO_Init(UART##n##_GPIO, &GPIO_InitStructure);                                 \
GPIO_PinAFConfig(UART##n##_GPIO, UART##n##_TX_PIN_SOURCE, GPIO_AF_USART##n);    \
GPIO_PinAFConfig(UART##n##_GPIO, UART##n##_RX_PIN_SOURCE, GPIO_AF_USART##n);    \
}

#define UART_CONFIG(n, bd)                                                      \
{                                                                               \
USART_InitTypeDef USART_InitStructure;                                          \
                                                                                \
USART_InitStructure.USART_BaudRate = bd;                                        \
USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     \
USART_InitStructure.USART_StopBits = USART_StopBits_1;                          \
USART_InitStructure.USART_Parity = USART_Parity_No;                             \
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; \
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 \
                                                                                \
USART_Init(USART##n, &USART_InitStructure);                                     \
                                                                                \
USART_Cmd(USART##n, ENABLE);                                                    \
                                                                                \
USART_ITConfig(USART##n, USART_IT_RXNE, ENABLE);                                \
}

#define UART_NVIC_CONFIG(n, prePrio, subPrio)                                   \
{                                                                               \
NVIC_InitTypeDef NVIC_InitStructure;                                            \
                                                                                \
NVIC_InitStructure.NVIC_IRQChannel = USART##n##_IRQn;                           \
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prePrio;                 \
NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPrio;                        \
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                 \
NVIC_Init(&NVIC_InitStructure);                                                 \
}
// #define UART_IRQ_HANDLER(n) \
// void USART##n##_IRQHandler()\
// {\
//     uint8_t data;\
//     \
//     if (USART_GetITStatus(USART##n, USART_IT_RXNE) != RESET) {\
//         data = (uint8_t)(USART_ReceiveData(USART##n) & 0x00FF);\
//         RingBuf_Put(&s_uart##n##ReadRingBuffer, &data, 1);\
//     }\
//     \
//     if(USART_GetITStatus(USART##n, USART_IT_TXE) != RESET) {\
//         if(RingBuf_Get(&s_uart##n##WriteRingBuffer, &data, 1)) {\
//             USART_SendData(USART##n, data);\
//         } else {\
//             USART_ITConfig(USART##n, USART_IT_TXE, DISABLE);\
//         }\
//     }\
// 	if (USART_GetFlagStatus(USART##n, USART_FLAG_ORE) != RESET) {\
// 		USART_ReceiveData(USART##n);\
// 		USART_ClearFlag(USART##n, USART_FLAG_ORE);\
// 	}\
// }
#define UART_IRQ_HANDLER(n) \
void USART##n##_IRQHandler()\
{\
   uint8_t data;\
   \
   if (USART_GetITStatus(USART##n, USART_IT_RXNE) != RESET) {\
       data = (uint8_t)(USART_ReceiveData(USART##n) & 0x00FF);\
       RingBuf_Put(&s_uart##n##ReadRingBuffer, &data, 1);\
       if(n == PSDK_UART_NUM) {\
           if (app_timer_cnt.api_rx==0) {\
               GPIO_WriteBit(APIRX_LED_PORT, APIRX_LED_PIN, LED_LIGHT);\
				app_timer_cnt.api_rx = APIRX_LED_CNT;\
           }\
       }\
   }\
   \
   if(USART_GetITStatus(USART##n, USART_IT_TXE) != RESET) {\
       if(RingBuf_Get(&s_uart##n##WriteRingBuffer, &data, 1)) {\
           USART_SendData(USART##n, data);\
           if(n == PSDK_UART_NUM) {\
               if (app_timer_cnt.api_tx==0) {\
                   GPIO_WriteBit(APITX_LED_PORT, APITX_LED_PIN, LED_LIGHT);\
					app_timer_cnt.api_tx = APITX_LED_CNT;\
                   }\
           }\
       } else {\
           USART_ITConfig(USART##n, USART_IT_TXE, DISABLE);\
       }\
   }\
   if (USART_GetFlagStatus(USART##n, USART_FLAG_ORE) != RESET) {\
     USART_ReceiveData(USART##n);\
     USART_ClearFlag(USART##n, USART_FLAG_ORE);\
    }\
}
void UART_Init(E_UartNum uartNum, uint32_t baudRate)
{    
    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
            UART_RINGBUFFER_INIT(1);
            RCC_AHB1PeriphClockCmd(UART1_GPIO_RCC, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APBPeriph_UART1, ENABLE);
            GPIO_CONFIG_UART(1);
            UART_NVIC_CONFIG(1, UART1_IRQ_PRIO_PRE, UART1_IRQ_PRIO_SUB);
            UART_CONFIG(1, baudRate);
#if PSDK_ARCH_USE_OS            
            osMutexDef(uart1Mutex);
            s_uart1Mutex = osMutexCreate(osMutex(uart1Mutex));
#endif			
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
            UART_RINGBUFFER_INIT(2);
            RCC_AHB1PeriphClockCmd(UART2_GPIO_RCC, ENABLE);
            RCC_APB1PeriphClockCmd(RCC_APBPeriph_UART2, ENABLE);
            GPIO_CONFIG_UART(2);
            UART_NVIC_CONFIG(2, UART2_IRQ_PRIO_PRE, UART2_IRQ_PRIO_SUB);
            UART_CONFIG(2, baudRate);
#if PSDK_ARCH_USE_OS              
            osMutexDef(uart2Mutex);
            s_uart2Mutex = osMutexCreate(osMutex(uart2Mutex));
#endif			
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
            UART_RINGBUFFER_INIT(3);
            RCC_AHB1PeriphClockCmd(UART3_GPIO_RCC, ENABLE);
            RCC_APB1PeriphClockCmd(RCC_APBPeriph_UART3, ENABLE);
            GPIO_CONFIG_UART(3);
            UART_NVIC_CONFIG(3, UART3_IRQ_PRIO_PRE, UART3_IRQ_PRIO_SUB);
            UART_CONFIG(3, baudRate);
#if PSDK_ARCH_USE_OS              
            osMutexDef(uart3Mutex);
            s_uart3Mutex = osMutexCreate(osMutex(uart3Mutex));
#endif			
        }
            break;
#endif

        default:
            break;
    }
}

int UART_Read(E_UartNum uartNum, uint8_t *buf, uint16_t readSize)
{
    uint16_t readRealSize;

    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
#if PSDK_ARCH_USE_OS  			
            osMutexWait(s_uart1Mutex, osWaitForever);
#endif			
            readRealSize = RingBuf_Get(&s_uart1ReadRingBuffer, buf, readSize);
#if PSDK_ARCH_USE_OS 			
            osMutexRelease(s_uart1Mutex);
#endif			
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
#if PSDK_ARCH_USE_OS 			
            osMutexWait(s_uart2Mutex, osWaitForever);
#endif			
            readRealSize = RingBuf_Get(&s_uart2ReadRingBuffer, buf, readSize);
#if PSDK_ARCH_USE_OS 			
            osMutexRelease(s_uart2Mutex);
#endif
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
#if PSDK_ARCH_USE_OS 			
            osMutexWait(s_uart3Mutex, osWaitForever);
#endif			
            readRealSize = RingBuf_Get(&s_uart3ReadRingBuffer, buf, readSize);
#if PSDK_ARCH_USE_OS 			
            osMutexRelease(s_uart3Mutex);
#endif			
        }
            break;
#endif

        default:
            return UART_ERROR;
    }

    return readRealSize;
}

int UART_Write(E_UartNum uartNum, const uint8_t *buf, uint16_t writeSize)
{
    int writeRealLen;

    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
#if PSDK_ARCH_USE_OS 				
            osMutexWait(s_uart1Mutex, osWaitForever);
#endif
            writeRealLen = RingBuf_Put(&s_uart1WriteRingBuffer, buf, writeSize);
            USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#if PSDK_ARCH_USE_OS 				
            osMutexRelease(s_uart1Mutex);
#endif
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
#if PSDK_ARCH_USE_OS 				
            osMutexWait(s_uart2Mutex, osWaitForever);
#endif
            writeRealLen = RingBuf_Put(&s_uart2WriteRingBuffer, buf, writeSize);
            USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#if PSDK_ARCH_USE_OS 				
            osMutexRelease(s_uart2Mutex);
#endif
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
#if PSDK_ARCH_USE_OS 				
            osMutexWait(s_uart3Mutex, osWaitForever);
#endif
            writeRealLen = RingBuf_Put(&s_uart3WriteRingBuffer, buf, writeSize);
            USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
#if PSDK_ARCH_USE_OS 				
            osMutexRelease(s_uart3Mutex);
#endif			
        }
            break;
#endif

        default:
            return UART_ERROR;
    }

    return writeRealLen;
}

#ifdef USING_UART_PORT_1
UART_IRQ_HANDLER(1)
#endif

#ifdef USING_UART_PORT_2
UART_IRQ_HANDLER(2)
#endif

#ifdef USING_UART_PORT_3
UART_IRQ_HANDLER(3)
#endif

#if 0
void USART1_IRQHandler()
{
    uint8_t data;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        data = (uint8_t)(USART_ReceiveData(USART1) & 0x00FF);
        RingBuf_Put(&s_uart1ReadRingBuffer, &data, 1);
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        if(RingBuf_Get(&s_uart1WriteRingBuffer, &data, 1)) {
            USART_SendData(USART1, data);
        } else {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}
#endif
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
