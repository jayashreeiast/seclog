/*=================================================================================================*/
/**
*   @file    startup_ghs.c
*   @version 0.4.4
*
*   @brief   Integration Framework - Startup file for ARM.
*   @details Startup file for ARM.
*     
*    (c) Copyright 2006-2016 Freescale Semiconductor, Inc. 
*       Copyright 2017-2019 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/       
/*=================================================================================================
*    Platform      :      CORTEXM
*    Build Version :      S32GXX_MCAL_0_4_4_EAR_ASR_REL_4_3_REV_0000_20190222
*
==================================================================================================*/
/*==================================================================================================
====================================================================================================*/

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

.globl CATCH_CORE_RESET
.globl _core_loop
.section ".core_loop","ax"
.thumb
_core_loop:
    nop
    nop
    nop
    nop
    b _core_loop 
    

.globl Reset_Handler 
Reset_Handler:
.globl _start
_start:
/*****************************************************/
/* Skip normal entry point as nothing is initialized */
/*****************************************************/
 mov   r0, #0
 mov   r1, #0
 mov   r2, #0
 mov   r3, #0
 mov   r4, #0
 mov   r5, #0
 mov   r6, #0
 mov   r7, #0

/*******************************************************************/
/* Freescale Guidance 1 - Init registers to avoid lock-step issues */
/* N/A                                                             */
/*******************************************************************/

/*******************************************************************/
/* Freescale Guidance 2 - MMU Initialization for CPU               */
/*  TLB0 - PbridgeB                                                */
/*  TLB1 - Internal Flash                                          */
/*  TLB2 - External SRAM                                           */
/*  TLB3 - Internal SRAM                                           */
/*  TLB4 - PbridgeA                                                */ 
/*******************************************************************/
    
/******************************************************************/
/* Autosar Guidance 1 - The start-up code shall initialize the    */
/* base addresses for interrupt and trap vector tables. These base*/
/* addresses are provided as configuration parameters or          */
/* linker/locator setting.                                        */
/******************************************************************/

/* relocate vector table to RAM */
ldr  r0, =VTOR_REG
ldr  r1, =VTABLE
ldr  r2, =(1 << 29)
orr  r1, r2 /* r1 = r1 | r2 */
str  r1,[r0]

/******************************************************************/
/* Autosar Guidance 2 - The start-up code shall initialize the    */
/* interrupt stack pointer, if an interrupt stack is              */
/* supported by the MCU. The interrupt stack pointer base address */
/* and the stack size are provided as configuration parameter or  */
/* linker/locator setting.                                        */
/*                                                                */
/******************************************************************/


/******************************************************************/
/* Autosar Guidance 3 - The start-up code shall initialize the    */
/* user stack pointer. The user stack pointer base address and    */
/* the stack size are provided as configuration parameter or      */
/* linker/locator setting.                                        */
/******************************************************************/

/* set up stack; r13 SP*/
ldr  r0, =_Stack_start
msr MSP, r0

/******************************************************************/
/* Autosar Guidance 4 - If the MCU supports context save          */
/* operation, the start-up code shall initialize the memory which */
/* is used for context save operation. The maximum amount of      */
/* consecutive context save operations is provided as             */
/* configuration parameter or linker/locator setting.             */
/*                                                                */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 5 - The start-up code shall ensure that the   */
/* MCU internal watchdog shall not be serviced until the watchdog */
/* is initialized from the MCAL watchdog driver. This can be      */
/* done for example by increasing the watchdog service time.      */
/*                                                                */
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
/* Autosar Guidance 6 - If the MCU supports cache memory for data */
/* and/or code, it shall be initialized and enabled in the        */
/* start-up code.                                                 */
/*                                                                */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 7 - The start-up code shall initialize MCU    */
/* specific features of internal memory like memory protection.   */
/*                                                                */
/******************************************************************/
 
