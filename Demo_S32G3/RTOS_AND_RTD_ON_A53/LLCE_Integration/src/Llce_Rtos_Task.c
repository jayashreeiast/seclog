/*
 * Copyright 2025 NXP.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Llce_Rtos_Task.h"
#include "Llce_Firmware_Load.h"


static uint8 can_fd_data[CAN_FD_FRAME_LEN] __attribute__((aligned(4)));
static uint8 can_std_data[CAN_STD_FRAME_LEN] __attribute__((aligned(4)));
extern QueueHandle_t xQueue;
uint64_t ullErrorFound = pdFALSE;

void Llce_Rtos_Prepare(void)
{
    Std_ReturnType can_retval = E_NOT_OK;
    uint16 u16MbGlobalIndex = 0U;

    Llce_Firmware_Load();

    Can_Enable_Timestamp();
    
    /* LLCE GIC interrupt initialization */
    Can_Llce_Init_Irq();

    Can_43_LLCE_Init(&Can_43_LLCE_Config_VS_0);

    /* Reset global flags and counters. */
    Can_CallBackSetUp();

    /*Initialize transmission data buffer with a standard Can frame.*/
    for (u16MbGlobalIndex = 0; u16MbGlobalIndex < CAN_STD_FRAME_LEN; u16MbGlobalIndex++)
    {
        can_std_data[u16MbGlobalIndex] = u16MbGlobalIndex;
    }

    /*Initialize transmission data buffer with a Can Fd frame.*/
    for (u16MbGlobalIndex = 0; u16MbGlobalIndex < CAN_FD_FRAME_LEN; u16MbGlobalIndex++)
    {
        can_fd_data[u16MbGlobalIndex] = u16MbGlobalIndex;
    }

    (void)can_retval;

    can_retval = Can_43_LLCE_SetBaudrate(CONTROLLER1, BADU_500K_2M); /* Index 1 corresponds to a baudrate configuration of (500 kbps; 2000 kbps).See Tresos configuration*/
    can_retval = Can_43_LLCE_SetBaudrate(CONTROLLER2, BADU_500K_2M);
    can_retval = Can_43_LLCE_SetBaudrate(CONTROLLER3, BADU_500K_2M);
    can_retval = Can_43_LLCE_SetBaudrate(CONTROLLER4, BADU_500K_2M);
    /* Start CONTROLLER1 ~ CONTROLLER4 */
    can_retval = Can_43_LLCE_SetControllerMode(CONTROLLER1, CAN_CS_STARTED);
    can_retval = Can_43_LLCE_SetControllerMode(CONTROLLER2, CAN_CS_STARTED);
    can_retval = Can_43_LLCE_SetControllerMode(CONTROLLER3, CAN_CS_STARTED);
    can_retval = Can_43_LLCE_SetControllerMode(CONTROLLER4, CAN_CS_STARTED);

    Can_TxConfirmation = 0;
    Can_RxIndication = 0;
    Can_RxDlc = 0;

    last_RxIndication = 0;
    last_TxConfirmation = 0;
}


void Llce_Send_Task(void)
{
    uint32_t ulReceivedValue;
    Std_ReturnType can_retval = E_NOT_OK;
    Can_PduType CanMessage;
    const uint32_t ulExpectedValue = 100UL;
    /* Set-up CAN frame data */
    CanMessage.length = CAN_FD_FRAME_LEN;
    CanMessage.sdu = can_fd_data;
    CanMessage.swPduHandle = 2; //%16
    CanMessage.id = 15 | CAN_LPDU_FD_U32;

    for( ;; )
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        /* To get here something must have been received from the queue, but is
        it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            can_retval = Can_43_LLCE_Write(CanHO_Config1_TX2, &CanMessage);
            /* Check status of transmission and reception */
            can_retval = Check_Status(&CanMessage);
        }
        else
        {
            ullErrorFound |= 1;
        }
    }
    
    (void)can_retval;
}