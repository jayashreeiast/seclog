/*
 *
 * IPC Shared Memory Driver sample application
 *
 * Copyright 2021-2025 NXP
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

#ifndef IPC_SHM_M7_0_H
#define IPC_SHM_M7_0_H

#if defined(__cplusplus)
extern "C"{
#endif

/* IPC shared memory driver interrupt service routine */
void shm_demo(void);
void handle_error(sint8 error);

#if defined(__cplusplus)
}
#endif

#endif /* IPC_SHM_SAMPLE_H */
