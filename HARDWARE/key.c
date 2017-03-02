#include "key.h"
#include "light.h"

//按键初始化函数，PB4
void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//由于另一端接GND，那么这里初始化为高电平，作为空闲状态；按下时导通，GND拉低IO口形成低电平
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
} 

//对按键操作的轮询动作
uint8_t key_up=1;					//KEY_UP=1代表着已经松开了
uint8_t Key_value = 1;

void KEY_Rock(void)
{
	Key_value = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
	
	if((key_up == 1)&&(Key_value == 0)&&(_key_count - TimingDelay > 500))//从松开到按下：返回相应键值，并且记录按下标志位。设置个时间用于防止多次触发，这里500ms
	{
		_key_count = TimingDelay;
		key_up=0;
		stand_count = TimingDelay;
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		}
	}
	else															//从按下到松开：记录当前是松开的。不管前一次记录的状态，只要读取到的键值是全松开的，那就记录全松开
	{
		key_up = 1;
	}
}
 

















