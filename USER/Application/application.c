#include "sys.h"
#include <app_config.h>
#include <psdk.h>
#include "uart.h"
#include "log_printf.h"

#include "app_infor.h"
#include "test_app_func.h"
#include "test_msg_push.h"
#include "test_payload_cam_emu.h"
#include "test_payload_gimbal_emu.h"

#include "timer.h"
#include "appaudio.h"
#include "appfiles.h"

extern appaudio_st _audio_handlest;
extern const char play_state_string[OVER_S_APY + 1][20];
extern const char record_state_string[OVER_S_REC + 1][20];
//Payload SDK User Info Set
T_PsdkUserInfo userInfo = {
    .APP_Name = USER_APP_NAME,
    .APP_Id = USER_APP_ID,
    .APP_Key = USER_APP_KEY,
    .DeveloperAccount = USER_DEVELOPER_ACCOUNT};

static T_PsdkUpper s_psdkUpperHandle;

static uint8_t s_uartRecBuf[PSDK_UART_REC_BUF_LEN];
static char printBuffer[512];

uint32_t can_uart_sw_settings = 0;

E_PsdkStat Psdk_Write(const uint8_t *pSendData, uint16_t dataLen)
{
    UART_Write(PSDK_UART_NUM, pSendData, dataLen);
    return PSDK_STAT_OK;
}

int Psdk_Read(uint8_t *pReadData, uint16_t dataLen)
{
    int res;

    res = UART_Read(PSDK_UART_NUM, pReadData, dataLen);

    return res;
}

//period upload to app
void ProcossPsdkPeroidSendTask(void const *parameter)
{
    uint16_t realSendLen;
    char aibf[32];
    if (app_timer_flag.psdk_peroid_upload)
    {
        app_timer_flag.psdk_peroid_upload = 0;
        app_timer_cnt.psdk_peroid_upload = PSDK_PER_CNT;
        itoa(audiolist_pst->id_mask, aibf, 2);
        sprintf(printBuffer, "REC=%d,MUSIC=%d,ID_FLAG=%s\r\n", audiolist_pst->rec_num, audiolist_pst->music_num, aibf);
        // sprintf(printBuffer, "REC=%d,MUSIC=%d\r\n", audiolist_pst->rec_num, audiolist_pst->music_num);
        if (_audio_handlest.play_id != AUDIOID_NONE)
        {
            sprintf(printBuffer, "%s[%s:ID%d] ", printBuffer, &play_state_string[_audio_handlest.audioplay->play_state][0], _audio_handlest.play_id);
            strcat(printBuffer, (char *)_audio_handlest.play_item->name);
        }
        else if (_audio_handlest.rec_id != AUDIOID_NONE)
        {
            sprintf(printBuffer, "%s[%s:ID%d] ", printBuffer, &record_state_string[_audio_handlest.audiorec->rec_state][0], _audio_handlest.rec_id);
            strcat(printBuffer, (char *)_audio_handlest.rec_item->name);
            //            sprintf(printBuffer, "%s[%s:ID%d] %s\r\n", printBuffer, &record_state_string[_audio_handlest.audiorec->rec_state][0], _audio_handlest.rec_id, _audio_handlest.rec_item->name);
        }

        //push this info to DJI Pilot Floating Window
        PsdkAppFunc_PushMsgToFloatingWindow(&s_psdkUpperHandle, printBuffer);

        //transfer this info to MSDK
        PsdkAppFunc_TransferToApp(&s_psdkUpperHandle, (const uint8_t *)printBuffer, strlen(printBuffer) + 1, &realSendLen);
        //       PSDK_DBG_PRINTF("per send psdk");
    }
}
//Receive Psdk Data Process, all callback function process here.
void PrcocessPsdkRecTask(void const *parameter)
{
    int res = 0;
    res = Psdk_Read(s_uartRecBuf, sizeof(s_uartRecBuf));

    if (res > 0)
    {
        // printf("psdk read:%d\n", res);
        PsdkUpper_ProcessReceiveData(&s_psdkUpperHandle, s_uartRecBuf, res);
    }
}
//psdk init
void psdk_init(void)
{
    //===================== psdk init ==============================//
    //base init
    PsdkUpper_Init(&s_psdkUpperHandle, &userInfo);
    PsdkUpper_RegSendFunction(&s_psdkUpperHandle, Psdk_Write);

    //app function init
    PsdkAppFunc_Init(&s_psdkUpperHandle);
    PsdkAppFunc_SetReceiveFromAppCallback(&s_psdkUpperHandle, Test_ReceiveFromMobileSDKFunc);

    PsdkAppFunc_SetWidgetList(&s_psdkUpperHandle, g_TestWidgetList, g_TestWidgetCount);
    PsdkAppFunc_RegGetWidgetValueCallback(&s_psdkUpperHandle, Test_GetWidgetValueFunc);
    PsdkAppFunc_RegSetWidgetValueCallback(&s_psdkUpperHandle, Test_SetWidgetValueFunc);

    //Msg subscribe Function init, callback functions in Application/psdk_test/test_msg_push.c
    PsdkMsgSub_Init(&s_psdkUpperHandle);
    PsdkMsgSub_RegMsgCallbackList(&s_psdkUpperHandle, &g_PushDataMsgCallbackList);

    //    // //Camera function Emulate Test, callback functions in Application/psdk_test/test_payload_cam_emu.c
    //    // //If want transfer video to Mobile SDK or DJI Pilot, this function must call to init Payload camera.
    //    // PsdkCamera_Init(&s_psdkUpperHandle, &g_testCameraHandler);

    //    // //Gimbal function Emulate Test, callback functions in Application/psdk_test/test_payload_gimbal_emu.c
    //    // PsdkGimbal_Init(&s_psdkUpperHandle, &g_testGimbalHandler);
    //    //===================== psdk init End ============================//

    PSDK_DBG_PRINTF("psdk init...\n");
}
//psdk process
void psdk_prcs(void)
{
    PrcocessPsdkRecTask(NULL);
    ProcossPsdkPeroidSendTask(NULL);
}
