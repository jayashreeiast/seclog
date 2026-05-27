/*
 * Copyright 2025 NXP.
 */
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Llce_Rtos_Task.h"
#include "Ipcf_Rtos_Task.h"
#include "Pfe_Rtos_Task.h"

uint64_t ui32_ms_cnt = 0;
QueueHandle_t xQueue = NULL;
void Create_RTD_Task(void);
int main(void)
{   
    /* Wait for Master initialization and bridge configuration to complete. */
    while(hal_read32(PFE_MASTER_READY_ADDRESS) != PFE_MASTER_READY_VALUE);

    /* LLCE initialization. */
    Llce_Rtos_Prepare();
    /* IPCF initialization. */
    Ipcf_Rtos_Prepare();
    /* PFE initialization. */
    Pfe_Rtos_PrePare();
    /* Create LLCE, IPCF and PFE RTOS task. */
    Create_RTD_Task();
}



void Create_RTD_Task(void)
{
    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

    if(xQueue != NULL)
    {
        xTaskCreate((TaskFunction_t) Llce_Send_Task, 
            "Llce_Send_Task", 
            configMINIMAL_STACK_SIZE, 
            NULL, 
            LLCE_SEND_TASK_PRIORITY,
            NULL );

        xTaskCreate((TaskFunction_t) Ipcf_Send_Task,
            "Ipcf_Send_Task",
            configMINIMAL_STACK_SIZE,
            NULL,
            IPCF_SEND_TASK_PRIORITY,
            NULL );

        xTaskCreate((TaskFunction_t) Pfe_Slave_Ping_Task,
            "Pfe_Slave_Ping_Task",
            configMINIMAL_STACK_SIZE,
            NULL,
            IPCF_SEND_TASK_PRIORITY,
            NULL );
            /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }
    for( ;; );
}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
    ui32_ms_cnt ++;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace;

    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}
/*-----------------------------------------------------------*/
