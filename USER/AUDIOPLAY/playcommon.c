/**
  ******************************************************************************
  * @file    playcommon.c
  * @author  ZKRT
  * @version V1.0
  * @date    27-March-2018
  * @brief
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
#include "audioplay.h"
#include "usart.h"
#include "ff.h"
#include "appfiles.h"
#include "i2s.h"
#include "appaudio_handle.h"
audioplay_st _audioplay;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief
  * @param
  * @note
  * @retval
  */
void audiocommon_play_over_check_mode(appaudio_st* audio) {
  switch (_audioplay.play_mode) { //判断当前播放模式，做不同的操作
  case NORMAL_MAPY:
    //close playing
    _audioplay.play_state = IDLE_S_APY;
    audio->play_id = AUDIOID_NONE;
    audio->play_item = NULL;
    break;

  case SINGLELOOP_MAPY:
    _audioplay.play_state = START_S_APY;
    break;


  case LISTLOOP_MAPY:
    _audioplay.play_state = START_S_APY;
    enter_play_next_song();
    break;

  default:
    //close playing
    _audioplay.play_state = IDLE_S_APY;
    audio->play_id = AUDIOID_NONE;
    audio->play_item = NULL;
    break;
  }
}
/**
  * @brief
  * @param
  * @note
  * @retval
  */
void audiocommon_play_over(void) {
  FIL *file = &_audioplay.play_fil;
  //关闭播放
  I2S_Play_Stop();
  f_close(file);
  _audioplay.wavtransferend = 0;
  _audioplay.play_state = IDLE_S_APY;
}
/**
  * @brief
  * @param
  * @note
  * @retval
  */
void audiocommon_play_stop(void) {
  audiocommon_play_over();
}
/**
  * @brief
  * @param
  * @note
  * @retval
  */
void audiocommon_play_pause(void) {
  //zkrt_todo://尝试关闭IIS输出中断，中断里判断状态填充无效发送数据，关闭WM8978 DAC, 关闭WM8978 IIS 输出
  //zkrt_test:暂时测试通过。此处不加任何代码，暂停处理在中断里判断状态填充无效发送数据
  I2S_Play_Stop(); 
}
/**
  * @brief
  * @param
  * @note
  * @retval
  */
void audiocommon_play_continue(void) {
  //zkrt_todo:
  //具体操作与wav_play_pause相对应
  //zkrt_test:暂时测试通过。此处不加任何代码。
  _audioplay.play_state = PLAYING_S_APY;
  I2S_Play_Start();
}
