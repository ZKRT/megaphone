#include "can.h"
#include "led.h"
#include "zkrt.h"

volatile uint8_t can1_rx_buff[CAN_BUFFER_SIZE];
volatile uint16_t can1_rx_buff_store = 0;
uint16_t can1_rx_buff_get = 0;

uint8_t CAN_Mode_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(CAN_M_RCC_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = CAN_M_T_PIN | CAN_M_R_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_M_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(CAN_M_PORT, CAN_M_T_PINSE, CAN_AF_NAME);
    GPIO_PinAFConfig(CAN_M_PORT, CAN_M_R_PINSE, CAN_AF_NAME);

    CAN_DeInit(CAN_M_NUM);
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;

    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
    CAN_InitStructure.CAN_Prescaler = 7;

    //设置完11个参数后，进行CAN初始化
    CAN_Init(CAN_M_NUM, &CAN_InitStructure); // 初始化CAN1

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (DEVICE_TYPE_SELF << 5);
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN_PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN_SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN_M_NUM, CAN_IT_FMP0, ENABLE);

    return 0;
}

void CAN1_RX0_IRQHandler(void)
{
    uint8_t i;
    CanRxMsg RxMessage;

    CAN_Receive(CAN_M_NUM, CAN_FIFO0, &RxMessage); //CAN设备里的FIFO0的数据，读取出来存到RxMessage结构体里

    for (i = 0; i < RxMessage.DLC; i++)
    {
        can1_rx_buff[can1_rx_buff_store] = RxMessage.Data[i];

        can1_rx_buff_store++;
        if (can1_rx_buff_store == CAN_BUFFER_SIZE)
        {
            can1_rx_buff_store = 0;
        }
    }
    if (!_can_led_cnt)
    {
        GPIO_WriteBit(CAN_LED_PORT, CAN_LED_PIN, LED_LIGHT);
        _can_led_cnt = CAN_LED_INTERVAL;
    }
}

//通过判断get和store的位置关系来决定是否有新值
uint8_t CAN1_rx_check(void)
{
    if (can1_rx_buff_store == can1_rx_buff_get) //当没有新的值到达时，存储值位置等于取出值位置
        return 0;
    else
        return 1; //当有值到达时，返回1
}

//当有新值的时候，取出一个值
uint8_t CAN1_rx_byte(void)
{
    uint8_t ch;

    ch = can1_rx_buff[can1_rx_buff_get]; //用ch记录下来收到的一个数据

    can1_rx_buff_get++;
    if (can1_rx_buff_get == CAN_BUFFER_SIZE)
    {
        can1_rx_buff_get = 0;
    }

    return ch;
}

////can发送一组数据(固定格式:ID为0X01,标准帧,数据帧)
////len:数据长度(最大为8)
////msg:数据指针,最大为8个字节.
////返回值:0,成功;其他,失败;
//uint8_t Can_Send_Msg(uint8_t* msg,uint8_t len)
//{
//  uint8_t mbox;
//  uint16_t i=0;
//  CanTxMsg TxMessage;
//
//	TxMessage.StdId=(DEVICE_TYPE_SELF<<4);
//  TxMessage.ExtId=0x00;
//  TxMessage.IDE=CAN_Id_Standard;
//  TxMessage.RTR=CAN_RTR_Data;
//  TxMessage.DLC=len;
//  for(i=0;i<len;i++)
//	TxMessage.Data[i]=msg[i];
//
//	mbox= CAN_Transmit(CAN1, &TxMessage);
//
//  i=0;
//  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;
//
//  if(i>=0XFFF)
//	{
//		CAN_Mode_Init();
//		return 1;
//  }
//
//	GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
//	_run_led_cnt = RUN_LED_INTERVAL;
//
//	return 0;		//成功返回
//}

uint8_t Can_Send_Msg(uint8_t *msg, uint8_t len)
{
    uint8_t status;
    uint8_t mbox;
    uint16_t i = 0;
    CanTxMsg TxMessage;

    //pack data
    TxMessage.StdId = (DEVICE_TYPE_SELF << 4);
    TxMessage.ExtId = 0x00;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = len;
    for (i = 0; i < len; i++)
        TxMessage.Data[i] = msg[i];

    GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
    _run_led_cnt = RUN_LED_INTERVAL;

    //transmit
    i = 0;
    mbox = CAN_Transmit(CAN_M_NUM, &TxMessage);
    while ((mbox == CAN_TxStatus_NoMailBox) && (i < 0x4FFF))
    {
        i++;
        mbox = CAN_Transmit(CAN_M_NUM, &TxMessage);
        //		printf("CAN_TxStatus_NoMailBox fail\n");
    }
    if (mbox == CAN_TxStatus_NoMailBox)
    {
        //		printf("can CAN_TxStatus_NoMailBox fail exceed\n");
        //		CAN_Mode_Init();  //zkrt_notice 不应该重启
        return 1;
    }
    //check transmitstatus
    i = 0;
    status = CAN_TransmitStatus(CAN_M_NUM, mbox);
    while ((status != CAN_TxStatus_Ok) && (i < 0x4FFF))
    {
        i++;
        status = CAN_TransmitStatus(CAN_M_NUM, mbox);
    }
    if (i >= 0x4FFF)
    {
        if (status == CAN_TxStatus_Pending)
        {
            printf("can mbox[%d] CAN_TxStatus_Pending\n", mbox);
        }
        else if (status == CAN_TxStatus_Failed)
        {
            CAN_Mode_Init();
            printf("can mbox[%d] CAN_TxStatus_Failed\n", mbox);
            return 1;
        }
    }

    //	GPIO_WriteBit(RUN_LED_PORT, RUN_LED_PIN, LED_LIGHT);
    //	_run_led_cnt = RUN_LED_INTERVAL;

    return 0; //成功返回
}

//将子模块的数据返回给管理模块
uint8_t CAN1_send_message_fun(uint8_t *message, uint8_t len)
{
    //假设一共50个字
    uint8_t count;
    uint8_t time;
    uint8_t ret;

    time = len / 8;

    for (count = 0; count < time; count++)
    {
        ret = Can_Send_Msg(message, 8);
        if (ret)
            return ret;
        message += 8;
    }
    if (len % 8)
    {
        ret = Can_Send_Msg(message, len % 8);
    }
    return ret;
}
