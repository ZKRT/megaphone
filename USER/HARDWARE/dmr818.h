/**
  ******************************************************************************
  * @file    dmr818.h
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

#ifndef __DMR818_H
#define __DMR818_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "uart.h"
/* Exported macro ------------------------------------------------------------*/
//len
#define DMR818_CMD_MAXBUFF 30
#define DM818_FIXED_LEN 9
//header and tailer
#define DMR818_HEAD 0x68
#define DMR818_TAIL 0x10
//R/W
#define DMR818_READ 0x00
#define DMR818_WRITE 0x01
#define DMR818_UPLOAD 0x02
//S/R
#define DMR818_SET 0x01
#define DM818_RES_OK 0x00
//cmd
#define DM818_CMD_CHANGECH 0x01
#define DM818_CMD_SET_VOL 0x02
#define DM818_CMD_SET_MIC 0x0b
#define DM818_CMD_SET_FEQ 0x0d
#define DM818_CMD_SET_NOISELEV 0x12
#define DM818_CMD_SET_TONETYPE 0x13
#define DM818_CMD_SET_TONEFEQ 0x14
#define DM818_CMD_SET_TXPOWER 0x17
//about send and receive frequency by signal channel
#define DMR818_CHANNEL1_FREQ 418500000
#define DMR818_CHANNEL_INTERVAL_FREQ 25000
#define DMR818_CHANNEL_BASE_FREQ DMR818_CHANNEL1_FREQ
//noise level define
#define DMR818_NOISELEV_NO 0
#define DMR818_NOISELEV_N 1
#define DMR818_NOISELEV_ENHANCE 2
//tone type
#define DMR818_TONETYPE_CARRIER 1
#define DMR818_TONETYPE_CTCSS 2
#define DMR818_TONETYPE_CDCSS 3
#define DMR818_TONETYPE_RE_CDCSS 4
//tone freq //adc
#define DMR818_CTCSS_T_FREQ_67 1
//tx power
#define DMR818_TX_POWER_HIGH 0x01
#define DMR818_TX_POWER_LOW 0xFF
/* Exported constants --------------------------------------------------------*/
//user config info
#define DMR818_UART_NUM UART_NUM_3 
#define DMR818_DEF_CH 9
#define DMR818_DEF_MIC 0
#define DMR818_DEF_VOL 9
#define DMR818_DEF_NOISELEV DMR818_NOISELEV_NO
#define DMR818_DEF_TONETYPE DMR818_TONETYPE_CTCSS
#define DMR818_DEF_TONEFREQ DMR818_CTCSS_T_FREQ_67
#define DMR818_DEF_TXPOWER DMR818_TX_POWER_LOW
/* Exported types ------------------------------------------------------------*/
//对讲机配置体
typedef struct
{
#define DMR_PTT_SEND 0
#define DMR_PTT_RECV 1
    uint8_t run_channel; //对讲机通道
    uint8_t ptt; //send or recv state
} dmr818_config_st;
/* Exported functions ------------------------------------------------------- */
void dmr818_init(void);
void dmr818_prcs(void);
//mix cmd send
char dmr818_runtime_change_channel(uint8_t ch);
//cmd send
char dmr818cmd_change_channel(unsigned char ch);
char dmr818cmd_set_vol(unsigned char vol);
char dmr818_set_mic(unsigned char mic);
char dmr818_set_feq(uint32_t recv_feq, uint32_t send_feq);
char dmr818_set_feq_by_ch(unsigned char ch);
char dmr818_noise_level(unsigned char level);
char dmr818_set_tone_type(uint8_t recv_tt, uint8_t send_tt);
char dmr818_set_tone_freq(uint8_t recv_tf, uint8_t send_tf);
char dmr818_set_tx_power(uint8_t power);
//else
char dmr818_interphone_send_or_recv(uint8_t state);
extern dmr818_config_st dmr818_config;
#endif
