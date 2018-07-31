#include "wavplay.h"
#include "audioplay.h"
#include "usart.h"
#include "i2s.h"
#include "wm8978.h"
#include "appfiles.h"
#include "appaudio.h"

extern audioplay_st _audioplay;

//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
u8 wav_decode_init(u8* fname, __wavctrl* wavx) {
	FIL _ftemp;
	u8 _buf[512] = {0};

	FIL*ftemp = &_ftemp;
	u8 *buf = _buf;
	u32 br = 0;
	u8 res = 0;

	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;

	if (ftemp && buf) {	//内存申请成功
		res = f_open(ftemp, (TCHAR*)fname, FA_READ); //打开文件
		if (res == FR_OK) {
			f_read(ftemp, buf, 512, &br);	         //读取512字节在数据
			riff = (ChunkRIFF *)buf;		            //获取RIFF块
			if (riff->Format == 0X45564157) {       //是WAV文件
				fmt = (ChunkFMT *)(buf + 12);	        //获取FMT块
				fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize); //读取FACT块
				if (fact->ChunkID == 0X74636166 || fact->ChunkID == 0X5453494C)
					wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize; //具有fact/LIST块的时候(未测试)
				else
					wavx->datastart = 12 + 8 + fmt->ChunkSize;
				data = (ChunkDATA *)(buf + wavx->datastart);	//读取DATA块
				if (data->ChunkID == 0X61746164) { //解析成功!
					wavx->audioformat = fmt->AudioFormat;		//音频格式
					wavx->nchannels = fmt->NumOfChannels;		//通道数
					wavx->samplerate = fmt->SampleRate;		//采样率
					wavx->bitrate = fmt->ByteRate * 8;			//得到位速
					wavx->blockalign = fmt->BlockAlign;		//块对齐
					wavx->bps = fmt->BitsPerSample;			//位数,16/24/32位

					wavx->datasize = data->ChunkSize;			//数据块大小
					wavx->datastart = wavx->datastart + 8;		//数据流开始的地方.
//					printf("--------\n");
//					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
//					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
//					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
//					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
//					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
//					printf("wavx->bps:%d\r\n",wavx->bps);
//					printf("wavx->datasize:%d\r\n",wavx->datasize);
//					printf("wavx->datastart:%d\r\n",wavx->datastart);
				} else
					res = 3; //data区域未找到.
			} else
				res = 2; //非wav文件
		} else
			res = 1; //打开文件错误
	}
	f_close(ftemp);
	return res;
}
//填充buf
//buf:数据区
//size:填充数据量
//bits:位数(16/24)
//返回值:读到的数据个数
u32 wav_buffill(void *fil, u8 *buf, u8 *tbuf, u16 size, u8 bits) {
	FIL* file = (FIL*)fil;
	u16 readlen = 0;
	u32 bread;
	u16 i;
	u8 *p;
	if (bits == 24) { //24bit音频,需要处理一下
		readlen = (size / 4) * 3;							      //此次要读取的字节数
		f_read(file, tbuf, readlen, (UINT*)&bread);	//读取数据
		p = tbuf;
		for (i = 0; i < size;) {
			buf[i++] = p[1];
			buf[i] = p[2];
			i += 2;
			buf[i++] = p[0];
			p += 3;
		}
		bread = (bread * 4) / 3;		//填充后的大小.
	} else {
		f_read(file, buf, size, (UINT*)&bread);//16bit音频,直接读取数据
		if (bread < size) { //不够数据了,补充0
			for (i = bread; i < size - bread; i++)
				buf[i] = 0;
		}
	}
	return bread;
}
//WAV播放时,I2S DMA传输回调函数
void wav_i2s_dma_tx_callback(void) {
	u16 i;
	if (DMA1_Stream4->CR & (1 << 19)) {
		_audioplay.wavwitchbuf = 0;
		if ((_audioplay.play_state != PLAYING_S_APY) || (_audioplay.play_state != START_S_APY)) { //当不播放音频时，跳进传输中断后，直接填充播放的音频数据为0
			for (i = 0; i < WAV_I2S_TX_DMA_BUFSIZE; i++) { //暂停
				_audioplay.i2sbuf1[i] = 0; //填充0
			}
		}
	} else {
		_audioplay.wavwitchbuf = 1;
		if ((_audioplay.play_state != PLAYING_S_APY) || (_audioplay.play_state != START_S_APY)) {
			for (i = 0; i < WAV_I2S_TX_DMA_BUFSIZE; i++) { //暂停
				_audioplay.i2sbuf2[i] = 0; //填充0
			}
		}
	}
	_audioplay.wavtransferend = 1;
}
//得到当前播放时间
//fx:文件指针
//wavx:wav播放控制器
void wav_get_curtime(FIL*fx, __wavctrl *wavx) {
	long long fpos;
	wavx->totsec = wavx->datasize / (wavx->bitrate / 8);	//歌曲总长度(单位:秒)
	fpos = fx->fptr - wavx->datastart; 					//得到当前文件播放到的地方
	wavx->cursec = fpos * wavx->totsec / wavx->datasize;	//当前播放到第多少秒了?
}
///////////////////////////////////////////////////////////////////////////////////////////////////wav play function
/**
  * @brief  wav_play_init
  * @param
  * @note
  * @retval
  */
