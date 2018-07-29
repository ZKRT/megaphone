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
  switch (_audioplay.play_mode) { //�жϵ�ǰ����ģʽ������ͬ�Ĳ���
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
  //�رղ���
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
  //zkrt_todo://���Թر�IIS����жϣ��ж����ж�״̬�����Ч�������ݣ��ر�WM8978 DAC, �ر�WM8978 IIS ���
  //zkrt_test:��ʱ����ͨ�����˴������κδ��룬��ͣ�������ж����ж�״̬�����Ч��������
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
  //���������wav_play_pause���Ӧ
  //zkrt_test:��ʱ����ͨ�����˴������κδ��롣
  _audioplay.play_state = PLAYING_S_APY;
  I2S_Play_Start();
}
