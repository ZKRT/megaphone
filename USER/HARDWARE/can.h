#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//APB1_CLK 42
#define _CAN_BAUD_RATE                   500 //500khz //zkrt_notice

#define _CAN_PRESCALER                   7
#define _CAN_BS1                         7
#define _CAN_BS2                         ((APB1_CLK*1000/(_CAN_BAUD_RATE*_CAN_PRESCALER))-3-_CAN_BS1)

#define CAN_BUFFER_SIZE                  (ZK_MAX_LEN*4)      //一个中科瑞泰协议包最多占用100个字节，这里用4个缓冲

uint8_t CAN_Mode_Init(void);
uint8_t CAN1_rx_check(void);
uint8_t CAN1_rx_byte(void);
uint8_t Can_Send_Msg(uint8_t* msg, uint8_t len);						//发送数据
uint8_t CAN1_send_message_fun(uint8_t *message, uint8_t len);//发送指令
#endif
