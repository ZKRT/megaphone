//#include "sys.h"
//#include "coder.h"
//#include "appfiles.h"
//#include "audioplay.h"
//#include "usart.h"
//#include "ff.h"
//#include "i2s.h"
//#include "wm8978.h"
//#include "appaudio.h"
//extern audioplay_st _audioplay;
//#define READBUF_SIZE         2048
//#define MP3_OUTPUT_SIZE      (MAX_NCHAN*1152)
//HMP3Decoder mp3decoder;
//MP3FrameInfo mp3FrameInfo;
//int bytesLeft; //readPtr��ʣ����Ч��֡���ݴ�С
//u8  readBuf[READBUF_SIZE]; //����ļ�����
//u8 *readPtr; //ָ��֡ͷ
//s32 offset;  //֡ͷƫ��λ��
//bool foudStartOfFrame=false;
//static FIL fsrc;
//short *mp3buffer1 = (short*)&_audioplay.i2sbuf1[0];
//short *mp3buffer2 = (short*)&_audioplay.i2sbuf2[0];
//short *output= (short*)&_audioplay.i2sbuf1[0];
//int err;
//u8 first_decode=0;

////����ʱ,I2S DMA����ص�����
//void _mp3_i2s_dma_tx_callback(void)
//{
//	u16 i;
//	if(DMA1_Stream4->CR&(1<<19))
//	{
//		_audioplay.wavwitchbuf =0;
//	}
//	else
//	{
//		_audioplay.wavwitchbuf =1;
//	}
//	_audioplay.wavtransferend =1;
//}
//void mp3_iis_play_standby(MP3FrameInfo *mp3finfo)
//{
//	if(mp3finfo->bitsPerSample==16)
//	{
//		WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
//		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡����
//	}
//	else if(mp3finfo->bitsPerSample==24)
//	{
//		WM8978_I2S_Cfg(2,2);	//�����ֱ�׼,24λ���ݳ���
//		I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
//	}
//	else
//	{
//		printf("audio bps not support\n");
//		return ;
//	}
//	I2S2_SampleRate_Set(mp3finfo->samprate);	//���ò�����
//	I2S2_TX_DMA_Init(_audioplay.i2sbuf1, _audioplay.i2sbuf2, MP3_OUTPUT_SIZE);         //����TX DMA
//	i2s_tx_callback = _mp3_i2s_dma_tx_callback;			                                        //�ص�����ָwav_i2s_dma_callback
//	I2S_Play_Start();
//}
//void mp3play(void)
//{
//	u32 f_br;
//	FRESULT f_res;
//	mp3decoder=MP3InitDecoder();
//	if(mp3decoder==0)
//	{
//		printf("MP3InitDecoder fail.\n");
//		while(1);
//	}
//	printf("MP3InitDecoder ok.\n");
//	f_res = f_open(&fsrc,"0:/MUSIC/Finesse.mp3",FA_OPEN_EXISTING|FA_READ);
//	if(f_res !=FR_OK)
//	{
//		printf("f_open fail.\n");
//		while(1);
//	}
//	printf("f_open ok.\n");
//	///�����������ҵ���ЧMP3֡��ʼ��
//	bytesLeft =0;
//	while(1)
//	{
//		if(bytesLeft)
//			memmove(readBuf, readPtr, bytesLeft);
//		if(bytesLeft < READBUF_SIZE) //��������
//		{
//			f_br = 0;
//			f_res = f_read(&fsrc, readBuf+bytesLeft, READBUF_SIZE-bytesLeft, &f_br);
//			if(f_res!=FR_OK)
//			{
//				//��ȡ����
//				printf("res:%d, f_br:%d\n", f_res, f_br);
//				printf("can not read file.\n");
//				while(1);
//				break;
//			}
//			else
//			{
//				bytesLeft+=f_br;
//				readPtr = readBuf;
//				printf("0left:%d\n", bytesLeft);
//				offset=MP3FindSyncWord(readBuf, bytesLeft);
//				if(offset <0)
//				{
//					if(f_br ==0)
//					{
//						//���ļ���ĩβ
//						printf("file over.\n");
//						break;
//					}
//					else//���뻺����δ����֡��ʼ�㡣��ȡ��һ֡
//					{
//						bytesLeft = 0;
//						continue;
//					}
//				}
//				else
//				{
//					//�ҵ�֡��ʼ�㡣ƫ�����������뻺������֡��ʼ��λ��
//					foudStartOfFrame = true;
//					readPtr+=offset;    //��λ֡���ݵ���ʼ�㣨��readBuf�������λ�ã�
//					bytesLeft= bytesLeft - offset;  //��λ��Ч֡��ʣ�೤�ȣ���readBuf��
//				}
//			}
//		}
//		//����
//		if(foudStartOfFrame ==true)
//		{
//			printf("we found mp3 sync word.\n");
//			foudStartOfFrame = false;
//			printf("1left:%d\n", bytesLeft);
//			err = MP3Decode(mp3decoder, &readPtr, &bytesLeft, output, 0);
//			printf("2left:%d\n", bytesLeft);
//			if(bytesLeft ==READBUF_SIZE)
//				bytesLeft = 0;
//			switch(err)
//			{
//				case ERR_MP3_NONE:
//					printf("MP3Decode ok.\n");
//					if(!first_decode)
//					{
//						MP3GetLastFrameInfo(mp3decoder, &mp3FrameInfo);
//						printf("bitrate:%d, nChans:%d, samprate:%d, bitsPerSample:%d, outputSamps:%d, layer:%d, version:%d\n",
//							mp3FrameInfo.bitrate, mp3FrameInfo.nChans, mp3FrameInfo.samprate, mp3FrameInfo.bitsPerSample, mp3FrameInfo.outputSamps, mp3FrameInfo.layer, mp3FrameInfo.version);
//
//						if(mp3FrameInfo.layer ==3)
//						{
//							first_decode = 1;
//							audio_enter_play_mode();
//							mp3_iis_play_standby(&mp3FrameInfo);
//						}
//						else
//							break;
//					}
//					if(mp3FrameInfo.nChans==1)
//						Convert_Mono(output);
//					else
//						Convert_Stereo(output);
//					//iis trans data
//					while(_audioplay.wavtransferend==0);
//					_audioplay.wavtransferend = 0;
//					if(_audioplay.wavwitchbuf == 0)
//					{
//						output=mp3buffer1;
//					}
//					else
//					{
//						output=mp3buffer2;
//					}
//					break;
////				case ERR_MP3_INDATA_UNDERFLOW:
////					CloseMP3File();
////				break;
//				case ERR_MP3_MAINDATA_UNDERFLOW:
//					printf("ERR_MP3_MAINDATA_UNDERFLOW\n");
////					ReadMoreMp3Data();
//				break;
//				case ERR_MP3_INVALID_FRAMEHEADER:
//					printf("ERR_MP3_INVALID_FRAMEHEADER\n");
////					ReadMoreMp3Data();
//				break;
////				case ERR_MP3_FREE_BITRATE_SYNC:
////					CloseMP3File();
////				break;
//				default:
//					printf("MP3Decode fail:%d\n", err);
////					while(1);
////					CloseMP3File();
//				break;
//			}
//		}
//	}
//	printf("decode end.\n");
//	f_close(&fsrc);
//	MP3FreeDecoder(mp3decoder);
//	while(1);
//	///
//}
