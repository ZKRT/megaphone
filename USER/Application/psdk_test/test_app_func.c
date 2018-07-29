/*********************************************************************
*       Includes
**********************************************************************
*/
#include "appaudio_handle.h"
#include "appaudio.h"

#include "test_app_func.h"
#include <string.h>

extern appaudio_st _audio_handlest;

/*********************************************************************
*       Defines, configurable
**********************************************************************
*/
//unique index define
#define SWITCH1_INDEX 0
#define SWITCH2_INDEX 1
#define BUTTON1_INDEX 2
#define SCALE1_INDEX 3
#define INPUT1_INDEX 4
#define LIST1_INDEX 5
#define BUTTON2_INDEX 6
#define BUTTON3_INDEX 7
#define LIST3_INDEX 8
#define LIST4_INDEX 9
#define LIST2_INDEX 10
#define INPUT2_INDEX 11
#define BUTTON4_INDEX 12

#define VOL_SCALE1_INDEX 0
#define PLAY_BUTTON1_INDEX 1
#define PAUSE_BUTTON2_INDEX 2
#define STOP_BUTTON3_INDEX 3
#define LASTSONG_BUTTON4_INDEX 4
#define NEXTSONG_BUTTON5_INDEX 5
#define CHOSONG_INPUT1_INDEX 6
#define STARTREC_INPUT2_INDEX 7
#define PASUEREC_BUTTON6_INDEX 8
#define STOPREC_LIST1_INDEX 9
#define DELCREC_BUTTON7_INDEX 10
#define DELIDREC_INPUT3_INDEX 11

/*********************************************************************
*       Static data
**********************************************************************
*/

////@formatter:off
//static const T_PsdkAppFuncWidgetListItem s_TestWidgetList[] = {
//APPFUNC_DEF_SWITCH_WIDGET(SWITCH1_INDEX, "Red LED"),
//APPFUNC_DEF_SWITCH_WIDGET(SWITCH2_INDEX, "Blue LED"),

//APPFUNC_DEF_BUTTON_WIDGET(BUTTON1_INDEX, "Button_1"),
//APPFUNC_DEF_SCALE_WIDGET(SCALE1_INDEX, "Scale_1"),
//APPFUNC_DEF_INT_INPUT_BOX_WIDGET(INPUT1_INDEX, "InputBox_1", "Unit : m"),
//APPFUNC_DEF_LIST_WIDGET(LIST1_INDEX, "List_1", 3, "CH1", "CH2", "CH3"),

//APPFUNC_DEF_BUTTON_WIDGET(BUTTON2_INDEX, "Button_2"),

//APPFUNC_DEF_BUTTON_WIDGET(BUTTON3_INDEX, "Button_3"),
//APPFUNC_DEF_LIST_WIDGET(LIST3_INDEX, "List_3", 10, "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8", "CH9", "CH10"),
//APPFUNC_DEF_LIST_WIDGET(LIST4_INDEX, "List_4", 2, "TEST1", "TEST2"),
//APPFUNC_DEF_LIST_WIDGET(LIST2_INDEX, "List_5", 1, "TEST_CHOOSE1"),
//APPFUNC_DEF_INT_INPUT_BOX_WIDGET(INPUT2_INDEX, "InputBox_2", "Unit : ms"),
//};
////@formatter:on

//@formatter:off
static const T_PsdkAppFuncWidgetListItem s_TestWidgetList[] = {
    APPFUNC_DEF_SCALE_WIDGET(VOL_SCALE1_INDEX, "Volume "),
    APPFUNC_DEF_BUTTON_WIDGET(PLAY_BUTTON1_INDEX, "Play"),
    APPFUNC_DEF_BUTTON_WIDGET(PAUSE_BUTTON2_INDEX, "Pause/Continue"),
    APPFUNC_DEF_BUTTON_WIDGET(STOP_BUTTON3_INDEX, "Stop play"),
    APPFUNC_DEF_BUTTON_WIDGET(LASTSONG_BUTTON4_INDEX, "Previous"),
    APPFUNC_DEF_BUTTON_WIDGET(NEXTSONG_BUTTON5_INDEX, "Next"),
    APPFUNC_DEF_INT_INPUT_BOX_WIDGET(CHOSONG_INPUT1_INDEX, "Play by ID", "ID range: 0~31"),
    APPFUNC_DEF_INT_INPUT_BOX_WIDGET(STARTREC_INPUT2_INDEX, "Start record", "Entry Date: YearMonthDay"),
    APPFUNC_DEF_BUTTON_WIDGET(PASUEREC_BUTTON6_INDEX, "Pause/Continue record"),
    APPFUNC_DEF_LIST_WIDGET(STOPREC_LIST1_INDEX, "Stop record", 3, "N/A", "Save", "Cancel"),
    APPFUNC_DEF_BUTTON_WIDGET(DELCREC_BUTTON7_INDEX, "Delete the current record file"),
    APPFUNC_DEF_INT_INPUT_BOX_WIDGET(DELIDREC_INPUT3_INDEX, "Delete the record file by ID", "ID range: 0~31"),
};
//@formatter:on

