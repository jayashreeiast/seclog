/**
 * @file      Startup_Exceptions.c
 * @version   24.12.0
 *
 * @brief     The exception vectors definition
 *
 *            Platform     : CORTEXM
 *            Build Version: S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
 *
 * @copyright (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *                All Rights Reserved.
 *
 *            This file contains sample code only.
 *            It is not part of the production code deliverables.
 */

#include "Platform_Types.h"
#include "Mcal.h"

void NMI_Handler(void);        /* NMI Handler */
void HardFault_Handler(void);  /* Hard Fault Handler */
void MemManage_Handler(void);  /* Reserved */
void BusFault_Handler(void);   /* Bus Fault Handler */
void UsageFault_Handler(void); /* Usage Fault Handler */
void SVC_Handler(void);        /* SVCall Handler */
void DebugMon_Handler(void);   /* Debug Monitor Handler */
void PendSV_Handler(void);     /* PendSV Handler */
void SysTick_Handler(void);    /* SysTick Handler */
void undefined_handler(void);  /* Undefined Handler */

void NMI_Handler(void)
{
    while (1);
}

void HardFault_Handler(void)
{
    while (1);
}

void MemManage_Handler(void)
{
    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void)
{
    while (1);
}

void DebugMon_Handler(void)
{
    while (1);
}

void PendSV_Handler(void)
{
    while (1);
}

#if (defined ENABLE_RUNTIME_MEASUREMENTS) || \
    (defined SYS_TICK_RUNTIME_MEASUREMENTS)
extern void SysDal_Rtm_EventHandler(void);
#endif /* (defined ENABLE_RUNTIME_MEASUREMENTS) ||
          (defined SYS_TICK_RUNTIME_MEASUREMENTS) */
void SysTick_Handler(void)
{
#if (defined ENABLE_RUNTIME_MEASUREMENTS) || \
    (defined SYS_TICK_RUNTIME_MEASUREMENTS)
    SysDal_Rtm_EventHandler();
#else
    while(1);
#endif /* (defined ENABLE_RUNTIME_MEASUREMENTS) ||
          (defined SYS_TICK_RUNTIME_MEASUREMENTS) */
}

void undefined_handler(void)
{
    while (1);
}
