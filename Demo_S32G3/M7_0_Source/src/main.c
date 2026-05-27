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
#if defined(__cplusplus)
extern "C"{
#endif

/* Including necessary configuration files. */
#include "Mcal.h"
#include "Platform.h"
#include "CDD_Rm.h"
#include "ipc_shm_m7_0.h"

volatile uint8 exit_code;

int main(void)
{

	/* Setup interrupts */
	Platform_Init(NULL_PTR);
	/* Initialize RM CDD */
	Rm_Init(NULL_PTR);

	shm_demo();

	for (;;) {
		if (exit_code != 0) {
			break;
		}
	}
	return exit_code;
}

#if defined(__cplusplus)
}
#endif