const T_PsdkAppFuncWidgetListItem *g_TestWidgetList = s_TestWidgetList;
const uint8_t g_TestWidgetCount = sizeof(s_TestWidgetList) / sizeof(T_PsdkAppFuncWidgetListItem);

/*********************************************************************
*       Static functions
**********************************************************************
*/
//static U_AppFuncWidgetValue button1Val; //buttonVal get not have meaning

// static U_AppFuncWidgetValue switch1Val = {.switchVal = PSDK_APPFUNC_SWITCH_VAL_OFF};
// static U_AppFuncWidgetValue scale1Val = {.scaleVal = 50};
// static U_AppFuncWidgetValue input1Val = {.intInputBoxVal = 200};
// static U_AppFuncWidgetValue list1Val = {.listVal = 1};

// static U_AppFuncWidgetValue switch2Val = {.switchVal = PSDK_APPFUNC_SWITCH_VAL_OFF};

// static U_AppFuncWidgetValue list3Val = {.listVal = 9};
// static U_AppFuncWidgetValue list4Val = {.listVal = 0};
// static U_AppFuncWidgetValue list2Val = {.listVal = 0};
// static U_AppFuncWidgetValue input2Val = {.intInputBoxVal = 2000};
// static char receivePrint[1024];

//static U_AppFuncWidgetValue button1Val; //buttonVal get not have meaning
static U_AppFuncWidgetValue vol_scale1Val = {.scaleVal = VOLUME_INIT_VALUE};
static U_AppFuncWidgetValue choosesong_input1Val = {.intInputBoxVal = 0};
static U_AppFuncWidgetValue startrecord_input2Val = {.intInputBoxVal = 0};
static U_AppFuncWidgetValue stoprecord_list1Val = {.listVal = 0};
static U_AppFuncWidgetValue delidrecord_input3Val = {.intInputBoxVal = 0};
static uint8_t pause_button2Val = PLAY_CTRL_PAUSE; //2 state, pause and continue
static uint8_t pauserec_button6val = REC_CTRL_PAUSE;
static char receivePrint[1024];

/*********************************************************************
*       Export functions
**********************************************************************
*/

void Test_ReceiveFromMobileSDKFunc(const uint8_t *pData, uint16_t dataLen)
{
    memcpy(receivePrint, pData, dataLen);
    receivePrint[dataLen] = 0;
    PSDK_LOG_DEBUG("%s", receivePrint);
}

E_PsdkStat
Test_GetWidgetValueFunc(E_PsdkAppFuncWidgetType widgetType, uint8_t widgetIndex, U_AppFuncWidgetValue *pWidgetValue)
{
    switch (widgetIndex)
    {
    case VOL_SCALE1_INDEX:
        *pWidgetValue = vol_scale1Val;
        break;
    case CHOSONG_INPUT1_INDEX:
        *pWidgetValue = choosesong_input1Val;
        break;
    case STARTREC_INPUT2_INDEX:
        *pWidgetValue = startrecord_input2Val;
        break;
    case STOPREC_LIST1_INDEX:
        *pWidgetValue = stoprecord_list1Val;
        break;
    case DELIDREC_INPUT3_INDEX:
        *pWidgetValue = delidrecord_input3Val;
        break;
    default:
        break;
    }

    return PSDK_STAT_OK;
}

