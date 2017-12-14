#include "wavplay.h" 
#include "audioplay.h"
#include "usart.h" 
#include "malloc.h"
#include "ff.h"
#include "i2s.h"
#include "wm8978.h"
#include "key.h"
#include "led.h"
#include "appfiles.h"
#include "appaudio.h"
 
//WAV������ʼ��
//fname:�ļ�·��+�ļ���
//wavx:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��;2,��WAV�ļ�;3,DATA����δ�ҵ�.
u8 wav_decode_init(u8* fname,__wavctrl* wavx)
{
	FIL _ftemp;
	u8 _buf[512];
	
	FIL*ftemp = &_ftemp;
	u8 *buf = _buf; 
	u32 br=0;
	u8 res=0;
	
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;
	
	if(ftemp&&buf)	//�ڴ�����ɹ�
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	            //��ȡ512�ֽ�������
			riff=(ChunkRIFF *)buf;		              //��ȡRIFF��
			if(riff->Format==0X45564157)            //��WAV�ļ�
			{
				fmt=(ChunkFMT *)(buf+12);	            //��ȡFMT�� 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//��ȡFACT��
				if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)
					wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//����fact/LIST���ʱ��(δ����)
				else 
					wavx->datastart=12+8+fmt->ChunkSize;  
				data=(ChunkDATA *)(buf+wavx->datastart);	//��ȡDATA��
				if(data->ChunkID==0X61746164)//�����ɹ�!
				{
					wavx->audioformat=fmt->AudioFormat;		//��Ƶ��ʽ
					wavx->nchannels=fmt->NumOfChannels;		//ͨ����
					wavx->samplerate=fmt->SampleRate;		//������
					wavx->bitrate=fmt->ByteRate*8;			//�õ�λ��
					wavx->blockalign=fmt->BlockAlign;		//�����
					wavx->bps=fmt->BitsPerSample;			//λ��,16/24/32λ
					
					wavx->datasize=data->ChunkSize;			//���ݿ��С
					wavx->datastart=wavx->datastart+8;		//��������ʼ�ĵط�. 
//					printf("--------\n");
//					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
//					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
//					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
//					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
//					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
//					printf("wavx->bps:%d\r\n",wavx->bps);
//					printf("wavx->datasize:%d\r\n",wavx->datasize);
//					printf("wavx->datastart:%d\r\n",wavx->datastart);  
				}
				else 
					res=3;//data����δ�ҵ�.
			}
			else 
				res=2;//��wav�ļ�
		}
		else 
			res=1;//���ļ�����
	}
	f_close(ftemp);
	return res;
}
//���buf
//buf:������
//size:���������
//bits:λ��(16/24)
//����ֵ:���������ݸ���
u32 wav_buffill(void *fil, u8 *buf, u8 *tbuf, u16 size, u8 bits)
{
	FIL* file=(FIL*)fil;
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
	if(bits==24)//24bit��Ƶ,��Ҫ����һ��
	{
		readlen=(size/4)*3;							            //�˴�Ҫ��ȡ���ֽ���
		f_read(file, tbuf,readlen, (UINT*)&bread);	//��ȡ����
		p=tbuf;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		}
		bread=(bread*4)/3;		//����Ĵ�С.
	}
	else 
	{
		f_read(file, buf, size, (UINT*)&bread);//16bit��Ƶ,ֱ�Ӷ�ȡ����  
		if(bread<size)//����������,����0
		{
			for(i=bread;i<size-bread;i++)
				buf[i]=0; 
		}
	}
	return bread;
}
//WAV����ʱ,I2S DMA����ص�����
void wav_i2s_dma_tx_callback(void) 
{
	u16 i;
	if(DMA1_Stream4->CR&(1<<19))
	{
		_audio_handlest.waviis.wavwitchbuf =0;
		if((_audio_handlest.play_state!=PLAYING_S_APY) || (_audio_handlest.play_state!=START_S_APY)) //����������Ƶʱ�����������жϺ�ֱ����䲥�ŵ���Ƶ����Ϊ0
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				_audio_handlest.waviis.i2sbuf1[i]=0;//���0
			}
		}
	}
	else 
	{
		_audio_handlest.waviis.wavwitchbuf =1;
		if((_audio_handlest.play_state!=PLAYING_S_APY) || (_audio_handlest.play_state!=START_S_APY))
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				_audio_handlest.waviis.i2sbuf2[i]=0;//���0
			}
		}
	}
	_audio_handlest.waviis.wavtransferend =1;
}
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//wavx:wav���ſ�����
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;  	
 	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	//�����ܳ���(��λ:��) 
	fpos=fx->fptr-wavx->datastart; 					//�õ���ǰ�ļ����ŵ��ĵط� 
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	//��ǰ���ŵ��ڶ�������?	
}
////����ĳ��WAV�ļ�
////fname:wav�ļ�·��.
////����ֵ:
////KEY0_PRES:��һ��
////KEY1_PRES:��һ��
////����:����
//u8 wav_play_song(u8* fname)
//{
//	u8 key;
//	u8 t=0; 
//	u8 res;  
//	u32 fillnum; 
//	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
//	audiodev.i2sbuf1=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
//	audiodev.i2sbuf2=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
//	audiodev.tbuf=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
//	if(audiodev.file&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&audiodev.tbuf)
//	{ 
//		res=wav_decode_init(fname,&wavctrl);//�õ��ļ�����Ϣ
//		if(res==0)//�����ļ��ɹ�
//		{
//			if(wavctrl.bps==16)
//			{
//				WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
//				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡����
//			}else if(wavctrl.bps==24)
//			{
//				WM8978_I2S_Cfg(2,2);	//�����ֱ�׼,24λ���ݳ���
//				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
//			}
//			I2S2_SampleRate_Set(wavctrl.samplerate);//���ò�����
//			I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2); //����TX DMA
//			i2s_tx_callback=wav_i2s_dma_tx_callback;			//�ص�����ָwav_i2s_dma_callback
//			audio_stop();
//			res=f_open(audiodev.file,(TCHAR*)fname,FA_READ);	//���ļ�
//			if(res==0)
//			{
//				f_lseek(audiodev.file, wavctrl.datastart);		//�����ļ�ͷ
//				fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
//				fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
//				audio_start();  
//				while(res==0)
//				{ 
//					while(wavtransferend==0);//�ȴ�wav�������; 
//					wavtransferend=0;
//					if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)//���Ž���?
//					{
//						res=KEY_REC_PAUSE;
//						break;
//					}
// 					if(wavwitchbuf)fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf2
//					else fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf1
//					while(1)
//					{
//						key=which_key_is_pressed(); 
//						if(key==KEY_WKUP)//��ͣ
//						{
//							if(audiodev.status&0X01)audiodev.status&=~(1<<0);
//							else audiodev.status|=0X01;  
//						}
//						if(key==KEY_STOP_SAVE||key==KEY_REC_PAUSE)//��һ��/��һ��
//						{
//							res=key;
//							break; 
//						}
//						wav_get_curtime(audiodev.file,&wavctrl);//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
//						audio_msg_show(wavctrl.totsec,wavctrl.cursec,wavctrl.bitrate);
//						t++;
//						if(t==20)
//						{
//							t=0;
////							LED0=!LED0;
//						}
//						if((audiodev.status&0X01)==0)delay_ms(10);
//						else break;
//					}
//				}
//				audio_stop(); 
//			}else res=0XFF; 
//		}else res=0XFF;
//	}else res=0XFF; 
//	myfree(SRAMIN,audiodev.tbuf);	//�ͷ��ڴ�
//	myfree(SRAMIN,audiodev.i2sbuf1);//�ͷ��ڴ�
//	myfree(SRAMIN,audiodev.i2sbuf2);//�ͷ��ڴ� 
//	myfree(SRAMIN,audiodev.file);	//�ͷ��ڴ� 
//	return res;
//} 
/**
  * @brief  wav_play_start
  * @param  
  * @note   
  * @retval
  */
