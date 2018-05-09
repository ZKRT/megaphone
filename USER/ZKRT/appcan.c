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
#include "appprotocol.h"
#ifdef UART_TEST_PROTOCOL
#include "osusart.h"
#endif
/* Private macro -------------------------------------------------------------*/
#define UPLOAD_TIMEOUT		 			    10 //10ms
#define UPLOAD_TIMEOUT_CNT		 			100 //
#define UPLOAD_TIMECNT_INIT         300
//CAN HEARTBEAT CMD relevant
#define CAN_HB_SEQ_INDEX            7
/* Private variables ---------------------------------------------------------*/
//uint8_t can_hb_cmd[8] = {0XAA, 0XBB, 0XCC, 0XDD, 0XEE, DEVICE_TYPE_MEGAPHONE, 0X00, 0X00};
uint16_t upload_timecnt; //定时上传时间计时
msg_handle_st msg_handlest;
/* Private functions ---------------------------------------------------------*/
static void data_upload_msTask(void);
//static void appcan_hb_pack(void);
static uint8_t appcan_hbpacket_send(void);
static void app_can_recv_handle(void);
static u8 zkrt_decode(zkrt_packet_t *packet);
static u8 common_data_handle(const zkrt_packet_t *spacket, zkrt_packet_t *respond_packet);

/**
  * @brief  appcan_init
  * @param  None
  * @retval None
  */
void appcan_init(void) {
//启动定时任务
	t_systmr_insertQuickTask(data_upload_msTask, UPLOAD_TIMEOUT, OSTMR_PERIODIC);
	upload_timecnt = UPLOAD_TIMECNT_INIT; //初始化3秒后才开始定时上传任务
//信息处理结构体初始化
	//发送结构体初始化
	zkrt_init_packet(&msg_handlest.sendpacket);
	msg_handlest.sendpacket.UAVID[ZK_DINDEX_DEVTYPE] = DEVICE_TYPE_SELF;
}
/**
  * @brief  appcan_prcs
  * @param  None
  * @retval None
  */
void appcan_prcs(void) {
	//recv handle
	app_can_recv_handle();
	//heartbeat handle
	if (!upload_timecnt) {
		upload_timecnt = UPLOAD_TIMEOUT_CNT;
		//执行定时上传
		appcan_hbpacket_send();
	}
}
///**
//  * @brief  appcan_hb_pack  心跳包封装
//  * @param  None
//  * @retval None
//  */
//static void appcan_hb_pack(void)
//{
//	can_hb_cmd[CAN_HB_SEQ_INDEX]++;
//}
/**
  * @brief  appcan_hbpacket_send  心跳包封装
  * @param  None
  * @retval None
  */
static uint8_t appcan_hbpacket_send(void) {
	zkrt_packet_t *packet2 = &msg_handlest.sendpacket;
	uint8_t *sdata = msg_handlest.data;
	common_data_plst* cd = (common_data_plst*)packet2->data;
	common_hbd_plst* chbd = (common_hbd_plst*)cd->type_data;
	//pack heartbeat
	packet2->cmd = SUBDEV_TO_UAV;
	packet2->command = ZK_COMMAND_COMMON;
	packet2->UAVID[ZK_DINDEX_DEVTYPE] = DEVICE_TYPE_SELF;
	packet2->length = THHB_FIXED_LEN;
	cd->type_num = TN_HEARTBEAT;
	chbd->hb_flag = TNHB_FLAG;
	msg_handlest.datalen = zkrt_final_encode(sdata, packet2);
#ifdef UART_TEST_PROTOCOL
	t_osscomm_sendMessage(sdata, msg_handlest.datalen, USART_TEST_NUM);
#else
	CAN1_send_message_fun(sdata, msg_handlest.datalen);
#endif

	return 0;
}
/**
  * @brief  data_upload_msTask 定时任务
  * @param  None
  * @retval None
  */
static void data_upload_msTask(void) {
	if (upload_timecnt) {
		upload_timecnt--;
	}
}
/**
  * @brief  app_can_recv_handle
  * @param  None
  * @retval None
  */
