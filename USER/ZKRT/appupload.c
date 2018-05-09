/**
  ******************************************************************************
  * @file    appupload.c
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   数据定时上传应用
  *          + (1) init
  *
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appcan.h"
#include "appupload.h"
#include "osqtmr.h"
#include "ostmr.h"
/* Private macro -------------------------------------------------------------*/
#define UPLOAD_TIMEOUT              30 //30ms
#define UPLOAD_TIMEOUT_CNT          1 //
#define UPLOAD_TIMECNT_INIT         5000/UPLOAD_TIMEOUT
/* Private variables ---------------------------------------------------------*/
uint16_t upload_timecnt;
/* Private functions ---------------------------------------------------------*/
static void data_upload_msTask(void);


/**
  * @brief  appupload_init
  * @param  None
  * @retval None
  */
void appupload_init(void) {
  t_systmr_insertQuickTask(data_upload_msTask, UPLOAD_TIMEOUT, OSTMR_PERIODIC);
  upload_timecnt = UPLOAD_TIMECNT_INIT; //初始化10秒后才开始定时上传任务
}
/**
  * @brief  appupload_prcs
  * @param  None
  * @retval None
  */
void appupload_prcs(void) {
  if (!upload_timecnt) {
    upload_timecnt = UPLOAD_TIMEOUT_CNT;
    //执行定时上传
    appcan_hbpacket_send();
//    printf("upload test\n");
  }
}
/**
  * @brief  data_upload_msTask 定时任务
  * @param  None
  * @retval None
  */
static void data_upload_msTask(void) {
  if (upload_timecnt) {
    upload_timecnt--;
  }
}
/**
*@
*/