bool wav_play_start(appaudio_st* audio)
{
	u8 res;  
	u32 fillnum; 
	__wavctrl *wavctrl = &audio->wavctrl;
	wavplay_iis_st *waviis = &audio->waviis;
	FIL *file = &audio->play_fil;
	char *fname = audio->play_objname;
	audioPlayState *play_state = &audio->play_state;
	
	if(wavctrl->bps==16)
	{
		WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡����
	}
	else if(wavctrl->bps==24)
	{
		WM8978_I2S_Cfg(2,2);	//�����ֱ�׼,24λ���ݳ���
		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
	}
	else
	{
		printf("audio bps not support\n");
		return false;
	}
	I2S2_SampleRate_Set(wavctrl->samplerate);//���ò�����
	I2S2_TX_DMA_Init(waviis->i2sbuf1, waviis->i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE/2);           //����TX DMA
	i2s_tx_callback = wav_i2s_dma_tx_callback;			                                        //�ص�����ָwav_i2s_dma_callback
	I2S_Play_Stop();
	*play_state = PAUSE_S_APY;  //set state
  res=f_open(file, (TCHAR*)fname, FA_READ);	//���ļ� //need to close file  //zkrt_notice
	if(res!=FR_OK)
	{
		//zkrt_todo
		printf("audio file open fail\n");
		*play_state = START_S_APY;
		return false;
	}	
	f_lseek(audiodev.file, wavctrl->datastart);		//�����ļ�ͷ
	fillnum=wav_buffill(file, waviis->i2sbuf1, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //ȡ����
	if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)
	{
		printf("audio file read too few\n");
		*play_state = START_S_APY;
		f_close(file);
		return false;
	}
	fillnum=wav_buffill(file, waviis->i2sbuf2, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //ȡ����
	if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)
	{
		printf("audio file read too few\n");
		*play_state = START_S_APY;
		f_close(file);
		return false;
	}
	I2S_Play_Start();
	*play_state = START_S_APY;  //set state
	return true;
}
/**
  * @brief  wav_playing
  * @param  
  * @note   
  * @retval
  */
