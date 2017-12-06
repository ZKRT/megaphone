#include "string.h"
#include "stdio.h"
#include "exfuns.h"
#include "fattester.h"	

#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

 //文件类型列表
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES"},			//NES文件
{"TXT","C","H"},	//文本文件
{"WAV","MP3","APE","FLAC"},//支持的音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
{"AVI"},			//视频文件
};
/////////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
//FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
//FIL *file;	  		//文件1
//FIL *ftemp;	  		//文件2.
//UINT br,bw;			//读写变量
//FILINFO fileinfo;	//文件信息
//DIR dir;  			//目录

//u8 *fatbuf;			//SD卡数据缓存区
/////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////公共文件区,使用static的时候////////////////////////////////////////////
FATFS __fs[_VOLUMES];
FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
FIL __file;
FIL *file;	  		//文件1
FIL __ftemp;
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
TCHAR	__fileinfo_lfname[_MAX_LFN * 2 + 1];
DIR dir;  			//目录
u8 __fatbuf[512];
u8 *fatbuf;			//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////

//为exfuns申请内存
//返回值:0,成功
//1,失败
//u8 exfuns_init(void)
//{
//	u8 i;
//	for(i=0;i<_VOLUMES;i++)
//	{
//		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//为磁盘i工作区申请内存	
//		if(!fs[i])break;
//	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为file申请内存
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为ftemp申请内存
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//为fatbuf申请内存
//	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
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
//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;//错误的字符串.
 	for(i=0;i<5;i++)//得到后缀名
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
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
//		res = f_write (&fdst, "瑞泰", sizeof("瑞泰"), &bw);	/* Write data to a file */
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
	
	//////test4: 不能打开文件夹后再打开文件，只能以绝地路径打开文件，已测试。code略
	

}	










