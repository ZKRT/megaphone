/*********************************************************************
 * Copyright (c) 2018 DJI. All rights reserved.
 * @File    application.h    
 * @Brief
 * Function prototypes for integration with FreeRTOS.
 * @History:
 *********************************************************************
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

// void RunIndicateTask(void const *parameter);
// void PsdkTestTask(void const *parameter);
// void PrcocessPsdkRecTask(void const *parameter);
// void application_init(void);
// void ProcossPsdkPeroidSendTask(void const *parameter);
void psdk_init(void);
void psdk_prcs(void);
#ifdef __cplusplus
}
#endif

#endif //APPLICATION_H

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/

