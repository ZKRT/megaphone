/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version  
  * @date    
  * @brief   //timerX init，interrupt ,this is TIM6,hwtmr0 is no need care just name
						 供ostmr.c调用,定时器初始化、中断的底层驱动
  ******************************************************************************/
#include "ostmr0.h"

#define	SYSTEM_TIMER_BASE_VALUE	5000


static volatile uint16_t _sysTimerCnt50Ms;        /* system timer counter in ms unit */
static volatile uint16_t _sysTimerCntSec;         /* system timer counter in sec unit */
static volatile uint16_t _taskSeparator5ms;       /* to separate 10ms tasks into 5ms period */
static volatile uint16_t _taskSeparator50ms;      /* to separate 100ms tasks into 5ms period */
static volatile uint16_t _taskSeparator500ms;     /* to separate 1000ms tasks into 5ms period */

static volatile vfp_t _fastTask[NUM_OF_HW_TMR_SEPARATOR], _slowTask[NUM_OF_HW_TMR_SEPARATOR], _secTask[NUM_OF_HW_TMR_SEPARATOR];

void hwtmr0_enable(void) 
{
	TIM_Cmd(FTTC_TIM_NUM, ENABLE);  
}

void hwtmr0_disable(void)
{
	TIM_Cmd(FTTC_TIM_NUM, DISABLE); 
}
void hwtmr0_init(void)
{
  uint32_t        __i;
   
  _taskSeparator5ms   = TRUE;
  _taskSeparator50ms  = TRUE;
  _taskSeparator500ms = TRUE; 
  _sysTimerCnt50Ms    = 0;
  _sysTimerCntSec     = 0;   
  
  for(__i =0; __i<NUM_OF_HW_TMR_SEPARATOR; __i++)
  { 
    _fastTask[__i] = NULL;
    _slowTask[__i] = NULL;
    _secTask[__i]  = NULL;
  }
}


void hwtmr1_irqHandler(void)
{  
	if (TIM_GetITStatus(FTTC_TIM_NUM, TIM_IT_Update) != RESET)
	{
		_sysTimerCnt50Ms++;
		_sysTimerCntSec++; 
		
		/* 10ms task */
		if(_fastTask[_taskSeparator5ms] != NULL)
		{
			(*_fastTask[_taskSeparator5ms])();
			/* alter _taskSeparator bit */
			_taskSeparator5ms = (~_taskSeparator5ms) & 0x00000001;
		}
		
		/* 100ms (normal) task */
		if(_sysTimerCnt50Ms >= 10)
		{
			if(_slowTask[_taskSeparator50ms] != NULL)
			{            
				(*_slowTask[_taskSeparator50ms])();
			}
			_sysTimerCnt50Ms = 0;            
			_taskSeparator50ms = (~_taskSeparator50ms) & 0x00000001;   
		}
		
		/* 1sec (slow) task */
		if(_sysTimerCntSec >= 100)
		{
			if(_secTask[_taskSeparator500ms] != NULL)
			{
				(*_secTask[_taskSeparator500ms])();
			} 
			_sysTimerCntSec = 0;
			_taskSeparator500ms = (~_taskSeparator500ms) & 0x00000001;    
		} 
		TIM_ClearITPendingBit(FTTC_TIM_NUM, TIM_IT_Update);
	}	
}

static void TimerInit_1st(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(FTTC_RCC_CLK, ENABLE);
  
  /* Time base configuration */

  TIM_TimeBaseStructure.TIM_Period = (5000 - 1);
 
  TIM_TimeBaseStructure.TIM_Prescaler = (FTTC_TIMER_CLK - 1); 
	
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(FTTC_TIM_NUM, &TIM_TimeBaseStructure);

//  TIM_ClearITPendingBit(FTTC_TIM_NUM, TIM_IT_Update);

  /* TIM IT enable */
  TIM_ITConfig(FTTC_TIM_NUM, TIM_IT_Update, ENABLE);

  /* TIM5 enable counter */
  TIM_Cmd(FTTC_TIM_NUM, ENABLE);  
}

bool_t b_hwtmr0_setting( vfp_t fast[], vfp_t slow[], vfp_t sec[])
{
	uint32_t            __i;
	for(__i =0; __i<NUM_OF_HW_TMR_SEPARATOR; __i++)
	{
		_fastTask[__i] = fast[__i];
		_slowTask[__i] = slow[__i];
		_secTask[__i]  = sec[__i];
	}
	
  TimerInit_1st();
//  TIM_NVIC_Configuration_1st();
	
	return (TRUE);
}


