/**
  ******************************************************************************
  * @file    appcan.c 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   canӦ������ͨ��
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
#include "zkrt.h"

/* Private macro -------------------------------------------------------------*/
#define UPLOAD_TIMEOUT		 			    10 //10ms
#define UPLOAD_TIMEOUT_CNT		 			100 //
#define UPLOAD_TIMECNT_INIT         300
//CAN HEARTBEAT CMD relevant
#define CAN_HB_SEQ_INDEX            7
/* Private variables ---------------------------------------------------------*/
uint8_t can_hb_cmd[8] = {0XAA, 0XBB, 0XCC, 0XDD, 0XEE, DEVICE_TYPE_MEGAPHONE, 0X00, 0X00};
uint16_t upload_timecnt;
/* Private functions ---------------------------------------------------------*/
static void data_upload_msTask(void);
static void appcan_hb_pack(void);
static uint8_t appcan_hbpacket_send(void);
static void app_can_recv_handle(void);
/**
  * @brief  appcan_init
  * @param  None
  * @retval None
  */
void appcan_init(void)
{
	t_systmr_insertQuickTask(data_upload_msTask, UPLOAD_TIMEOUT, OSTMR_PERIODIC);
	upload_timecnt = UPLOAD_TIMECNT_INIT; //��ʼ��3���ſ�ʼ��ʱ�ϴ�����
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
		//ִ�ж�ʱ�ϴ�
		appcan_hbpacket_send();
//		printf("upload test\n");
	}
}
/**
  * @brief  appcan_hb_pack  ��������װ
  * @param  None
  * @retval None
  */
static void appcan_hb_pack(void)
{
	can_hb_cmd[CAN_HB_SEQ_INDEX]++;
}
/**
  * @brief  appcan_hbpacket_send  ��������װ
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
  * @brief  data_upload_msTask ��ʱ����
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
	
}