E_PsdkStat Test_SetWidgetValueFunc(E_PsdkAppFuncWidgetType widgetType, uint8_t widgetIndex,
                                   const U_AppFuncWidgetValue *pWidgetValue)
{
    switch (widgetIndex)
    {
    case VOL_SCALE1_INDEX:
        PSDK_LOG_DEBUG("vol Opt");
        vol_scale1Val = *pWidgetValue;
        enter_volctrl_handle((uint8_t)vol_scale1Val.scaleVal);
        break;
    case PLAY_BUTTON1_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button1 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button1 Release Up");
            if (_audio_handlest.play_id != AUDIOID_NONE)
            {
                playSong_plst temp = {_audio_handlest.play_id, REC_FLAG_OUTEN};
                enter_playsong_handle(&temp);
            }
            else
            {
                enter_play_next_song();
            }
        }
        break;
    case PAUSE_BUTTON2_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button2 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button2 Release Up, %d", pause_button2Val);
            if (pause_button2Val == PLAY_CTRL_PAUSE)
            { //pasue
                enter_playctrl_handle(PLAY_CTRL_PAUSE);
                pause_button2Val = PLAY_CTRL_CONTINUE;
            }
            else
            { //continue play song
                enter_playctrl_handle(PLAY_CTRL_CONTINUE);
                pause_button2Val = PLAY_CTRL_PAUSE;
            }
        }
        break;
    case STOP_BUTTON3_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button3 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button3 Release Up");
            enter_playctrl_handle(PLAY_CTRL_STOP);
        }
        break;
    case LASTSONG_BUTTON4_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button4 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button4 Release Up");
			enter_play_last_song();
        }
        break;
    case NEXTSONG_BUTTON5_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button5 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button5 Release Up");
            enter_play_next_song();
        }
        break;
    case CHOSONG_INPUT1_INDEX:
        PSDK_LOG_DEBUG("Input1 Opt, %d", pWidgetValue->intInputBoxVal);
        choosesong_input1Val = *pWidgetValue;
        {
            playSong_plst temp = {(uint8_t)choosesong_input1Val.intInputBoxVal, REC_FLAG_OUTEN};
            enter_playsong_handle(&temp);
        }
        break;
    case STARTREC_INPUT2_INDEX:
        startrecord_input2Val = *pWidgetValue;
        PSDK_LOG_DEBUG("Input2 Opt: %d", startrecord_input2Val.intInputBoxVal);
        {
            char name[AUDIO_NAME_LEN] = {0};
            uint8_t rid;
            sprintf(name, "%d--%d", startrecord_input2Val.intInputBoxVal, TimingDelay);
            enter_startrec_handle((uint8_t *)name, REC_FLAG_OUTEN, &rid);
        }
        break;
    case PASUEREC_BUTTON6_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button6 Press Down");
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            recCtrl_plst temp;
            PSDK_LOG_DEBUG("Button6 Release Up");
            if (pauserec_button6val == REC_CTRL_PAUSE)
            {
                temp.option = REC_CTRL_PAUSE;
                enter_recctrl_handle(&temp);
                pauserec_button6val = REC_CTRL_CONTINUE;
            }
            else
            {
                temp.option = REC_CTRL_CONTINUE;
                enter_recctrl_handle(&temp);
                pauserec_button6val = REC_CTRL_PAUSE;
            }
        }
        break;
    case STOPREC_LIST1_INDEX:
        PSDK_LOG_DEBUG("List1 Opt");
        stoprecord_list1Val = *pWidgetValue;
        {
            rstopRec_plst temp;
            enter_stoprec_handle((uint8_t)stoprecord_list1Val.listVal, &temp);
        }
        break;
    case DELCREC_BUTTON7_INDEX:
        if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN)
        {
            PSDK_LOG_DEBUG("Button7 Press Down, Stop Play");
            if ((_audio_handlest.play_id != AUDIOID_NONE) && (_audio_handlest.play_item->attr == ATTR_RECORD))
                enter_playctrl_handle(PLAY_CTRL_STOP);
        }
        else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP)
        {
            PSDK_LOG_DEBUG("Button7 Release Up");
            if ((_audio_handlest.play_id != AUDIOID_NONE) && (_audio_handlest.play_item->attr == ATTR_RECORD))
            {
                enter_delrecaudio_handle(_audio_handlest.play_id);
            }
        }
        break;
    case DELIDREC_INPUT3_INDEX:
        PSDK_LOG_DEBUG("Input3 Opt");
        delidrecord_input3Val = *pWidgetValue;
        enter_delrecaudio_handle((uint8_t)delidrecord_input3Val.intInputBoxVal);
        break;
    default:
        break;
    }

    return PSDK_STAT_OK;
}
//E_PsdkStat
//Test_GetWidgetValueFunc(E_PsdkAppFuncWidgetType widgetType, uint8_t widgetIndex, U_AppFuncWidgetValue *pWidgetValue)
//{
//    switch (widgetIndex) {
//        case SWITCH1_INDEX:
//            *pWidgetValue = switch1Val;
//            break;
//        case SWITCH2_INDEX:
//            *pWidgetValue = switch2Val;
//            break;

