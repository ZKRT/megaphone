#include "string.h"
#include "stdio.h"
#include "exfuns.h"
#include "fattester.h"	

#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

 //�ļ������б�
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES"},			//NES�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
{"AVI"},			//��Ƶ�ļ�
};
/////////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
//FATFS *fs[_VOLUMES];//�߼����̹�����.	 
//FIL *file;	  		//�ļ�1
//FIL *ftemp;	  		//�ļ�2.
//UINT br,bw;			//��д����
//FILINFO fileinfo;	//�ļ���Ϣ
//DIR dir;  			//Ŀ¼

//u8 *fatbuf;			//SD�����ݻ�����
/////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////�����ļ���,ʹ��static��ʱ��////////////////////////////////////////////
FATFS __fs[_VOLUMES];
FATFS *fs[_VOLUMES];//�߼����̹�����.	 
FIL __file;
FIL *file;	  		//�ļ�1
FIL __ftemp;
FIL *ftemp;	  		//�ļ�2.
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
TCHAR	__fileinfo_lfname[_MAX_LFN * 2 + 1];
DIR dir;  			//Ŀ¼
u8 __fatbuf[512];
u8 *fatbuf;			//SD�����ݻ�����
///////////////////////////////////////////////////////////////////////////////////////

//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
//u8 exfuns_init(void)
//{
//	u8 i;
//	for(i=0;i<_VOLUMES;i++)
//	{
//		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
//		if(!fs[i])break;
//	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊftemp�����ڴ�
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//Ϊfatbuf�����ڴ�
//	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //������һ��ʧ��,��ʧ��.
//	else return 1;	
//}
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)&__fs[i];
	}
	file=(FIL*)&__file;
	ftemp=(FIL*)&__ftemp;
	fatbuf=(u8*)&__fatbuf[0];
	return 1;	
}
//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	
/**
  * @brief  appfatfs_test.
  * @param  None
  * @retval None
  */
#pragma pack(1)
typedef struct
{
	u8 id;
	char name[10];
}testsf;
#pragma pack() 
void appfatfs_test(void)
{
//////test 1: read and write	
//	FIL fsrc, fdst;      
//	BYTE buffer[4096];   
//	FRESULT res;         
//	UINT br, bw;         

//	res = f_open(&fsrc, "0:/TEST/read.txt", FA_OPEN_EXISTING | FA_READ);
//	if(res==FR_OK)
//	{
//		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
//		if(res==FR_OK)
//		{
//			printf("read.txt:%s\n",buffer);
//		}	
//	}
//	else
//	{
//		printf("can not open file 0:/TEST/read.txt\n");
//	}

//	
//	res = f_open(&fdst, "0:/TEST/write.txt", FA_CREATE_ALWAYS | FA_WRITE);
//  if (res==FR_OK)
//	{
//		res = f_write (&fdst, "��̩", sizeof("��̩"), &bw);	/* Write data to a file */
//		if(res==FR_OK)
//		{
//			printf("wirte ok\n");
//		}
//	}
//	else
//	{
//		printf("can not open file 0:/TEST/write.txt\n");
//	}	
//	f_close(&fsrc);
//	f_close(&fdst);

//////test2: scan dir and read dir 
//  mf_scan_files("0:/RECORDER");

//////test3: write struct data to file and read struct data in file
//	testsf sdata[10];
//	testsf rdata[20];
//	int i;
//	FIL fsrc, fdst;      
//	FRESULT res;         
//	UINT br, bw;       	
//	for(i=0; i<10; i++)
//	{
//		sdata[i].id = i+1;
//		sprintf(sdata[i].name, "%dhello\n", i+1);
//	}
//	res = f_open(&fdst, "0:/TEST/hello", FA_CREATE_ALWAYS | FA_WRITE);
//  if (res==FR_OK)
//	{
//		res = f_write (&fdst, (void*)&sdata, sizeof(sdata), &bw);	/* Write data to a file */
//		if(res==FR_OK)
//		{
//			printf("wirte ok\n");
//		}
//	}
//	else
//	{
//		printf("can not open file 0:/TEST/hello\n");
//	}	
//  f_sync(&fdst);
//	res = f_open(&fsrc, "0:/TEST/hello", FA_OPEN_EXISTING | FA_READ);
//	if(res==FR_OK)
//	{
//		res = f_read(&fsrc, (void*)&rdata, sizeof(rdata), &br);
//		if(res==FR_OK)
//		{
//			printf("read info is=======\n");
//			for(i=0; i<10; i++)
//			{
//				printf("id:%d,name:%s\n", rdata[i].id, rdata[i].name);
//			}
//		}	
//	}
//	else
//	{
//		printf("can not open file 0:/TEST/hello\n");
//	}	
//	f_close(&fsrc);	
//		f_close(&fdst); 
//	while(1);
	
	//////test4: ���ܴ��ļ��к��ٴ��ļ���ֻ���Ծ���·�����ļ����Ѳ��ԡ�code��
	

}	