static void app_can_recv_handle(void) {
	zkrt_packet_t *packet = &msg_handlest.recvpacket;
	zkrt_packet_t *packet2 = &msg_handlest.sendpacket;
	u8 ret = NOTNEED_RETRUN;
//receive data
	if (zkrt_decode(&msg_handlest.recvpacket) == 0)
		return;
//check cmd
	if ((packet->cmd != APP_TO_UAV) && (packet->cmd != UAV_TO_SUBDEV) && (packet->cmd != APP_TO_UAV_SUPER))
		return;
//check command
	if (packet->command > ZK_COMMAND_MAX)
		return;
//check uavid
	if (packet->UAVID[3] != DEVICE_TYPE_SELF)
		return;
//check data control num
	if ((packet->data[CTRLNUM_INDEX] >= CN_MAX) || (packet->data[CTRLNUM_INDEX] <= CN_None))
		return;
//check data
	switch (packet->command) {
	case ZK_COMMAND_NORMAL:

		break;

	case ZK_COMMAND_SPECIFIED:
//			printf("ptcol_fun\n");
		ret = ptcol_fun[packet->data[CTRLNUM_INDEX]](packet->data, packet2->data, packet->length, &packet2->length);
		break;

	case ZK_COMMAND_COMMON:
		ret = common_data_handle(packet, packet2);
		break;

	default: break;
	}
//respond data
	if (ret == NEED_RETRUN) {
		packet2->cmd = packet->cmd + 1;
		packet2->command = packet->command;
		msg_handlest.datalen = zkrt_final_encode(msg_handlest.data, packet2);
#ifdef UART_TEST_PROTOCOL
		t_osscomm_sendMessage(msg_handlest.data, msg_handlest.datalen, USART_TEST_NUM);
#else
		ret = CAN1_send_message_fun(msg_handlest.data, msg_handlest.datalen);
#endif
	}
//clear recv packet
	memset(packet, 0, sizeof(zkrt_packet_t));
}
/**
  * @brief  zkrt_decode
  * @param  None
  * @retval None
  */
static u8 zkrt_decode(zkrt_packet_t *packet) {
#ifdef UART_TEST_PROTOCOL
	uint8_t testbuf[100] = "HELLO";
	uint16_t testbuf_len;
	if (t_osscomm_ReceiveMessage(testbuf, &testbuf_len, USART_TEST_NUM) == SCOMM_RET_OK) {
		testbuf_len = testbuf_len > sizeof(zkrt_packet_t) ? sizeof(zkrt_packet_t) : testbuf_len;
		memcpy((void*)packet, testbuf, testbuf_len);
		return 1;
	}
#else

	uint8_t can_value;
	while (CAN1_rx_check() == 1) {
		can_value = CAN1_rx_byte();
		if (zkrt_decode_char(packet, can_value) == 1) {
			return 1;
		}
	}
#endif
	return 0;
}
/**
  * @brief
  * @param  None
  * @retval None
  */
static u8 common_data_handle(const zkrt_packet_t *spacket, zkrt_packet_t *rpacket) {
	u8 res = NEED_RETRUN;
	common_data_plst *scommon = (common_data_plst *)(spacket->data);
	common_data_plst *rcommon = (common_data_plst*)(rpacket->data);
	common_get_devinfo_plst *rother;

	memcpy(rpacket, spacket, ZK_HEADER_LEN);
	//differnet msg handle by type num
	switch (scommon->type_num) {
	case TN_GETDEVINFO:
		rother = (common_get_devinfo_plst *)(rpacket->data + 1);
		rother->status = 0;
		memcpy(rother->model, DEV_MODEL, sizeof(DEV_MODEL));
		memcpy(rother->hw_version, DEV_HW, 6);
		memcpy(rother->sw_version, DEV_SW, 6);
		rpacket->length = sizeof(common_get_devinfo_plst) + 1;
		break;
	default:
		res = NOTNEED_RETRUN;
		break;
	}

	//packet common handle
	if (res == NEED_RETRUN) {
		rcommon->type_num = scommon->type_num;
		rpacket->cmd = UAV_TO_APP;
		rpacket->end_code = _END_CODE;
	}
	return NEED_RETRUN;
}
