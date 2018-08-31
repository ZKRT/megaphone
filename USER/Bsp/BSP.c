#include "hw_config.h"
#include "port.h"
#include "BSP.h"
#include "usart.h"
#include "ostmr.h"
#include "osqtmr.h"
#include "osusart.h"
#include "can.h"
#include "exfuns.h"
#include "ff.h"
#include "diskio.h"
#include "wm8978.h"
#include "uart.h"
#include "log_printf.h"
#include <app_config.h>
#include "pwm.h"
#include "dmr818.h"

extern u8 is_file_exsit;

//static void RCC_Configuration(void);
//static void GPIO_Configuration(void);
static void NVIC_Configuration(void);

/*
 * 函数名：BSP_Init
 * 描  述：时钟初始化、硬件初始化
 * 输  入：无
 * 输  出：无
 */
void BSP_Init(void)
{
    FRESULT res;
    /*!< At this stage the microcontroller clock setting is already configured,
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f0xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f0xx.c file
  */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* error */
        while (1)
            ;
    }
    /* Configure clock GPIO, USARTs */
    // RCC_Configuration();

    //  /* Configure GPIO Pin Tx/Rx for Usart communication */
    // GPIO_Configuration();

    /* Configure NVIC */
    NVIC_Configuration();

    b_ostmr_init();
    b_systmr_init();
    //  os_usart_init();  //usart os task init
    //  usart_config();
    LOG_Init();
    UART_Init(PSDK_UART_NUM, 115200);
    UART_Init(DMR818_UART_NUM, 57600);
    pwm_init();
    // CAN_Mode_Init();
    exfuns_init(); //为fatfs相关变量申请内存
    res = f_mount(fs[SD_CARD], "0:", 1);
    if (res == FR_OK)
    { //挂载SD卡
        is_file_exsit = 1;
    }
    printf("f_mount: %d\n", res);
    WM8978_Init(); //初始化WM8978
}
///**
//  * @brief  Configures the different system clocks.
//  * @param  None
//  * @retval None
//  */
//static void RCC_Configuration(void)
//{
//    /* Enable GPIO clock */
//    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

////    /* Enable 1xUSARTs Clock */
////    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

//    //  /* Enable CAN Clock */
//    //  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

//    //  /* Enable I2C Clock */
//    //  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

//    //  /* Enable I2S Clock */
//    //  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

//    //  /* Enable SDIO Clock */
//    //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
//}

///**
//  * @brief  Configures the different GPIO ports.
//  * @param  None
//  * @retval None
//  */
//static void GPIO_Configuration(void)
//{
//    // GPIO_InitTypeDef GPIO_InitStructure;

//    // /*LED Pins configuration  *************************************************/
//    // GPIO_InitStructure.GPIO_Pin = RUN_LED_PIN | APITX_LED_PIN | APIRX_LED_PIN;
//    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    // GPIO_Init(RUN_LED_PORT, &GPIO_InitStructure);

//    // /*KEY Pins configuration  *************************************************/
//    // GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN;
//    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    // GPIO_Init(KEY1_PORT, &GPIO_InitStructure);

////    /* USART1 Pins configuration ************************************************/
////    /* Connect pin to Periph */
////    GPIO_PinAFConfig(USART_TEST_TX_PORT, USART_TEST_TX_PINSE, GPIO_AF_USART1);
////    GPIO_PinAFConfig(USART_TEST_RX_PORT, USART_TEST_RX_PINSE, GPIO_AF_USART1);

////    /* Configure pins as AF pushpull */
////    GPIO_InitStructure.GPIO_Pin = USART_TEST_TX_PIN | USART_TEST_RX_PIN;
////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
////    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
////    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
////    GPIO_Init(USART_TEST_TX_PORT, &GPIO_InitStructure);

//    //  /*CAN Pins configuration  *************************************************/
//    //  /* Connect pin to Periph */
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
//    //
//    //  /* Configure pins as AF pushpull */
//    //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
//    //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    //  GPIO_Init(GPIOA, &GPIO_InitStructure);

//    //  /*I2C1 Pins configuration  *************************************************/
//    //  /* Connect pin to Periph */
//    //  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
//    //  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
//    //
//    //  /* Configure pins as AF pushpull */
//    //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//    //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    //  GPIO_Init(GPIOB, &GPIO_InitStructure);

//    //  /*I2S2 Pins configuration  *************************************************/
//    //  /* Connect pin to Periph */
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);
//    //
//    //  /* Configure pins as AF pushpull */
//    //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
//    //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    //  GPIO_Init(GPIOA, &GPIO_InitStructure);
//    //
//    //  /*SDIO Pins configuration  *************************************************/
//    //  /* Connect pin to Periph */
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
//    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);
//    //
//    //  /* Configure pins as AF pushpull */
//    //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
//    //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    //  GPIO_Init(GPIOA, &GPIO_InitStructure);
//}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the preemption priority and subpriority:
     - 2 bits for pre-emption priority
     - 2 bits for subpriority
   */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* fast timer task IRQ Channel configuration */
    NVIC_InitStructure.NVIC_IRQChannel = FTTC_IRQ_CH;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = FTTC_PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = FTTC_SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* quick fast timer irq configuration */
    NVIC_InitStructure.NVIC_IRQChannel = QTTC_IRQ_CH;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = QTTC_PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = QTTC_SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//    /* USART1 IRQ Channel configuration */
//    NVIC_InitStructure.NVIC_IRQChannel = USART_TEST_IRQ_CH;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_TEST_PreemptionPriority;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_TEST_SubPriority;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    //  /* CAN IRQ Channel configuration */
    //  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    //  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN_PreemptionPriority;
    //  NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN_SubPriority;
    //  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //  NVIC_Init(&NVIC_InitStructure);
}
