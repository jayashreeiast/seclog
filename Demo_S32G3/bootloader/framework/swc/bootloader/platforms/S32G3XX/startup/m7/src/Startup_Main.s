/**
 * @file      Startup_Main.s
 * @version   24.12.0
 *
 * @brief     Startup entry-point file
 * @details   Contains the core startup logic, i.e. vector table relocation,
 *            stack pointer initialization, disabling the software watchdog
 *            for the current core, system and memory initialization,
 *            and jumping to the application main.
 *
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

/* Register holding the logical id of the accessing core. */
#define MSCM_CPXNUM_REG      0x40198004

/* Logical IDs for the M7 cores. */
/* Only M7_0 is supported as primary boot target. */
#define MSCM_CPXNUM_M7_0     0x4

/* Cortex-M7 vector table offset register. */
#define M7_VTOR_TABLE_OFFSET 0xE000ED08

/* The watchdog service code sequence step 1 to disable Soft Lock. */
#define SWT_WSC_UNLOCK_SEQ_1 0xC520
/* The watchdog service code sequence step 2 to disable Soft Lock. */
#define SWT_WSC_UNLOCK_SEQ_2 0xD928
/* The watchdog service code sequence step 2 to disable Soft Lock. */
#define SWT_CR_DISABLE_VAL   0xFF000040

/* Control register address for SWT_0. */
#define SWT_0_CR_ADDR        0x40100000
/* Status register address for SWT_0. */
#define SWT_0_SR_ADDR        0x40100010

/* DTCM control register address. */
#define CM7_DTCM_CR          0xE000EF94

/* Table, having the linker-defined symbols, for copying and zeroing. */
.section ".init_table", "a"
.thumb
.align 2
  .long 0 /* The C-mapped array size. */
.section ".zero_table", "a"
.thumb
.align 2
  .long 2 /* The C-mapped array size. */
  .long __BSS_SRAM_NC_START
  .long __BSS_SRAM_NC_END
  .long __BSS_SRAM_START
  .long __BSS_SRAM_END

/* .startup section, containing the main routines. */
/* This will, in turn, keep the reset handler address constant. */
.section ".startup","ax"
#ifdef GCC
/* Required for the instructions setting flags (s suffix) */
/* that wouldn't be recognized with Thumb by the GNU assembler. */
.syntax unified
#endif
.thumb
.align 2

.globl Reset_Handler
Reset_Handler:

/* Zero out the low registers and disable interrupts. */
  cpsid i
  mov   r0, #0
  mov   r1, #0
  mov   r2, #0
  mov   r3, #0
  mov   r4, #0
  mov   r5, #0
  mov   r6, #0
  mov   r7, #0

/******************************************************************/
/* Autosar Guidance 1 - The start-up code shall initialize the    */
/* base addresses for interrupt and trap vector tables. These base*/
/* addresses are provided as configuration parameters or          */
/* linker/locator setting.                                        */
/******************************************************************/
/* Vector table relocation to RAM */
  ldr   r0, =M7_VTOR_TABLE_OFFSET
  ldr   r1, =__RAM_INTERRUPT_START
  str   r1, [r0]

/******************************************************************/
/* Autosar Guidance 3 - The start-up code shall initialize the    */
/* user stack pointer. The user stack pointer base address and    */
/* the stack size are provided as configuration parameter or      */
/* linker/locator setting.                                        */
/******************************************************************/
/* Set the main stack pointer to the stack start. */
  ldr   r0, =__Stack_start
  msr   MSP, r0

/* Determine the current core that is being started. */
/* Then, disable the software watchdog for the current core. */
  ldr   r0, =MSCM_CPXNUM_REG
  ldr   r1, [r0]
  ldr   r0, =MSCM_CPXNUM_M7_0
  cmp   r1, r0
  beq   Disable_SWT0

/* Only M7_0 is allowed. */
  b     Invalid_Boot_Core_Loop

/******************************************************************/
/* Autosar Guidance 5 - The start-up code shall ensure that the   */
/* MCU internal watchdog shall not be serviced until the watchdog */
/* is initialized from the MCAL watchdog driver. This can be      */
/* done for example by increasing the watchdog service time.      */
/******************************************************************/
/* Note from manual: For any operation to be performed on an SWT */
/* instance, its respective core must be enabled. */
Disable_SWT0:
/* Firstly, disable the Soft Lock via the SR register */
/* and then disable the watchdog. */
  ldr   r0, =SWT_0_SR_ADDR
  ldr   r1, =SWT_WSC_UNLOCK_SEQ_1
  str   r1, [r0]
  ldr   r1, =SWT_WSC_UNLOCK_SEQ_2
  str   r1, [r0]
  ldr   r0, =SWT_0_CR_ADDR
  ldr   r1, =SWT_CR_DISABLE_VAL
  str   r1, [r0]

/******************************************************************/
/* Autosar Guidance 13 - The start-up code shall initialize a     */
/* minimum amount of RAM in order to allow proper execution of    */
/* the MCU driver services and the caller of these services.      */
/******************************************************************/
Sram_Init:
  ldr   r0, =__RAM_INIT
  cmp   r0, #0
/* Skip SRAM initialization if it's is done through other means (i.e. DCD). */
  beq   SRAM_Loop_End

/* Check whether the SRAM memory range is valid. */
  ldr   r1, =__INT_SRAM_START
  ldr   r2, =__INT_SRAM_END
  subs  r2, r1
  subs  r2, #1
  ble   SRAM_Loop_End

  movs  r0, #0
  movs  r3, #0
SRAM_Loop:
/* Zero-out the available SRAM range (given via linker script). */
  stm   r1!, {r0, r3}
  subs  r2, #8
  bge   SRAM_Loop
SRAM_Loop_End:

DTCM_Init:
/* Initialize DTCM */
  ldr   r0, =__DTCM_INIT
  cmp   r0, #0
/* Skip SRAM initialization if it's is done through other means (ie DCD). */
  beq    DTCM_Loop_End

/* Enable DTCM */
  ldr   r1, =CM7_DTCM_CR
  ldr   r0, [r1]
  ldr   r2, =0x1
  orr   r0, r2
  str   r0, [r1]

  ldr   r1, =__INT_DTCM_START
  ldr   r2, =__INT_DTCM_END

/* Check whether the DTCM memory range is valid. */
  subs  r2, r1
  subs  r2, #1
  ble   DTCM_Loop_End

  movs  r0, #0
  movs  r3, #0
DTCM_Loop:
/* Zero-out the available DTCM range (given via linker script). */
  stm   r1!, {r0,r3}
  subs  r2, #8
  bge   DTCM_Loop
DTCM_Loop_End:

  bl Startup_Init_Memory
  bl Startup_Init_System
  bl main

/* BootROM allows only M7_0 to be used for loading an image. */
Invalid_Boot_Core_Loop:
  b .

.align 4

/* Assemble the literal pool. */
.ltorg
