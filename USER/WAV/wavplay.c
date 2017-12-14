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
 
//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
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
	
	if(ftemp&&buf)	//内存申请成功
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//打开文件
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	            //读取512字节在数据
			riff=(ChunkRIFF *)buf;		              //获取RIFF块
			if(riff->Format==0X45564157)            //是WAV文件
			{
				fmt=(ChunkFMT *)(buf+12);	            //获取FMT块 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//读取FACT块
				if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)
					wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//具有fact/LIST块的时候(未测试)
				else 
					wavx->datastart=12+8+fmt->ChunkSize;  
				data=(ChunkDATA *)(buf+wavx->datastart);	//读取DATA块
				if(data->ChunkID==0X61746164)//解析成功!
				{
					wavx->audioformat=fmt->AudioFormat;		//音频格式
					wavx->nchannels=fmt->NumOfChannels;		//通道数
					wavx->samplerate=fmt->SampleRate;		//采样率
					wavx->bitrate=fmt->ByteRate*8;			//得到位速
					wavx->blockalign=fmt->BlockAlign;		//块对齐
					wavx->bps=fmt->BitsPerSample;			//位数,16/24/32位
					
					wavx->datasize=data->ChunkSize;			//数据块大小
					wavx->datastart=wavx->datastart+8;		//数据流开始的地方. 
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
					res=3;//data区域未找到.
			}
			else 
				res=2;//非wav文件
		}
		else 
			res=1;//打开文件错误
	}
	f_close(ftemp);
	return res;
}
//填充buf
//buf:数据区
//size:填充数据量
//bits:位数(16/24)
//返回值:读到的数据个数
u32 wav_buffill(void *fil, u8 *buf, u8 *tbuf, u16 size, u8 bits)
{
	FIL* file=(FIL*)fil;
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
	if(bits==24)//24bit音频,需要处理一下
	{
		readlen=(size/4)*3;							            //此次要读取的字节数
		f_read(file, tbuf,readlen, (UINT*)&bread);	//读取数据
		p=tbuf;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		}
		bread=(bread*4)/3;		//填充后的大小.
	}
	else 
	{
		f_read(file, buf, size, (UINT*)&bread);//16bit音频,直接读取数据  
		if(bread<size)//不够数据了,补充0
		{
			for(i=bread;i<size-bread;i++)
				buf[i]=0; 
		}
	}
	return bread;
}
//WAV播放时,I2S DMA传输回调函数
void wav_i2s_dma_tx_callback(void) 
{
	u16 i;
	if(DMA1_Stream4->CR&(1<<19))
	{
		_audio_handlest.waviis.wavwitchbuf =0;
		if((_audio_handlest.play_state!=PLAYING_S_APY) || (_audio_handlest.play_state!=START_S_APY)) //当不播放音频时，跳进传输中断后，直接填充播放的音频数据为0
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				_audio_handlest.waviis.i2sbuf1[i]=0;//填充0
			}
		}
	}
	else 
	{
		_audio_handlest.waviis.wavwitchbuf =1;
		if((_audio_handlest.play_state!=PLAYING_S_APY) || (_audio_handlest.play_state!=START_S_APY))
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				_audio_handlest.waviis.i2sbuf2[i]=0;//填充0
			}
		}
	}
	_audio_handlest.waviis.wavtransferend =1;
}
//得到当前播放时间
//fx:文件指针
//wavx:wav播放控制器
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;  	
 	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	//歌曲总长度(单位:秒) 
	fpos=fx->fptr-wavx->datastart; 					//得到当前文件播放到的地方 
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	//当前播放到第多少秒了?	
}
////播放某个WAV文件
////fname:wav文件路径.
////返回值:
////KEY0_PRES:下一曲
////KEY1_PRES:上一曲
////其他:错误
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
//		res=wav_decode_init(fname,&wavctrl);//得到文件的信息
//		if(res==0)//解析文件成功
//		{
//			if(wavctrl.bps==16)
//			{
//				WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
//				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
//			}else if(wavctrl.bps==24)
//			{
//				WM8978_I2S_Cfg(2,2);	//飞利浦标准,24位数据长度
//				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
//			}
//			I2S2_SampleRate_Set(wavctrl.samplerate);//设置采样率
//			I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2); //配置TX DMA
//			i2s_tx_callback=wav_i2s_dma_tx_callback;			//回调函数指wav_i2s_dma_callback
//			audio_stop();
//			res=f_open(audiodev.file,(TCHAR*)fname,FA_READ);	//打开文件
//			if(res==0)
//			{
//				f_lseek(audiodev.file, wavctrl.datastart);		//跳过文件头
//				fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
//				fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
//				audio_start();  
//				while(res==0)
//				{ 
//					while(wavtransferend==0);//等待wav传输完成; 
//					wavtransferend=0;
//					if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)//播放结束?
//					{
//						res=KEY_REC_PAUSE;
//						break;
//					}
// 					if(wavwitchbuf)fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//填充buf2
//					else fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//填充buf1
//					while(1)
//					{
//						key=which_key_is_pressed(); 
//						if(key==KEY_WKUP)//暂停
//						{
//							if(audiodev.status&0X01)audiodev.status&=~(1<<0);
//							else audiodev.status|=0X01;  
//						}
//						if(key==KEY_STOP_SAVE||key==KEY_REC_PAUSE)//下一曲/上一曲
//						{
//							res=key;
//							break; 
//						}
//						wav_get_curtime(audiodev.file,&wavctrl);//得到总时间和当前播放的时间 
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
//	myfree(SRAMIN,audiodev.tbuf);	//释放内存
//	myfree(SRAMIN,audiodev.i2sbuf1);//释放内存
//	myfree(SRAMIN,audiodev.i2sbuf2);//释放内存 
//	myfree(SRAMIN,audiodev.file);	//释放内存 
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
		WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
	}
	else if(wavctrl->bps==24)
	{
		WM8978_I2S_Cfg(2,2);	//飞利浦标准,24位数据长度
		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
	}
	else
	{
		printf("audio bps not support\n");
		return false;
	}
	I2S2_SampleRate_Set(wavctrl->samplerate);//设置采样率
	I2S2_TX_DMA_Init(waviis->i2sbuf1, waviis->i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE/2);           //配置TX DMA
	i2s_tx_callback = wav_i2s_dma_tx_callback;			                                        //回调函数指wav_i2s_dma_callback
	I2S_Play_Stop();
	*play_state = PAUSE_S_APY;  //set state
  res=f_open(file, (TCHAR*)fname, FA_READ);	//打开文件 //need to close file  //zkrt_notice
	if(res!=FR_OK)
	{
		//zkrt_todo
		printf("audio file open fail\n");
		*play_state = START_S_APY;
		return false;
	}	
	f_lseek(audiodev.file, wavctrl->datastart);		//跳过文件头
	fillnum=wav_buffill(file, waviis->i2sbuf1, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
	if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)
	{
		printf("audio file read too few\n");
		*play_state = START_S_APY;
		f_close(file);
		return false;
	}
	fillnum=wav_buffill(file, waviis->i2sbuf2, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
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
	
	//判断iis的中断发送完成
	if(*iis_transed ==1)
	{
		*iis_transed = 0;
		
		if(*buffindex)
			fillnum=wav_buffill(file, waviis->i2sbuf2, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
		else 
			fillnum=wav_buffill(file, waviis->i2sbuf1, waviis->tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
		
		//判断是否有数据发送
		if(fillnum ==0) //播放完成
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
	//关闭播放
	I2S_Play_Stop();
	f_close(&audio->play_fil);
	audio->waviis.wavtransferend = 0;
	
	switch(audio->play_mode)	//判断当前播放模式，做不同的操作
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
	//zkrt_todo://尝试关闭IIS输出中断，中断里判断状态填充无效发送数据，关闭WM8978 DAC, 关闭WM8978 IIS 输出
	//zkrt_test:暂时测试通过。此处不加任何代码，暂停处理在中断里判断状态填充无效发送数据
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
	//具体操作与wav_play_pause相对应
	//zkrt_test:暂时测试通过。此处不加任何代码。
}

