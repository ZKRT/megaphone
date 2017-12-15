#include "recorder.h" 
#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "key.h"
#include "exfuns.h"  
#include "string.h"  
#include "appaudio.h"

audiorec_st _audiorec;
extern audioplay_st _audioplay;
const u16 i2splaybuf[2]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.

////////////////////////////////////////////////////////////////////////////////////iis tx IRQ hander function
//录音 I2S_DMA接收中断服务函数.在中断里面写入数据
void rec_i2s_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	if((_audiorec.rec_state==START_S_REC)||(_audiorec.rec_state==RECORDING_S_REC))
	{
		if(DMA1_Stream3->CR&(1<<19))
		{
			res=f_write(&_audiorec.rec_fil, (void*)_audiorec.i2srecbuf1, I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(res)
			{
				//zkrt_todo: 是否读写在main函数处理 是否错误时置录音状态为失败，或者重新开始录音？并删除旧的录音资料
				printf("write error:%d\r\n",res);
			}
		}
		else 
		{
			res=f_write(&_audiorec.rec_fil, (void*)_audiorec.i2srecbuf2, I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		_audiorec.wavsize+=I2S_RX_DMA_BUF_SIZE;
	}
}  
/////////////////////////////////////////////////////////////////////////////////////////wm8978 handle and iis configure function
//进入PCM 录音模式 		  
void audio_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//开启ADC
	WM8978_Input_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
	if(_audiorec.rec_out_flag == REC_FLAG_OUTEN)
		WM8978_Output_Cfg(0, 1);		//开启BYPASS输出 
	else
		WM8978_Output_Cfg(0, 0);		//开启BYPASS输出 
	WM8978_MIC_Gain(46);		//MIC增益设置 
	
	WM8978_I2S_Cfg(2,0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
	I2S2_SampleRate_Set(16000);	//设置采样率 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//配置TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
	I2S2ext_RX_DMA_Init((u8*)_audiorec.i2srecbuf1, (u8*)_audiorec.i2srecbuf2, I2S_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
  i2s_rx_callback=rec_i2s_dma_rx_callback;//回调函数指wav_i2s_dma_callback
 	I2S_Play_Start();	//开始I2S数据发送(主机)
	I2S_Rec_Start(); 	//开始I2S数据接收(从机)
}  
//进入PCM 放音模式 		  
void audio_enter_play_mode(void)
{
	WM8978_ADDA_Cfg(1,0);		//开启DAC 
  if(_audioplay.out_flag ==REC_FLAG_OUTEN)
	{
		WM8978_Input_Cfg(1,1,0);	//关闭输入通道(MIC&LINE IN)
		WM8978_Output_Cfg(1,1);		//开启DAC输出
		WM8978_MIC_Gain(46);			//MIC增益设置为0 				
	}
	else
	{
		WM8978_Input_Cfg(0,0,0);	//关闭输入通道(MIC&LINE IN)
		WM8978_Output_Cfg(1,0);		//开启DAC输出
		WM8978_MIC_Gain(0);			//MIC增益设置为0 		
	}
	I2S_Play_Stop();			//停止时钟发送
	I2S_Rec_Stop(); 			//停止录音
}
//进入对讲模式
void audio_enter_speak_mode(void)
{
	WM8978_ADDA_Cfg(0, 0);		 //close adc and dac 
	WM8978_Input_Cfg(1,1,0);	 //(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		 //bypass
	WM8978_MIC_Gain(46);			   //MIC增益设置为0 
	I2S_Play_Stop();			     //停止时钟发送
	I2S_Rec_Stop(); 			     //停止录音
}
//音频工作模式调整（录音，播放，对讲）
void audio_workmode_adjust(void)
{
	//zkrt_todo
	audio_enter_speak_mode();
}
/////////////////////////////////////////////////////////////////////////////wav file function
//初始化WAV头.
void recoder_wav_init(__WaveHeader* wavhead) //初始化WAV头			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定,最后需要计算
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16个字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
 	wavhead->fmt.NumOfChannels=2;		//双声道
 	wavhead->fmt.SampleRate=16000;		//16Khz采样率 采样速率
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*4;//字节速率=采样率*通道数*(ADC位数/8)
 	wavhead->fmt.BlockAlign=4;			//块大小=通道数*(ADC位数/8)
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
  wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小,还需要计算  
}
/////////////////////////////////////////////////////////////////////////////////////////recode handle function
/**
  * @brief  recode_init
  * @param  
  * @note   
  * @retval
  */
void recode_init(void)
{
	
}
/**
  * @brief  recode_start
  * @param  
  * @note   
  * @retval
  */
void recode_start_handle(void)
{
	audio_enter_rec_mode();
	_audiorec.rec_state = RECORDING_S_REC;	
}
/**
  * @brief  recoding_start
  * @param  
  * @note   
  * @retval
  */
void recoding_handle(void)
{
	
}
/**
  * @brief  recode_stop
  * @param  
  * @note   
  * @retval
  */
void recode_stop_handle(void)
{
	_audiorec.rec_state = OVER_S_REC;	
}
/**
  * @brief  recode_over
  * @param  
  * @note   
  * @retval
  */
void recode_over_handle(void)
{
	_audiorec.rec_state = IDLE_S_REC;	
}
/**
  * @brief  recode_pause_handle
  * @param  
  * @note   
  * @retval
  */
void recode_pause_handle(void)
{
}
/**
  * @brief  recode_continue_handle
  * @param  
  * @note   
  * @retval
  */
void recode_continue_handle(void)
{
	audio_enter_rec_mode();
	_audiorec.rec_state = RECORDING_S_REC;	
}