u8 wav_playing(appaudio_st* audio)
{
	u32 fillnum;  
	u8 play_state = PLAYING_S_APY;
	vu8 *iis_transed = &audio->waviis.wavtransferend;
	vu8 *buffindex = &audio->waviis.wavwitchbuf;
	__wavctrl *wavctrl = &audio->wavctrl;
	wavplay_iis_st *waviis = &audio->waviis;
	FIL *file = &audio->play_fil;
	
	//�ж�iis���жϷ������
	if(*iis_transed ==1)
	{
		*iis_transed = 0;
		
		if(*buffindex)
			fillnum=wav_buffill(file, waviis->i2sbuf2, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //ȡ����
		else 
			fillnum=wav_buffill(file, waviis->i2sbuf1, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //ȡ����
		
		//�ж��Ƿ������ݷ���
		if(fillnum ==0) //�������
		{
			play_state = OVER_S_APY;
		}
	}
	return play_state;
}
/**
  * @brief  wav_play_over
  * @param  
  * @note   
  * @retval
  */
void wav_play_over(appaudio_st* audio)
{
	//�رղ���
	I2S_Play_Stop();
	f_close(&audio->play_fil);
	audio->waviis.wavtransferend = 0;
	
	switch(audio->play_mode)	//�жϵ�ǰ����ģʽ������ͬ�Ĳ���
	{
		case NORMAL_MAPY:
			//close playing
		  audio->play_state = IDLE_S_APY;
		  audio->play_id = AUDIOID_NONE;
		  audio->play_item = NULL;
			break;

		case SINGLELOOP_MAPY:
			audio->play_state = START_S_APY;
			break;


		case LISTLOOP_MAPY:
			audio->play_state = START_S_APY;
		  audio->play_id = audio_item_nextid_loop(audio->play_id);
		  audio->play_item = audio_item_get(audio->play_id);
			break;	

    default:
			//close playing
		  audio->play_state = IDLE_S_APY;
		  audio->play_id = AUDIOID_NONE;
		  audio->play_item = NULL;
			break;
	}
}
/**
  * @brief  wav_play_stop
  * @param  
  * @note   
  * @retval
  */
void wav_play_stop(appaudio_st* audio)
{
	wav_play_over(audio);
	audio->play_state = IDLE_S_APY;
	audio->play_id = AUDIOID_NONE;
	audio->play_item = NULL;
}
/**
  * @brief  wav_play_pause
  * @param  
  * @note   
  * @retval
  */
void wav_play_pause(appaudio_st* audio)
{
	//zkrt_todo://���Թر�IIS����жϣ��ж����ж�״̬�����Ч�������ݣ��ر�WM8978 DAC, �ر�WM8978 IIS ���
	//zkrt_test:��ʱ����ͨ�����˴������κδ��룬��ͣ�������ж����ж�״̬�����Ч��������
}
/**
  * @brief  wav_play_continue
  * @param  
  * @note   
  * @retval
  */
void wav_play_continue(appaudio_st* audio)
{
	//zkrt_todo:
	//���������wav_play_pause���Ӧ
	//zkrt_test:��ʱ����ͨ�����˴������κδ��롣
}

