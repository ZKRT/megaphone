#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H
#include "sys.h"
#include "appaudio.h"

#define I2S_TX_DMA_BUF_SIZE    8192		//定义TX DMA 数组大小(播放192Kbps@24bit的时候,需要设置8192大才不会卡)

void audiocommon_play_over_check_mode(appaudio_st* audio);
void audiocommon_play_over(void);
void audiocommon_play_stop(void);
void audiocommon_play_pause(void);
void audiocommon_play_continue(void);

#endif
