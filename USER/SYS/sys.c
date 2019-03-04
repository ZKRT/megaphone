#include "sys.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t TimingDelay = 0XFFFFFFFF;

/**
  * @brief  INTX_DISABLE //�ر������ж�(���ǲ�����fault��NMI�ж�)
  * @param  None
  * @retval None
  */
__asm void INTX_DISABLE(void) {
  CPSID   I
  BX      LR
}
//���������ж�
__asm void INTX_ENABLE(void) {
  CPSIE   I
  BX      LR
}
/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void) {
//  if (TimingDelay != 0x00)
//  {
  TimingDelay--;
//  }
}
void delay_ms(__IO uint32_t nTime) {

}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime) {
  uint32_t nowtime = TimingDelay;

  while (nowtime - TimingDelay < nTime);
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void iwdg_init(void) {
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //Enables write access to IWDG_PR and IWDG_RLR registers.
  IWDG_SetPrescaler(IWDG_Prescaler_64);   //base 1.6ms
  IWDG_SetReload(2000); //feed dog time
  IWDG_ReloadCounter();
  IWDG_Enable();
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void iwdg_feed(void) {
  IWDG_ReloadCounter();
}
//
char *itoa(int num, char *str, int radix)
{
  char index[] = "0123456789ABCDEF";
  unsigned unum;
  int i = 0, j, k;

  if (radix == 10 && num < 0)
  {
    unum = (unsigned)-num;
    str[i++] = '-';
  }
  else
    unum = (unsigned)num;

  do
  {
    str[i++] = index[unum % (unsigned)radix];
    unum /= radix;
  } while (unum);
  str[i] = '\0';
  if (str[0] == '-')
    k = 1;
  else
    k = 0;
  char temp;
  for (j = k; j <= (i - 1) / 2; j++)
  {
    temp = str[j];
    str[j] = str[i - 1 + k - j];
    str[i - 1 + k - j] = temp;
  }
  return str;
}
