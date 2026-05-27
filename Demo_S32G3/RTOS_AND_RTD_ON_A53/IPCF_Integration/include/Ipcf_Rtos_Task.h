/**
 * IPC Shared Memory Driver sample application
 *
 * Copyright 2020-2025 NXP
 * All Rights Reserved.
 *
 * NXP Confidential. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 */

#ifndef IPCF_RTOS_TASK_H
#define IPCF_RTOS_TASK_H

#if defined(__cplusplus)
extern "C"{
#endif

#define IPCF_SEND_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_SEND_FREQUENCY_MS            pdMS_TO_TICKS( 200 )
/* IPC shared memory driver interrupt service routine */
void Ipcf_Rtos_Prepare(void);
void Ipcf_Send_Task(void);
void handle_error(sint8 error);

#if defined(__cplusplus)
}
#endif

#endif /* IPC_SHM_SAMPLE_H */