/******************************************************************/
/* Autosar Guidance 8 - If external memory is used, the memory    */
/* shall be initialized in the start-up code. The start-up code   */
/* shall be prepared to support different memory configurations   */
/* depending on code location. Different configuration options    */
/* shall be taken into account for code execution from            */
/* external/internal memory.                                      */
/* N/A - external memory is not used                              */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 9 - The settings of the different memories    */
/* shall be provided to the start-up code as configuration        */
/* parameters.                                                    */
/* N/A - all memories are already configured                      */
/******************************************************************/
 
/******************************************************************/
/* Autosar Guidance 10 - In the start-up code a default           */
/* initialization of the MCU clock system shall be performed      */
/* including global clock prescalers.                             */
/******************************************************************/
bl SystemInit

/******************************************************************/
/* Autosar Guidance 5 - The start-up code shall ensure that the   */
/* MCU internal watchdog shall not be serviced until the watchdog */
/* is initialized from the MCAL watchdog driver. This can be      */
/* done for example by increasing the watchdog service time.      */
/*                                                                */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 11 - The start-up code shall enable           */
/* protection mechanisms for special function registers(SFR’s),   */
/* if supported by the MCU.                                       */
/* N/A - will be handled by Autosar OS                            */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 12 - The start-up code shall initialize all   */
/* necessary write once registers or registers common to several  */
/* drivers where one write, rather than repeated writes, to the   */
/* register is required or highly desirable.                      */
/******************************************************************/

/******************************************************************/
/* Autosar Guidance 13 - The start-up code shall initialize a     */
/* minimum amount of RAM in order to allow proper execution of    */
/* the MCU driver services and the caller of these services.      */
/******************************************************************/
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

/* Zero fill the non cacheable bss segment */
  ldr  r2, =OS_NONCACHE_BSS_START
  ldr  r4, =OS_NONCACHE_BSS_END

  mov   r3, #0
FillZeroNonCacheBss:

  str   r3, [r2]
  add   r2, r2, #4
  cmp   r2, r4
  bcc   FillZeroNonCacheBss
/*********************************/
/* Set the small rw data pointer */
/*********************************/

/******************************************************************/
/* Call Main Routine                                              */
/******************************************************************/
/* bl startup_go_to_user_mode */
#ifdef BOOTLOADER
	bl Bl_main
#else
	bl main
#endif
/******************************************************************/
/* Init runtime check data space                                  */
/******************************************************************/
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
.globl startup_getControlRegisterValue
startup_getControlRegisterValue:
mrs r0, CONTROL
bx r14

.globl startup_getAipsRegisterValue
startup_getAipsRegisterValue:
mrs r0, IPSR
bx r14
#endif
  

#if defined(MULTICORE)
    mov r0, #0
    bl  OSHWSynch
#endif

/* call application entry point */
bl main

b . /* in case main returns */

.endf Reset_Handler



.align 2
.globl Core_ID_1_Startup
Core_ID_1_Startup:

/* TBD - set SP regs as we run from RAM not after reset */

    /* ldr  SP, =_Stack2_start*/

    /* relocate vector table to RAM */
    ldr  r0, =VTOR_REG
    ldr  r1, =VTABLE
    ldr  r2, =(1 << 29)
    orr  r1, r2  /* r1 = r1 | r2 */
    str  r1,[r0]

#if defined(MULTICORE)
    mov r0, #0
    bl  OSHWSynch
#endif
        
    /* call application entry point */
    bl main

    b . /* in case main returns */

.endf Core_ID_1_Startup


.section .stack_main,"aw"
.align 3
#ifdef __STACK_SIZE
Stack_Size .equ __STACK_SIZE
#else
Stack_Size .equ  0x1000
#endif
.space Stack_Size


.section .stack2_main,"aw"
.align 3
#ifdef __STACK_SIZE
Stack2_Size .equ __STACK_SIZE
#else
Stack2_Size .equ  0x1000
#endif
.space Stack2_Size


