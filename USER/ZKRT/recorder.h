#ifndef __RECORDER_H
#define __RECORDER_H
#include "sys.h"
#include "ff.h"
#include "wavplay.h" 

#define I2S_RX_DMA_BUF_SIZE    	4096		//定义RX DMA 数组大小

void rec_i2s_dma_rx_callback(void);
void recoder_wav_init(__WaveHeader* wavhead);

void recode_init(void);
void recode_start_handle(void);
void recoding_handle(void);
void recode_stop_handle(void);
void recode_over_handle(void);

void audio_enter_rec_mode(void);
void audio_enter_play_mode(void);
void audio_enter_speak_mode(void);
void audio_workmode_adjust(void);
#endif












