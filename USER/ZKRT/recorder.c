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
const u16 i2splaybuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.

////////////////////////////////////////////////////////////////////////////////////iis tx IRQ hander function
//¼�� I2S_DMA�����жϷ�����.���ж�����д������
void rec_i2s_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	if((_audiorec.rec_state==START_S_REC)||(_audiorec.rec_state==RECORDING_S_REC))
	{
		if(DMA1_Stream3->CR&(1<<19))
		{
			res=f_write(&_audiorec.rec_fil, (void*)_audiorec.i2srecbuf1, I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
			if(res)
			{
				//zkrt_todo: �Ƿ��д��main�������� �Ƿ����ʱ��¼��״̬Ϊʧ�ܣ��������¿�ʼ¼������ɾ���ɵ�¼������
				printf("write error:%d\r\n",res);
			}
		}
		else 
		{
			res=f_write(&_audiorec.rec_fil, (void*)_audiorec.i2srecbuf2, I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		_audiorec.wavsize+=I2S_RX_DMA_BUF_SIZE;
	}
}  
/////////////////////////////////////////////////////////////////////////////////////////wm8978 handle and iis configure function
//����PCM ¼��ģʽ 		  
void audio_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//����ADC
	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	if(_audiorec.rec_out_flag == REC_FLAG_OUTEN)
		WM8978_Output_Cfg(0, 1);		//����BYPASS��� 
	else
		WM8978_Output_Cfg(0, 0);		//����BYPASS��� 
	WM8978_MIC_Gain(46);		//MIC�������� 
	
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	I2S2_SampleRate_Set(16000);	//���ò����� 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//����TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S2ext_RX_DMA_Init((u8*)_audiorec.i2srecbuf1, (u8*)_audiorec.i2srecbuf2, I2S_RX_DMA_BUF_SIZE/2); 	//����RX DMA
  i2s_rx_callback=rec_i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
}  
//����PCM ����ģʽ 		  
void audio_enter_play_mode(void)
{
	WM8978_ADDA_Cfg(1,0);		//����DAC 
  if(_audioplay.out_flag ==REC_FLAG_OUTEN)
	{
		WM8978_Input_Cfg(1,1,0);	//�ر�����ͨ��(MIC&LINE IN)
		WM8978_Output_Cfg(1,1);		//����DAC���
		WM8978_MIC_Gain(46);			//MIC��������Ϊ0 				
	}
	else
	{
		WM8978_Input_Cfg(0,0,0);	//�ر�����ͨ��(MIC&LINE IN)
		WM8978_Output_Cfg(1,0);		//����DAC���
		WM8978_MIC_Gain(0);			//MIC��������Ϊ0 		
	}
	I2S_Play_Stop();			//ֹͣʱ�ӷ���
	I2S_Rec_Stop(); 			//ֹͣ¼��
}
//����Խ�ģʽ
void audio_enter_speak_mode(void)
{
	WM8978_ADDA_Cfg(0, 0);		 //close adc and dac 
	WM8978_Input_Cfg(1,1,0);	 //(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		 //bypass
	WM8978_MIC_Gain(46);			   //MIC��������Ϊ0 
	I2S_Play_Stop();			     //ֹͣʱ�ӷ���
	I2S_Rec_Stop(); 			     //ֹͣ¼��
}
//��Ƶ����ģʽ������¼�������ţ��Խ���
void audio_workmode_adjust(void)
{
	//zkrt_todo
	audio_enter_speak_mode();
}
/////////////////////////////////////////////////////////////////////////////wav file function
//��ʼ��WAVͷ.
void recoder_wav_init(__WaveHeader* wavhead) //��ʼ��WAVͷ			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
 	wavhead->fmt.NumOfChannels=2;		//˫����
 	wavhead->fmt.SampleRate=16000;		//16Khz������ ��������
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*4;//�ֽ�����=������*ͨ����*(ADCλ��/8)
 	wavhead->fmt.BlockAlign=4;			//���С=ͨ����*(ADCλ��/8)
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
  wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
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

