#include "sys.h"
#include "led.h"
#include "zkrt.h"
#include "adc.h"
#include "uart.h"
#include "can.h"
/*
头文件中
can.h包含了zkrt.h
light.h包含了can.h、zkrt.h
key.h包含了light.h
*/

void bsp_init(void)
{
	SystemInit ();		/*系统初始化*/
	RCC_Configuration();
	SysTick_Init();
	LED_Init();
//	ADC1_Init();
//	USART1_Config();
	CAN_Mode_Init(CAN_Mode_Normal);
//	KEY_Init();
}

uint8_t status_light[8] = {0XAA, 0XBB, 0XCC, 0XDD, 0XEE, DEVICE_TYPE_MEGAPHONE, 0X00, 0X00};

int main()
{
  bsp_init();
	
	while (1)
	{		
		if (_10ms_count - TimingDelay >= 10)								//10ms一个时间片
		{
			_10ms_count = TimingDelay;
//			ADC_StartOfConversion(ADC1);											//每10ms一次，读取板载电压
//			
//			if ((_10ms_flag%10) == 0)													//每100ms一次，整合电压、检测电压、发送心跳
//			{				
//				if (MAVLINK_TX_INIT_VAL - TimingDelay > 2000)		//初始化的2S内不执行检查，以后每次读取到后都检查
//				{
//					bat_read();
//					if (stand_count - TimingDelay > 500)
//					{
//						bat_check();
//					}
//				}
//			}
			
			if ((_10ms_flag%100) == 0)												//每1000ms一次，发送一次心跳
			{
				if (MAVLINK_TX_INIT_VAL - TimingDelay > 3000)		//初始化的3S内不执行发送心跳，以后每次都发送心跳
				{
//					status_light[6] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
					status_light[7]++;
					if (status_light[7] == 0XFF)
					{
						status_light[7] = 0;
					}
					Can_Send_Msg(status_light, 8);								//这个简单的语句，便于调试响应
				}
			}
			_10ms_flag++;
		}
		
		if (led_rx_count - TimingDelay > 50)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		
		if (led_tx_count - TimingDelay > 50)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_6);
		}
	}
}
