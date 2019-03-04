/**
  ******************************************************************************
  * @file    dmr818.c
  * @author  ZKRT
  * @version V1.0
  * @date    2018-08-24
  * @brief   
  *
 ===============================================================================
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dmr818.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DM818_MULTI_CMD_DELAY_INIT 5 //value*100 ms
#define DM818_MULTI_CMD_DELAY 3 //value*100 ms
/* Private macro -------------------------------------------------------------*/
typedef int (*Dmr818_sendFuncptr_t)(E_UartNum, const uint8_t *, uint16_t);
static Dmr818_sendFuncptr_t dmr818_send_data = &UART_Write;
/* Private variables ---------------------------------------------------------*/
dmr818_config_st dmr818_config;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint16_t dmr818_calc_checksum(unsigned char *buf, uint16_t len);
static void dmr818_gpio_init(void);
/**
  * @}
  */

/**
  * @brief 
  * @param  None
  * @retval None
  */
void dmr818_init(void)
{
    //config variables init
    dmr818_config.run_channel = DMR818_DEF_CH;
    dmr818_config.ptt = DMR_PTT_RECV;
    //hardware init
    dmr818_gpio_init();
    //dmr818 chip config by uart and dmr818 protocol
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818cmd_change_channel(dmr818_config.run_channel);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818cmd_set_vol(DMR818_DEF_VOL);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_set_mic(DMR818_DEF_MIC);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_set_feq_by_ch(dmr818_config.run_channel);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_set_tone_type(DMR818_DEF_TONETYPE, DMR818_DEF_TONETYPE);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_set_tone_freq(DMR818_DEF_TONEFREQ, DMR818_DEF_TONEFREQ);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_noise_level(DMR818_DEF_NOISELEV);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
    dmr818_set_tx_power(DMR818_DEF_TXPOWER);
    ostmr_wait(DM818_MULTI_CMD_DELAY_INIT);
}
/**
  * @brief 
  * @param  None
  * @retval None
  */
void dmr818_prcs(void)
{
}
/**
  * @brief  dmr818_gpio_init
  * @param  None
  * @retval None
  */
static void dmr818_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DMR_PTT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = DMR_PTT_PIN;
    GPIO_Init(DMR_PTT_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(DMR_PTT_PORT, DMR_PTT_PIN, (BitAction)DMR_PTT_RECV);
}
/**
  * @brief 
  * @param  None
  * @retval None
  */
char dmr818_interphone_send_or_recv(uint8_t state)
{
    if (state > DMR_PTT_RECV)
        return 0;
    dmr818_config.ptt = state;
    GPIO_WriteBit(DMR_PTT_PORT, DMR_PTT_PIN, (BitAction)dmr818_config.ptt);
    return 1;
}
///////////////////////////////////////////////////////cmd
/**
  * @brief 
  * @param  None
  * @retval None
  */
char dmr818_runtime_change_channel(uint8_t ch)
{
    uint8_t ret = 0;
    dmr818_config.run_channel = ch;
    ret = dmr818cmd_change_channel(dmr818_config.run_channel);
    ostmr_wait(DM818_MULTI_CMD_DELAY);
    ret = dmr818_set_feq_by_ch(ch);
    ostmr_wait(DM818_MULTI_CMD_DELAY);
    ret = dmr818_set_tone_type(DMR818_DEF_TONETYPE, DMR818_DEF_TONETYPE);
    ostmr_wait(DM818_MULTI_CMD_DELAY);
    ret = dmr818_set_tone_freq(DMR818_DEF_TONEFREQ, DMR818_DEF_TONEFREQ);
    return ret;
}
/**
  * @brief 
  * @param  None
  * @retval None
  */
static uint16_t dmr818_calc_checksum(unsigned char *buf, uint16_t len)
{
    uint32_t sum = 0;
    while (len > 1)
    {
        sum += 0xFFFF & (*buf << 8 | *(buf + 1));
        buf += 2;
        len -= 2;
    }
    if (len)
    {
        sum += (0xFF & *buf) << 8;
    }
    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ((uint16_t)sum ^ 0xFFFF);
}
/**
  * @brief  信道切换 0x01
  * @param  None
  * @retval None
  */
char dmr818cmd_change_channel(unsigned char ch)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 10;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_CHANGECH;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = ch;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  接收音量设置 0x02
  * @param  None
  * @retval None
  */
char dmr818cmd_set_vol(unsigned char vol)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 10;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_VOL;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = vol;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  设置Mic增益 0x0b
  * @param  None
  * @retval None
  */
char dmr818_set_mic(unsigned char mic)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 10;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_MIC;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = mic;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  收发频率设置 0x0d
  * @param  None
  * @retval None
  */
char dmr818_set_feq(uint32_t recv_feq, uint32_t send_feq)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 17;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_FEQ;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    memcpy(buffer + 8, &recv_feq, 4);
    memcpy(buffer + 8 + 4, &send_feq, 4);
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  收发频率设置 0x0d
  * @param  None
  * @retval None
  */
char dmr818_set_feq_by_ch(unsigned char ch)
{
    uint32_t freq;
    uint8_t ret;
    if ((ch < 1) || (ch > 16))
        return 0;
    freq = DMR818_CHANNEL_BASE_FREQ + ((ch - 1) * DMR818_CHANNEL_INTERVAL_FREQ);
    ret = dmr818_set_feq(freq, freq);
    return (ret);
}
/**
  * @brief  静噪级别设置 0X12 （zkrt设置为常开，注意：dmr上位机软件显示与命令协议的常开和正常是相反的定义）
  * @param  None
  * @retval None
  */
char dmr818_noise_level(unsigned char level)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 10;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_NOISELEV;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = level;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  设置亚音类型 0x13 （zkrt设置成模拟亚音即CTCSS）
  * @param  None
  * @retval None
  */
char dmr818_set_tone_type(uint8_t recv_tt, uint8_t send_tt)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 11;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_TONETYPE;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = recv_tt;
    buffer[9] = send_tt;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  设置亚音频率 0x14 （zkrt设置为67Hz，即1）
  * @param  None
  * @retval None
  */
char dmr818_set_tone_freq(uint8_t recv_tf, uint8_t send_tf)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 11;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_TONEFEQ;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = recv_tf;
    buffer[9] = send_tf;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @brief  设置发射功率 0x17
  * @param  None
  * @retval None
  */
char dmr818_set_tx_power(uint8_t power)
{
    unsigned char buffer[DMR818_CMD_MAXBUFF] = {0};
    uint16_t len, sum;
    len = 10;
    buffer[0] = DMR818_HEAD;
    buffer[1] = DM818_CMD_SET_TXPOWER;
    buffer[2] = DMR818_WRITE;
    buffer[3] = DMR818_SET;
    buffer[6] = 0;
    buffer[7] = len - DM818_FIXED_LEN;
    buffer[8] = power;
    buffer[len - 1] = DMR818_TAIL;
    sum = dmr818_calc_checksum(buffer, len);
    buffer[5] = (sum & 0xFF);
    buffer[4] = (sum & 0xFF00) >> 8;
    dmr818_send_data(DMR818_UART_NUM, buffer, len);
    return (1);
}
/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
