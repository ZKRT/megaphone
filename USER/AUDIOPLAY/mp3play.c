/**
  ******************************************************************************
  * @file    mp3play.c
  * @author  ZKRT
  * @version V1.0
  * @date    23-March-2018
  * @brief   mp3播放驱动
	*					 + (1) init
	*
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "audioplay.h"
#include "i2s.h"
#include "appfiles.h"
#include "appaudio.h"
#include "mp3play.h"
#include "wm8978.h"
extern audioplay_st _audioplay;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//mp3解析相关
HMP3Decoder _mp3decoder = 0;        //解析关键结构体信息
MP3FrameInfo _mp3FrameInfo;      //帧信息
mp3_user_dec_st mp3user_dec;    //解析需要用到的参数
mp3_user_app_st mp3user_app;    //用户关心的mp3状态信息等
/* Private functions ---------------------------------------------------------*/
void mp3_parameter_reset(void);
void mp3_start_decode_reset(void);
void mp3_i2s_dma_tx_callback(void);
void mp3_iis_init(MP3FrameInfo *mp3finfo);
u8 mp3_transcomple(void);
void mp3_decode_handle(void);
u8 mp3_decode(void);
void mp3_output_trans(u8 state);
void mp3_find_frame(FIL *file);
///////////////////////////////////////////////////////////////////// process
/**
  * @brief
  * @param  None
  * @retval None
  */
