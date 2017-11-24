#include "sys.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t TimingDelay = 0XFFFFFFFF;

/**
  * @brief  INTX_DISABLE //关闭所有中断(但是不包括fault和NMI中断)
  * @param  None
  * @retval None
  */
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
//  if (TimingDelay != 0x00)
//  { 
    TimingDelay--;
//  }
}
void delay_ms(__IO uint32_t nTime)
{
	
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
	uint32_t nowtime = TimingDelay;
	
  while(nowtime-TimingDelay < nTime);
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void iwdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //Enables write access to IWDG_PR and IWDG_RLR registers.
	IWDG_SetPrescaler(IWDG_Prescaler_64);		//base 1.6ms
	IWDG_SetReload(2000);	//feed dog time  
	IWDG_ReloadCounter();
	IWDG_Enable();
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void iwdg_feed(void)
{
  IWDG_ReloadCounter();
}