void wav_play_init(void) {

}

/**
  * @brief  wav_play_start
  * @param
  * @note
  * @retval
  */
bool wav_play_start(void) {
	u8 res;
	u32 fillnum;
	__wavctrl *wavctrl = &_audioplay.wavctrl;
	FIL *file = &_audioplay.play_fil;
	char *fname = _audioplay.play_objname;
	audioPlayState *play_state = &_audioplay.play_state;

	if (wavctrl->bps == 16) {
		WM8978_I2S_Cfg(2, 0);	//飞利浦标准,16位数据长度
		I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
	} else if (wavctrl->bps == 24) {
		WM8978_I2S_Cfg(2, 2);	//飞利浦标准,24位数据长度
		I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_24b);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
	} else {
		printf("audio bps not support\n");
		return false;
	}
	I2S2_SampleRate_Set(wavctrl->samplerate);//设置采样率
	I2S2_TX_DMA_Init(_audioplay.i2sbuf1, _audioplay.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE / 2);         //配置TX DMA
	i2s_tx_callback = wav_i2s_dma_tx_callback;			                                              //回调函数指wav_i2s_dma_callback
	I2S_Play_Stop();
	*play_state = PAUSE_S_APY;  //set state
	res = f_open(file, (TCHAR*)fname, FA_READ);	//打开文件 //need to close file  //zkrt_notice
	if (res != FR_OK) {
		//zkrt_todo
		printf("audio file open fail\n");
		*play_state = STOP_S_APY;
		return false;
	}
	f_lseek(file, wavctrl->datastart);		//跳过文件头
	fillnum = wav_buffill(file, _audioplay.i2sbuf1, _audioplay.tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
	if (fillnum != WAV_I2S_TX_DMA_BUFSIZE) {
		printf("audio file read too few\n");
		*play_state = STOP_S_APY;
		f_close(file);
		return false;
	}
	fillnum = wav_buffill(file, _audioplay.i2sbuf2, _audioplay.tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
	if (fillnum != WAV_I2S_TX_DMA_BUFSIZE) {
		printf("audio file read too few\n");
		*play_state = STOP_S_APY;
		f_close(file);
		return false;
	}
	I2S_Play_Start();
	*play_state = PLAYING_S_APY;  //set state
	return true;
}
/**
  * @brief  wav_playing
  * @param
  * @note
  * @retval
  */
u8 wav_playing(void) {
	u32 fillnum;
	audioPlayState *play_state = &_audioplay.play_state;
	vu8 *iis_transed = &_audioplay.wavtransferend;
	vu8 *buffindex = &_audioplay.wavwitchbuf;
	__wavctrl *wavctrl = &_audioplay.wavctrl;
	FIL *file = &_audioplay.play_fil;

	//判断iis的中断发送完成
	if (*iis_transed == 1) {
		*iis_transed = 0;

		if (*buffindex)
			fillnum = wav_buffill(file, _audioplay.i2sbuf2, _audioplay.tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据
		else
			fillnum = wav_buffill(file, _audioplay.i2sbuf1, _audioplay.tbuf, WAV_I2S_TX_DMA_BUFSIZE, wavctrl->bps); //取数据

		//判断是否有数据发送
		if (fillnum == 0) { //播放完成
			*play_state = OVER_S_APY;
		}
	}
	return (u8)(*play_state);
}
/**
  * @brief  wav_play_over_check_mode
  * @param
  * @note
  * @retval
  */
void wav_play_over_check_mode(appaudio_st* audio) {
	audiocommon_play_over_check_mode(audio);
}

/**
  * @brief  wav_play_over
  * @param
  * @note
  * @retval
  */
void wav_play_over(void) {
	audiocommon_play_over();
}
/**
  * @brief  wav_play_stop
  * @param
  * @note
  * @retval
  */
void wav_play_stop(void) {
	audiocommon_play_stop();
}
/**
  * @brief  wav_play_pause
  * @param
  * @note
  * @retval
  */
void wav_play_pause(void) {
	audiocommon_play_pause();
}
/**
  * @brief  wav_play_continue
  * @param
  * @note
  * @retval
  */
void wav_play_continue(void) {
	audiocommon_play_continue();
}