//void mp3_init(void)
//{
//	//global parameter
//	mp3_parameter_reset();
//	//init decoder
//	if(_mp3decoder)
//		MP3FreeDecoder(_mp3decoder);
//
//	_mp3decoder = MP3InitDecoder();
//	if(_mp3decoder==0)
//	{
//		printf("MP3InitDecoder fail.\n");
//		mp3user_app.decoder_status = 0;
//	}
//	else
//	{
//		printf("MP3InitDecoder ok.\n");
//		mp3user_app.decoder_status = 1;
//	}
//}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_prcs(void) {
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_playing(void) {
	FIL *file = &_audioplay.play_fil;
	audioPlayState *play_state = &_audioplay.play_state;
	if (mp3user_dec.state != Normal_MP3S) {
		printf("mp3_playing fail:%d\n", mp3user_dec.state);
		*play_state = OVER_S_APY;
		return;
	}

	if (mp3_transcomple() == 0) //等待传输完成
		return;

	mp3_find_frame(file);
	mp3_decode_handle();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_play_pause(void) {
	audiocommon_play_pause();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_play_stop(void) {
	audiocommon_play_stop();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_play_over(void) {
	audiocommon_play_over();
}
//////////////////////////////////////////////////////// parameter reset
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_parameter_reset(void) {
//mp3_user_dec_st
	mp3_start_decode_reset();
//mp3_user_app_st
	mp3user_app.decoder_status = 0;
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_start_decode_reset(void) {
//mp3_user_dec_st
	memset(mp3user_dec.readBuf, 0, sizeof(mp3user_dec.readBuf));
	mp3user_dec.readPtr = NULL;
	mp3user_dec.bytesLeft = 0;
	mp3user_dec.offset = 0;
	mp3user_dec.foudStartOfFrame = false;
	mp3user_dec.buffer1 = (short*)&_audioplay.i2sbuf1[0];
	mp3user_dec.buffer2 = (short*)&_audioplay.i2sbuf2[0];
	mp3user_dec.output = mp3user_dec.buffer1;
	mp3user_dec.get_frame_info = true;
	mp3user_dec.frame_cnt = 0;
	mp3user_dec.state = Normal_MP3S;
	mp3user_dec.need_wait_transcomle = false;
	mp3user_dec.decode_fail_cnt = 0;
}
////////////////////////////////////////////////////////////////////
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_iis_init(MP3FrameInfo *mp3finfo) {
	if (mp3finfo->bitsPerSample == 16) {
		WM8978_I2S_Cfg(2, 0);	//飞利浦标准,16位数据长度
		I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
	} else if (mp3finfo->bitsPerSample == 24) {
		WM8978_I2S_Cfg(2, 2);	//飞利浦标准,24位数据长度
		I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_24b);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
	} else {
		printf("audio bps not support\n");
		mp3user_dec.state = FrameNotSupport_MP3S;
		return ;
	}
	if (I2S2_SampleRate_Set(mp3finfo->samprate) != 0) {	//设置采样率
		printf("audio samprate not support\n");
		mp3user_dec.state = SampleRateNotSupport_MP3S;
		return ;
	}
	I2S2_TX_DMA_Init((u8*)mp3user_dec.buffer1, (u8*)mp3user_dec.buffer2, MP3_OUTPUT_SIZE);         //配置TX DMA
	i2s_tx_callback = mp3_i2s_dma_tx_callback;			                                        //回调函数指wav_i2s_dma_callback
	I2S_Play_Start();
	printf("mp3_iis_init..................................\n");
}
/**
  * @brief
  * @param  None
  * @retval 1-ok, 0-not ok
  */
u8 mp3_transcomple(void) {
	if (mp3user_dec.need_wait_transcomle != true)
		return 1;

	if (_audioplay.wavtransferend == 0)
		return 0;

	if (_audioplay.wavwitchbuf == 0)
		mp3user_dec.output = mp3user_dec.buffer1;
	else
		mp3user_dec.output = mp3user_dec.buffer2;

	_audioplay.wavtransferend = 0;
	mp3user_dec.need_wait_transcomle = false;

	return 1;
}
/**
  * @brief
  * @param  None
  * @retval
  */
u8 mp3_decode(void) {
	mp3_user_trans_state_em TransState = None_Trans;
	//解码
	if (mp3user_dec.foudStartOfFrame == true) {
		int err;
		mp3user_dec.foudStartOfFrame = false;
		err = MP3Decode(_mp3decoder, &mp3user_dec.readPtr, &mp3user_dec.bytesLeft, mp3user_dec.output, 0);
		printf("2left:%d\n", mp3user_dec.bytesLeft);
		if (mp3user_dec.bytesLeft == READBUF_SIZE)
			mp3user_dec.bytesLeft = 0;
		switch (err) {
		case ERR_MP3_NONE:
			mp3user_dec.frame_cnt++;
			mp3user_dec.decode_fail_cnt = 0;
			printf("MP3Decode ok.\n");
			if (mp3user_dec.get_frame_info == true) {
				MP3GetLastFrameInfo(_mp3decoder, &_mp3FrameInfo);
				printf("bitrate:%d, nChans:%d, samprate:%d, bitsPerSample:%d, outputSamps:%d, layer:%d, version:%d\n",
				       _mp3FrameInfo.bitrate, _mp3FrameInfo.nChans, _mp3FrameInfo.samprate, _mp3FrameInfo.bitsPerSample, _mp3FrameInfo.outputSamps, _mp3FrameInfo.layer, _mp3FrameInfo.version);
				if ((_mp3FrameInfo.bitrate) && (_mp3FrameInfo.outputSamps == MP3_OUTPUT_SIZE)) {
					TransState = I2SInit_Trans;
					mp3user_dec.get_frame_info = false;
				} else {
					if (mp3user_dec.frame_cnt > 50) { //zkrt_notice: 错误计数处理
						mp3user_dec.state = FrameNotSupport_MP3S;
					}
					TransState = None_Trans;
					return TransState;
				}
			} else
				TransState = JustTrans_Trans;
			break;
//			case ERR_MP3_LIB_ERROR:
//				printf("ERR_MP3_LIB_ERROR.......\n");
//				break;
		default:
			printf("MP3Decode fail:%d\n", err);
			mp3user_dec.decode_fail_cnt++; //出错计数
			mp3user_dec.bytesLeft = 0;     //重置输出剩余

			if (mp3user_dec.get_frame_info == false) { //解析出错后重新获取帧信息
				mp3user_dec.get_frame_info = true;
				TransState = I2SStop_Trans;
			}
			//出错过多重新初始化解析体 或者 特定的错误
			if ((err == ERR_MP3_LIB_ERROR) ||
			        (mp3user_dec.decode_fail_cnt == 10) ||
			        (mp3user_dec.decode_fail_cnt == 20) ||
			        (mp3user_dec.decode_fail_cnt == 30) ||
			        (mp3user_dec.decode_fail_cnt == 40)) {
				if (_mp3decoder)
					MP3FreeDecoder(_mp3decoder);
				_mp3decoder = MP3InitDecoder();
				printf("reset decoder...................\n");
			}
			//出错太多直接返回错误不再解析
			if (mp3user_dec.decode_fail_cnt > 100)
				mp3user_dec.state = DecodeFail_MP3S;
			break;
		}
	}
	return	TransState;
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_output_trans(u8 state) {
	switch (state) {
	case None_Trans:
		break;
	case I2SInit_Trans:
		mp3_iis_init(&_mp3FrameInfo);
		break;
	case JustTrans_Trans:
		printf("outsamp:%d, number_out_samples: %d\n", _mp3FrameInfo.outputSamps, _mp3FrameInfo.number_out_samples);
		if (_mp3FrameInfo.nChans == 1)
			Convert_Mono(mp3user_dec.output);
		else
			Convert_Stereo(mp3user_dec.output);
		mp3user_dec.need_wait_transcomle = true;
		break;
	case I2SStop_Trans:
		I2S_Play_Stop();
		break;
	default:
		break;
	}
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_decode_handle(void) {
	mp3_output_trans(mp3_decode());
}
///**  //190328
//  * @brief
//  * @param  None
//  * @retval None
//  */
//void mp3_decode_handle(void)
//{
//	//解码
//	if(mp3user_dec.foudStartOfFrame ==true)
//	{
//		int err;
////		printf("we found mp3 sync word.\n");
//		mp3user_dec.foudStartOfFrame = false;
//		err = MP3Decode(_mp3decoder, &mp3user_dec.readPtr, &mp3user_dec.bytesLeft, mp3user_dec.output, 0);
//		printf("2left:%d\n", mp3user_dec.bytesLeft);
//		if(mp3user_dec.bytesLeft ==READBUF_SIZE)
//			mp3user_dec.bytesLeft = 0;
//		switch(err)
//		{
//			case ERR_MP3_NONE:
//				mp3user_dec.frame_cnt++;
//				mp3user_dec.decode_fail_cnt = 0;
//				printf("MP3Decode ok.\n");
//				if(mp3user_dec.get_frame_info==true)
//				{
//					MP3GetLastFrameInfo(_mp3decoder, &_mp3FrameInfo);
//					printf("bitrate:%d, nChans:%d, samprate:%d, bitsPerSample:%d, outputSamps:%d, layer:%d, version:%d\n",
//						_mp3FrameInfo.bitrate, _mp3FrameInfo.nChans, _mp3FrameInfo.samprate, _mp3FrameInfo.bitsPerSample, _mp3FrameInfo.outputSamps, _mp3FrameInfo.layer, _mp3FrameInfo.version);
//					if((_mp3FrameInfo.bitrate)&&(_mp3FrameInfo.outputSamps == MP3_OUTPUT_SIZE))  //zkrt_notice
//					{
//						mp3user_dec.get_frame_info = false;
////						audio_enter_play_mode(); //not need
//						mp3_iis_init(&_mp3FrameInfo);
//					}
//					else
//					{
//						if(mp3user_dec.frame_cnt >50) //zkrt_notice: 错误计数处理
//						{
//							mp3user_dec.state = FrameNotSupport_MP3S;
//						}
//						return;
//					}
//				}
//				printf("outsamp:%d, number_out_samples: %d\n", _mp3FrameInfo.outputSamps,_mp3FrameInfo.number_out_samples);
//				if(_mp3FrameInfo.nChans==1)
//					Convert_Mono(mp3user_dec.output);
////					Convert_Mono_zkrt(mp3user_dec.output, _mp3FrameInfo.outputSamps);
//				else
//					Convert_Stereo(mp3user_dec.output);
////					Convert_Stereo_zkrt(mp3user_dec.output, _mp3FrameInfo.outputSamps);
//
//				mp3user_dec.need_wait_transcomle = true;
//				break;
////			case ERR_MP3_INDATA_UNDERFLOW:
////					mp3user_dec.state = DecodeFail_MP3S;
////				break;
////				case ERR_MP3_MAINDATA_UNDERFLOW:
////					mp3user_dec.state = DecodeFail_MP3S;
////				break;
////			case ERR_MP3_FREE_BITRATE_SYNC:
////					mp3user_dec.state = DecodeFail_MP3S;
////				break;
//			default:
//				printf("MP3Decode fail:%d\n", err);
//				if(mp3user_dec.get_frame_info == false)
//				{
//					mp3user_dec.get_frame_info = true; //解析出错后重新获取帧信息
//					I2S_Play_Stop();
//				}
//				mp3user_dec.decode_fail_cnt++;
//				if(mp3user_dec.decode_fail_cnt ==10)  //出错过多重新初始化解析体
//				{
//					if(_mp3decoder)
//						MP3FreeDecoder(_mp3decoder);
//					_mp3decoder = MP3InitDecoder();
//				}
//				if(mp3user_dec.decode_fail_cnt >50)  //出错太多直接返回错误不再解析
//					mp3user_dec.state = DecodeFail_MP3S;
//			break;
//		}
//	}
//}
/**
  * @brief
  * @param  None
  * @retval 0-ok, 1-fail
  */
u8 mp3_get_frame_info_by_file(MP3FrameInfo *mp3FrameInfo, const char *fname) {
	FIL *file = &_audioplay.play_fil;
	strcpy(_audioplay.play_objname, fname);
	printf("get frame info:%s\n", _audioplay.play_objname);

	mp3_play_start();
	while (1) {
		while (mp3user_dec.foudStartOfFrame == false) {
			mp3_find_frame(file);
			if (mp3user_dec.state != Normal_MP3S)
				return 1;
		}
		if (mp3_decode() == I2SInit_Trans) {
			mp3FrameInfo->datasize = f_size(file) - f_tell(file);//zkrt_todo: 连续获取到若干次同样帧信息才认为是播放起始帧头
			mp3_play_over();
			return 0;
		}
		if (mp3user_dec.state != Normal_MP3S)
			return 1;

	}
}
///**  //180328
//  * @brief
//  * @param  None
//  * @retval 0-ok, 1-fail
//  */
//u8 mp3_get_frame_info_by_file(MP3FrameInfo *mp3FrameInfo, const char *fname)
//{
//	FIL *file = &_audioplay.play_fil;
//	int error;
//	strcpy(_audioplay.play_objname, fname);
//	printf("get frame info:%s\n",_audioplay.play_objname);
//	mp3_play_start();
//	while(1)
//	{
//		while(mp3user_dec.foudStartOfFrame == false)
//		{
//			mp3_find_frame(file);
//			if(mp3user_dec.state != Normal_MP3S)
//				return 1;
//		}
//		mp3user_dec.foudStartOfFrame = false;
//		error = MP3Decode(_mp3decoder, &mp3user_dec.readPtr, &mp3user_dec.bytesLeft, mp3user_dec.output, 0);
//		printf("2left:%d\n", mp3user_dec.bytesLeft);
//		if(mp3user_dec.bytesLeft ==READBUF_SIZE)
//			mp3user_dec.bytesLeft = 0;
//		switch(error)
//		{
//			case ERR_MP3_NONE:
//				MP3GetLastFrameInfo(_mp3decoder, mp3FrameInfo);
//				printf("bitrate:%d, nChans:%d, samprate:%d, bitsPerSample:%d, outputSamps:%d, layer:%d, version:%d\n",
//				mp3FrameInfo->bitrate, mp3FrameInfo->nChans, mp3FrameInfo->samprate, mp3FrameInfo->bitsPerSample, mp3FrameInfo->outputSamps, mp3FrameInfo->layer, mp3FrameInfo->version);
////				if(mp3FrameInfo->layer ==3)
//				if((_mp3FrameInfo.bitrate)&&(_mp3FrameInfo.outputSamps == MP3_OUTPUT_SIZE))  //zkrt_notice
//					return 0;
//			break;
//			default:
//				mp3user_dec.decode_fail_cnt++;
//				if(mp3user_dec.decode_fail_cnt ==10)
//				{
//					if(_mp3decoder)
//						MP3FreeDecoder(_mp3decoder);
//					_mp3decoder = MP3InitDecoder();
//				}
//				printf("MP3Decode:%d\n",error);
//				break;
//		}
//	}
//}
/**
  * @brief  根据文件来启动mp3播放
  * @param
  * @retval
  */
u8 mp3_play_start(void) {
	FRESULT f_res;
	FIL *file = &_audioplay.play_fil;
	char *fname = _audioplay.play_objname;
	audioPlayState *play_state = &_audioplay.play_state;
	//global parameter
	mp3_parameter_reset();
	//init decoder
	if (_mp3decoder)
		MP3FreeDecoder(_mp3decoder);

	_mp3decoder = MP3InitDecoder();
	if (_mp3decoder == 0) {
		printf("MP3InitDecoder fail.\n");
		mp3user_dec.state = Error_MP3S;
		mp3user_app.decoder_status = 0;
		*play_state = FAIL_S_APY;
		return 0;
	} else {
		printf("MP3InitDecoder ok.\n");
		mp3user_app.decoder_status = 1;
	}
	f_res = f_open(file, fname, FA_OPEN_EXISTING | FA_READ);
	if (f_res != FR_OK) {
		mp3user_dec.state = OpenFileFail_MP3S;
		printf("f_open fail.\n");
		*play_state = FAIL_S_APY;
		return 0;
	}
	_audioplay.play_state = PLAYING_S_APY;
	return 1;
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_find_frame(FIL *file) {
	UINT f_br;
	FRESULT f_res;

	if (mp3user_dec.bytesLeft)
		memmove(mp3user_dec.readBuf, mp3user_dec.readPtr, mp3user_dec.bytesLeft);

	if (mp3user_dec.bytesLeft < READBUF_SIZE) { //补充数据
		f_br = 0;
		f_res = f_read(file, mp3user_dec.readBuf + mp3user_dec.bytesLeft, READBUF_SIZE - mp3user_dec.bytesLeft, &f_br);
		if (f_res != FR_OK) {
			//读取出错
			printf("res:%d, f_br:%d\n", f_res, f_br);
			printf("can not read file.\n");
			mp3user_dec.state = ReadFileFail_MP3S;
			return;
		} else {
			if (f_br == 0) {
				//读文件到末尾
				printf("file over.\n");
				mp3user_dec.state = ReadOver_MP3S;
				return;
			}
			mp3user_dec.bytesLeft += f_br;
			mp3user_dec.readPtr = mp3user_dec.readBuf;
			printf("0left:%d\n", mp3user_dec.bytesLeft);
			mp3user_dec.offset = MP3FindSyncWord(mp3user_dec.readBuf, mp3user_dec.bytesLeft);
			if (mp3user_dec.offset < 0) {
				//输入缓冲区未包含帧起始点。读取另一帧
				mp3user_dec.bytesLeft = 0;
				return;
			} else {
				//找到帧起始点。偏移量包含输入缓冲区的帧起始点位置
				mp3user_dec.foudStartOfFrame = true;
				mp3user_dec.readPtr += mp3user_dec.offset;  //定位帧数据的起始点（在readBuf数组里的位置）
				mp3user_dec.bytesLeft = mp3user_dec.bytesLeft - mp3user_dec.offset; //定位有效帧的剩余长度，在readBuf里
				//zkrt_test 虽然找到帧起始点，但是剩余数据长度不够处理
				printf("1left:%d\n", mp3user_dec.bytesLeft);
				if (mp3user_dec.bytesLeft < MAINBUF_SIZE) {
					mp3_find_frame(file);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////// iis callback
/**
  * @brief
  * @param  None
  * @retval None
  */
void mp3_i2s_dma_tx_callback(void) {
	u16 i;
	if (DMA1_Stream4->CR & (1 << 19)) {
		_audioplay.wavwitchbuf = 0;
		if ((_audioplay.play_state != PLAYING_S_APY) || (_audioplay.play_state != START_S_APY)) { //当不播放音频时，跳进传输中断后，直接填充播放的音频数据为0
			for (i = 0; i < MP3_OUTPUT_SIZE * 2; i++) { //暂停 //16位缓存所以填充2倍的MP3_OUTPUT_SIZE
				_audioplay.i2sbuf1[i] = 0; //填充0
			}
		}
	} else {
		_audioplay.wavwitchbuf = 1;
		if ((_audioplay.play_state != PLAYING_S_APY) || (_audioplay.play_state != START_S_APY)) {
			for (i = 0; i < MP3_OUTPUT_SIZE * 2; i++) { //暂停
				_audioplay.i2sbuf2[i] = 0; //填充0
			}
		}
	}
	_audioplay.wavtransferend = 1;
}