//        case SCALE1_INDEX:
//            *pWidgetValue = scale1Val;
//            break;
//        case INPUT1_INDEX:
//            *pWidgetValue = input1Val;
//            break;
//        case INPUT2_INDEX:
//            *pWidgetValue = input2Val;
//            break;

//        case LIST1_INDEX:
//            *pWidgetValue = list1Val;
//            break;
//        case LIST2_INDEX:
//            *pWidgetValue = list2Val;
//            break;
//        case LIST3_INDEX:
//            *pWidgetValue = list3Val;
//            break;
//        case LIST4_INDEX:
//            *pWidgetValue = list4Val;
//            break;
//        default:
//            break;
//    }

//    return PSDK_STAT_OK;
//}

//E_PsdkStat Test_SetWidgetValueFunc(E_PsdkAppFuncWidgetType widgetType, uint8_t widgetIndex,
//                                   const U_AppFuncWidgetValue *pWidgetValue)
//{
//    switch (widgetIndex) {
//        case BUTTON1_INDEX:
//            if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN) {
//                PSDK_LOG_DEBUG("Button1 Press Down");
//            } else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP) {
//                PSDK_LOG_DEBUG("Button1 Release Up");
//            }
//            break;
//        case BUTTON2_INDEX:
//            if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN) {
//                PSDK_LOG_DEBUG("Button2 Press Down");
//            } else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP) {
//                PSDK_LOG_DEBUG("Button2 Release Up");
//            }
//            break;
//        case BUTTON3_INDEX:
//            if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN) {
//                PSDK_LOG_DEBUG("Button3 Press Down");
//            } else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP) {
//                PSDK_LOG_DEBUG("Button3 Release Up");
//            }
//            break;
//        case BUTTON4_INDEX:
//            if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_PRESS_DOWN) {
//                PSDK_LOG_DEBUG("Button4 Press Down");
//            } else if (pWidgetValue->buttonVal == PSDK_APPFUNC_BUTTON_VAL_RELEASE_UP) {
//                PSDK_LOG_DEBUG("Button4 Release Up");
//            }
//            break;
//        case SWITCH1_INDEX:
//            PSDK_LOG_DEBUG("Switch1 Opt");
//            switch1Val = *pWidgetValue;
////            if (switch1Val.data == PSDK_APPFUNC_SWITCH_VAL_ON) {
////                LED_Control(LED2_RED, LED_OPT_ON);
////            } else {
////                LED_Control(LED2_RED, LED_OPT_OFF);
////            }
//            break;
//        case SWITCH2_INDEX:
//            PSDK_LOG_DEBUG("Switch2 Opt");
//            switch2Val = *pWidgetValue;
////            if (switch2Val.data == PSDK_APPFUNC_SWITCH_VAL_ON) {
////                LED_Control(LED3_BLUE, LED_OPT_ON);
////            } else {
////                LED_Control(LED3_BLUE, LED_OPT_OFF);
////            }
//            break;

//        case SCALE1_INDEX:
//            PSDK_LOG_DEBUG("Scale1 Opt");
//            scale1Val = *pWidgetValue;
//            break;

//        case INPUT1_INDEX:
//            PSDK_LOG_DEBUG("Input1 Opt");
//            input1Val = *pWidgetValue;
//            break;
//        case INPUT2_INDEX:
//            PSDK_LOG_DEBUG("Input2 Opt");
//            input2Val = *pWidgetValue;
//            break;

//        case LIST1_INDEX:
//            PSDK_LOG_DEBUG("List1 Opt");
//            list1Val = *pWidgetValue;
//            break;
//        case LIST2_INDEX:
//            PSDK_LOG_DEBUG("List2 Opt");
//            list2Val = *pWidgetValue;
//            break;
//        case LIST3_INDEX:
//            PSDK_LOG_DEBUG("List3 Opt");
//            list3Val = *pWidgetValue;
//            break;
//        case LIST4_INDEX:
//            PSDK_LOG_DEBUG("List4 Opt");
//            list4Val = *pWidgetValue;
//            break;
//        default:
//            break;
//    }

//    return PSDK_STAT_OK;
//}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
