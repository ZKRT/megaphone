#include "key.h"
#include "light.h"

//������ʼ��������PB4
void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//������һ�˽�GND����ô�����ʼ��Ϊ�ߵ�ƽ����Ϊ����״̬������ʱ��ͨ��GND����IO���γɵ͵�ƽ
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
} 

//�԰�����������ѯ����
uint8_t key_up=1;					//KEY_UP=1�������Ѿ��ɿ���
uint8_t Key_value = 1;

void KEY_Rock(void)
{
	Key_value = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
	
	if((key_up == 1)&&(Key_value == 0)&&(_key_count - TimingDelay > 500))//���ɿ������£�������Ӧ��ֵ�����Ҽ�¼���±�־λ�����ø�ʱ�����ڷ�ֹ��δ���������500ms
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
	else															//�Ӱ��µ��ɿ�����¼��ǰ���ɿ��ġ�����ǰһ�μ�¼��״̬��ֻҪ��ȡ���ļ�ֵ��ȫ�ɿ��ģ��Ǿͼ�¼ȫ�ɿ�
	{
		key_up = 1;
	}
}
 

















