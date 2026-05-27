/**
 * @file      Startup_Vector_Table.s
 * @version   24.12.0
 *
 * @brief     The vector table definition
 * @details   This file contains the exception vectors mapping for all IRQs.
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

.section ".intc_vector", "ax"
.align 4
.thumb
.globl undefined_handler
.globl VTABLE
.globl __Stack_start       /* Initial stack pointer value. */
.globl Reset_Handler
.globl NMI_Handler
.globl HardFault_Handler
.globl MemManage_Handler
.globl BusFault_Handler
.globl UsageFault_Handler
.globl SVC_Handler
.globl DebugMon_Handler
.globl PendSV_Handler
.globl SysTick_Handler

VTABLE:
.long __Stack_start
.long Reset_Handler + 1    /* The reset handler must be written in Thumb. */
.long NMI_Handler
.long HardFault_Handler
.long MemManage_Handler
.long BusFault_Handler
.long UsageFault_Handler
.long 0                    /* Reserved */
.long 0                    /* Reserved */
.long 0                    /* Reserved */
.long 0                    /* Reserved */
.long SVC_Handler
.long DebugMon_Handler
.long 0                    /* Reserved */
.long PendSV_Handler
.long SysTick_Handler      /* 15 */

.long undefined_handler    /* 0 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 10 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 20 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 30 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 40 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 50 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 60 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 70 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 80 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 90 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 100 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 110 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 120 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 130 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 140 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 150 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 160 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 170 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 180 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 190 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 200 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 210 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 220 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 230 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler    /* 240 */
.long undefined_handler
