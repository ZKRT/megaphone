#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h" 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define RUN_LED_INTERVAL                  1    //
#define CAN_LED_INTERVAL                  1    //


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appled_init(void);
void appled_prcs(void);

extern volatile uint8_t _can_led_cnt;
extern volatile uint8_t _run_led_cnt;
#endif