/*
 * Copyright 2025 NXP.
 */
#ifndef LLCE_RTOS_TASK_H
#define LLCE_RTOS_TASK_H

#include "PlatformTypes.h"
#include "CanIf_Cbk.h"
#include "Llce_Init.h"


/* Priorities at which the tasks are created. */
#define LLCE_SEND_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS            pdMS_TO_TICKS( 200 )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH                       ( 1 )

/*
 * The tasks as described in the comments at the top of this file.
 */
void Llce_Rtos_Prepare(void);
void Llce_Send_Task(void);
/*-----------------------------------------------------------*/

#endif
