#ifndef __KEY_H
#define __KEY_H	 

/* Includes ------------------------------------------------------------------*/
#include "sys.h" 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//key value read in gpio regester //zkrt_notice
#define KEY1_V 		                         GPIO_ReadInputDataBit(KEY1_PORT,GPIO_Pin_0) 
#define KEY2_V 		                         GPIO_ReadInputDataBit(KEY2_PORT,GPIO_Pin_1)	
#define KEY3_V 		                         GPIO_ReadInputDataBit(KEY3_PORT,GPIO_Pin_2) 
#define KEY4_V	                           GPIO_ReadInputDataBit(KEY4_PORT,GPIO_Pin_3)

//key presed or realese value, this define relevant the gpio resgister  //zkrt_notice
#define KEY_PRESED                         0  
#define KEY_RELEASED                       1

//key num mapp for Key_st[n] 
#define KEY1_NUM                           0
#define KEY2_NUM                           1
#define KEY3_NUM                           2
#define KEY4_NUM                           3
#define KEY_NUM                            4

#define KEY_STOP_SAVE                      KEY3_NUM
#define KEY_PLAY                           KEY2_NUM
#define KEY_REC_PAUSE                      KEY1_NUM
#define KEY_WKUP                           KEY4_NUM

#define KEY0_PRES 	                       1
#define KEY1_PRES	                         2
#define KEY2_PRES	3
#define WKUP_PRES   4  //ZKRT_TODO

//jitter timerout
#define JITTER_CNT                         2  //200ms

//key handle structure
typedef struct
{
	u8 jitter_val;     //jitter value of press or release, KEY_PRESED or KEY_RELEASED
	u8 pres;           //truely value of press KEY_PRESED
	u8 lastj_val;      //last jitter value
	volatile u32 timecnt;       //count, chedk the time of key presed, vaule 1 is equal to 100ms.
	volatile u8 timestart;      //start count flag, 1-statrt, 0-stop
}Key_st;




/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appkey_init(void);
void appkey_prcs(void);
uint8_t key_pressed(uint8_t num, uint8_t sec);
uint8_t which_key_is_pressed(void);
//extern volatile uint8_t _can_led_cnt;
//extern volatile uint8_t _run_led_cnt;
#endif
