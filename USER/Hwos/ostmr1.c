/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    2014/10/31
  * @brief   //timerX init,interrupt ,this is TIM7, base 1ms, hwtmr2 is no need care just name
  ******************************************************************************/
#include "ostmr1.h"

static volatile vfp_t _qTask;
static volatile vfp_t _fTask;

static volatile uint16_t _sysTimerCnt10ms;

static void TimerInit_2nd(void);

void hwtmr2_enable(void)
{
  TIM_Cmd(QTTC_TIM_NUM, ENABLE);  
}

void hwtmr2_disable(void)
{
  TIM_Cmd(QTTC_TIM_NUM, DISABLE); 
}

void hwtmr2_init(void)
{
  _qTask= NULL;    
  _fTask = NULL;
  _sysTimerCnt10ms = 0;
}

void hwtmr2_irqHandler(void) 
{  
	if (TIM_GetITStatus(QTTC_TIM_NUM, TIM_IT_Update) != RESET)
	{
		_sysTimerCnt10ms++;
		/* 1ms task */
		(*_qTask)();
		if(_sysTimerCnt10ms >= 10)
		{
			(*_fTask)();
			_sysTimerCnt10ms = 0;
		}
		TIM_ClearITPendingBit(QTTC_TIM_NUM, TIM_IT_Update);
	}	
}

bool_t b_hwtmr2_setting( vfp_t quick, vfp_t fast)
{
	_qTask= quick;
	_fTask = fast;

	/* default system timer settings */
	TimerInit_2nd(); // base:1ms , Timer2
//	TIM_NVIC_Configuration_2nd();

	return (TRUE);
}

static void TimerInit_2nd(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(QTTC_RCC_CLK, ENABLE);
  
  /* Time base configuration */

  TIM_TimeBaseStructure.TIM_Period = (1000 - 1); //1ms base 
 
  TIM_TimeBaseStructure.TIM_Prescaler = (QTTC_TIMER_CLK - 1); 

  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(QTTC_TIM_NUM, &TIM_TimeBaseStructure);

  TIM_ClearITPendingBit(QTTC_TIM_NUM, TIM_IT_Update);

  /* TIM IT enable */
  TIM_ITConfig(QTTC_TIM_NUM, TIM_IT_Update, ENABLE);

  /* TIM5 enable counter */
  TIM_Cmd(QTTC_TIM_NUM, ENABLE);  
}
