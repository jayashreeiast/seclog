/*=================================================================================================*/
/**
*   @file    Startup.s
*   @version 24.12.0
*
*   @brief   Integration Framework - Startup file for ARM.
*   @details Startup file for ARM.
*     
*    (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/       
/*=================================================================================================
*    Platform      :      CORTEXM
*    Build Version :      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
==================================================================================================*/

.globl VTABLE
.section ".startup","ax"
.thumb
/************************************************************************/
/* Autosar synopsis of startup code (See MCU Specification):            */
/*                                                                      */
/*   Before the MCU driver can be initialized, a basic initialization   */
/*   of the MCU has to be executed. This MCU specific initialization is */
/*   typically executed in a start-up code. The start-up code of the    */
/*   MCU shall be executed after power up and any kind of micro-        */
/*   controller reset. It shall perform very basic and microcontroller  */
/*   specific start-up initialization and shall be kept short, because  */
/*   the MCU clock and PLL is not yet initialized. The start-up code    */
/*   shall cover MCU specific initialization, which is not part of      */
/*   other MCU services or other MCAL drivers. The following steps      */
/*   summarizes basic functionality which shall be included in the      */
/*   start-up code. They are listed for guidance, because some          */
/*   functionality might not be supported. No code will be found in     */
/*   case.                                                              */
/************************************************************************/
.set VTOR_REG, 0xE000ED08
.align 2
.globl RESET_CATCH_CORE

.globl Reset_Handler 
Reset_Handler:
.globl _start
_start:
/*****************************************************/
/* Clear General Purpose Registers                   */
/*****************************************************/
 mov   r0, #0
 mov   r1, #0
 mov   r2, #0
 mov   r3, #0
 mov   r4, #0
 mov   r5, #0
 mov   r6, #0
 mov   r7, #0
 

/******************************************************************/
/* Set vector table address in RAM                                */
/******************************************************************/
ldr  r0, =VTOR_REG
ldr  r1, =VTABLE
ldr  r2, =(1 << 29)
orr  r1, r2 /* r1 = r1 | r2 */
str  r1,[r0]


/******************************************************************/
/* Set stack pointer                                              */
/******************************************************************/
ldr  r0, =_Stack_start
msr MSP, r0

/******************************************************************/
/* Disable platform watchdogs                                     */
/******************************************************************/
/* disable SWT0 */
ldr  r0, =0x40100010
ldr  r1, =0xC520
str  r1, [r0]
ldr  r1, =0xD928
str  r1, [r0]
ldr  r0, =0x40100000
ldr  r1, =0xFF000040
str  r1, [r0]

/* disable SWT1 */
ldr  r0, =0x40104010
ldr  r1, =0xC520
str  r1, [r0]
ldr  r1, =0xD928
str  r1, [r0]
ldr  r0, =0x40104000
ldr  r1, =0xFF000040
str  r1, [r0]

/* disable SWT2 */
ldr  r0, =0x40108010
ldr  r1, =0xC520
str  r1, [r0]
ldr  r1, =0xD928
str  r1, [r0]
ldr  r0, =0x40108000
ldr  r1, =0xFF000040
str  r1, [r0]

/* disable SWT3 */
ldr  r0, =0x4010C010
ldr  r1, =0xC520
str  r1, [r0]
ldr  r1, =0xD928
str  r1, [r0]
ldr  r0, =0x4010C000
ldr  r1, =0xFF000040
str  r1, [r0]


/******************************************************************/
/* Initialize the system (MPU, interrupts)                        */
/******************************************************************/
bl SystemInit


/************************/
/* Erase ".bss Section" */
/************************/

/* Zero fill the bss segment */
  ldr  r2, =BSS_SRAM_START
  ldr  r4, =BSS_SRAM_END

  mov   r3, #0
FillZerobss:
  str   r3, [r2]
  add   r2, r2, #4
  cmp   r2, r4
  bcc   FillZerobss

/******************************************************************/
/* Call Main Routine                                              */
/******************************************************************/
bl main

