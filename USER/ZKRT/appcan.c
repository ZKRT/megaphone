/**
  ******************************************************************************
  * @file    appcan.c 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   can应用数据通信
	*					 + (1) init
	*                       
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "can.h"
#include "appcan.h"

/* Private macro -------------------------------------------------------------*/
#define UPLOAD_TIMEOUT		 			    10 //10ms
#define UPLOAD_TIMEOUT_CNT		 			100 //
#define UPLOAD_TIMECNT_INIT         300
//CAN HEARTBEAT CMD relevant
#define CAN_HB_SEQ_INDEX            7
/* Private variables ---------------------------------------------------------*/
uint8_t can_hb_cmd[8] = {0XAA, 0XBB, 0XCC, 0XDD, 0XEE, DEVICE_TYPE_MEGAPHONE, 0X00, 0X00};
uint16_t upload_timecnt; //定时上传时间计时
msg_handle_st msg_handlest; 
/* Private functions ---------------------------------------------------------*/
static void data_upload_msTask(void);
static void appcan_hb_pack(void);
static uint8_t appcan_hbpacket_send(void);
static void app_can_recv_handle(void);
static u8 zkrt_decode(zkrt_packet_t *packet);
/**
  * @brief  appcan_init
  * @param  None
  * @retval None
  */
void appcan_init(void)
{
//启动定时任务	
	t_systmr_insertQuickTask(data_upload_msTask, UPLOAD_TIMEOUT, OSTMR_PERIODIC);	
	upload_timecnt = UPLOAD_TIMECNT_INIT; //初始化3秒后才开始定时上传任务
//信息处理结构体初始化
  	
}
/**
  * @brief  appcan_prcs
  * @param  None
  * @retval None
  */
void appcan_prcs(void)
{
	//recv handle
	app_can_recv_handle();
	//heartbeat handle
	if(!upload_timecnt)
	{
		upload_timecnt = UPLOAD_TIMEOUT_CNT;
		//执行定时上传
		appcan_hbpacket_send();
//		printf("upload test\n");
	}
}
/**
  * @brief  appcan_hb_pack  心跳包封装
  * @param  None
  * @retval None
  */
static void appcan_hb_pack(void)
{
	can_hb_cmd[CAN_HB_SEQ_INDEX]++;
}
/**
  * @brief  appcan_hbpacket_send  心跳包封装
  * @param  None
  * @retval None
  */
static uint8_t appcan_hbpacket_send(void)
{
	uint8_t *data = (uint8_t*)&can_hb_cmd;
	uint8_t ret;
	appcan_hb_pack();
	Can_Send_Msg(data, 8);	
//	ret = CAN1_send_message_fun(data, sizeof(can_hb_cmd));  //ZKRT_TODO
	return ret;
}
/**
  * @brief  data_upload_msTask 定时任务
  * @param  None
  * @retval None
  */
static void data_upload_msTask(void)
{
	if(upload_timecnt)
	{
		upload_timecnt--;
	}
}	
/**
  * @brief  app_can_recv_handle 
  * @param  None
  * @retval None
  */
static void app_can_recv_handle(void)
{
	zkrt_packet_t *packet = &msg_handlest.recvpacket;
//receive data	
	if(zkrt_decode(&msg_handlest.recvpacket)==0)
		return;
//check cmd
	if((packet->cmd!=APP_TO_UAV)&&(packet->cmd!=UAV_TO_SUBDEV)&&(packet->cmd!=APP_TO_UAV_SUPER))
		return;
//check command
	if(packet->command>ZK_COMMAND_MAX)
		return;
//check uavid
  if(packet->UAVID[3]!=DEVICE_TYPE_SELF)
		return;
	
//check data	
}
/**
  * @brief  zkrt_decode 
  * @param  None
  * @retval None
  */
static u8 zkrt_decode(zkrt_packet_t *packet)
{
	uint8_t can_value;
	while(CAN1_rx_check() == 1)
	{
		can_value = CAN1_rx_byte();
		if (zkrt_decode_char(packet,can_value)==1)
		{
			return 1;
		}
	}
	return 0;
}
