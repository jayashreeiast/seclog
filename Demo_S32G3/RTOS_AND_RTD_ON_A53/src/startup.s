/*
 * Copyright 2025 NXP.
 */

/* Secure Configuration Register */
.set SCR_EL3_EEL2   ,   (1 << 18)
.set SCR_EL3_TWE    ,   (1 << 13)
.set SCR_EL3_TWI    ,   (1 << 12)
.set SCR_EL3_ST     ,   (1 << 11)
.set SCR_EL3_RW     ,   (1 << 10)
.set SCR_EL3_SIF    ,   (1 << 9)
.set SCR_EL3_HCE    ,   (1 << 8)
.set SCR_EL3_SMD    ,   (1 << 7)
.set SCR_EL3_RES1_1 ,   (1 << 5)
.set SCR_EL3_RES1_0 ,   (1 << 4)
.set SCR_EL3_EA     ,   (1 << 3)
.set SCR_EL3_FIQ    ,   (1 << 2)
.set SCR_EL3_IRQ    ,   (1 << 1)
.set SCR_EL3_NS     ,   (1 << 0)

/* Hypervisor Configuration Register */
.set HCR_EL2_ID     ,   (1 << 33)
.set HCR_EL2_CD     ,   (1 << 32)
.set HCR_EL2_RW     ,   (1 << 31)
.set HCR_EL2_TRVM   ,   (1 << 30)
.set HCR_EL2_HVC    ,   (1 << 29)
.set HCR_EL2_TDZ    ,   (1 << 28)

/* EL2 System Control Register */
.set SCTLR_EL2_SA   ,   (1 << 3)

/* EL1 System Control Register */
.set SCTLR_EL1_NTWE     ,   (1 << 18)
.set SCTLR_EL1_NTWI     ,   (1 << 17)
.set SCTLR_EL1_CP15BEN  ,   (1 << 5)
.set SCTLR_EL1_SA0      ,   (1 << 4)
.set SCTLR_EL1_SA       ,   (1 << 3)



.section ".startup","ax"
.balign 8
/*****************************************************/
/* Clear Registers                                   */
/*****************************************************/
.globl Reset_Handler
.globl _start

_start:
Reset_Handler:
    /* Initialise the register bank */
    mov  x0,  xzr
    mov  x1,  xzr
    mov  x2,  xzr
    mov  x3,  xzr
    mov  x4,  xzr
    mov  x5,  xzr
    mov  x6,  xzr
    mov  x7,  xzr
    mov  x8,  xzr
    mov  x9,  xzr
    mov  x10, xzr
    mov  x11, xzr
    mov  x12, xzr
    mov  x13, xzr
    mov  x14, xzr
    mov  x15, xzr
    mov  x16, xzr
    mov  x17, xzr
    mov  x18, xzr
    mov  x19, xzr
    mov  x20, xzr
    mov  x21, xzr
    mov  x22, xzr
    mov  x23, xzr
    mov  x24, xzr
    mov  x25, xzr
    mov  x26, xzr
    mov  x27, xzr
    mov  x28, xzr
    mov  x29, xzr
    mov  x30, xzr

    fmov d0, xzr
    fmov d1, xzr
    fmov d2, xzr
    fmov d3, xzr
    fmov d4, xzr
    fmov d5, xzr
    fmov d6, xzr
    fmov d7, xzr
    fmov d8, xzr
    fmov d9, xzr
    fmov d10, xzr
    fmov d11, xzr
    fmov d12, xzr
    fmov d13, xzr
    fmov d14, xzr
    fmov d15, xzr
    fmov d16, xzr
    fmov d17, xzr
    fmov d18, xzr
    fmov d19, xzr
    fmov d20, xzr
    fmov d21, xzr
    fmov d22, xzr
    fmov d23, xzr
    fmov d24, xzr
    fmov d25, xzr
    fmov d26, xzr
    fmov d27, xzr
    fmov d28, xzr
    fmov d29, xzr
    fmov d30, xzr
    fmov d31, xzr

/******************************************************************/
/* Set stack pointers                                             */
/******************************************************************/
    ldr x0, =__Stack_start_c0
    mov SP, x0      /* SP_EL3 */
    msr SP_EL2, x0  /* msr S3_6_c4_c1_0, x0 */
    msr SP_EL1, x0  /* msr S3_4_c4_c1_0, x0 */
    msr SP_EL0, x0  /* msr S3_0_c4_c1_0, x0 */

    /* SPSel[SP] = PSTATE.SP = 1 - Use SP_ELx for ELx */
    msr SPSel, #1   /* msr S3_0_c4_c2_0, #1 */

/******************************************************************/
/* Initialize system registers                                    */
/******************************************************************/
/*
* Initialize HCR_EL2
* HCR_EL2[RW] = 1 - EL1 is in AArch64
* No traps or VM modifications
*/
    mov x0, #HCR_EL2_RW
    msr HCR_EL2, x0       /* msr S3_4_c1_c1_0, x0 */

/*
* Initialize SCTLR_EL2/1
* This is not strictly needed, since we never drop to EL2/1
*/
    mov x0, #SCTLR_EL2_SA
    msr SCTLR_EL2, x0     /* msr S3_4_c1_c0_0, x0 */

    mov x0,     #SCTLR_EL1_NTWE
    orr x0, x0, #SCTLR_EL1_NTWI
    orr x0, x0, #SCTLR_EL1_CP15BEN
    orr x0, x0, #SCTLR_EL1_SA0
    orr x0, x0, #SCTLR_EL1_SA
    msr SCTLR_EL1, x0     /* msr S3_0_c1_c0_0, x0 */

/******************************************************************/
/* Initialize the base addresses for interrupt and trap vector    */
/* tables. These base addresses are provided as configuration     */
/* parameters or linker/locator setting.                          */
/******************************************************************/
    /* Install the vector tables for EL3, EL2, EL1 */
    ldr x0, =ETABLE_EL3
    msr VBAR_EL3, x0      /* msr S3_6_c12_c0_0, x0 */

    ldr x0, =ETABLE_EL2
    msr VBAR_EL2, x0      /* msr S3_4_c12_c0_0, x0 */

    ldr x0, =ETABLE_EL1
    msr VBAR_EL1, x0      /* msr S3_0_c12_c0_0, x0 */

/*
* Configure SCR_EL3 - Asynchronous exceptions routing
* SCR_EL3[RW]  = 1 - The next lower level (EL2 if present, else EL1) is in AArch64
* SCR_EL3[EA]  = 1 - External Abort (SError) exceptions are always taken in EL3
* SCR_EL3[FIQ] = 1 - FIQ exceptions are always taken in EL3
* SCR_EL3[IRQ] = 1 - IRQ exceptions are always taken in EL3
* SCR_EL3[SMD] = 1 - UNDEFINE "Secure Monitor Call" instruction (i.e. the trap to EL3)
*                    HCR_EL2[TSC] has priority if it traps attempted EL1 execution of SMC instructions to EL2, when EL2 is enabled in the current Security state
*                    Note: SMC is always UNDEFINED at EL0
* SCR_EL3[NS]  = 1 - Place EL0 and EL1 in Non-secure state (i.e. cannot access Secure memory)
*                    Note: EL3 is always in Secure state, EL2 is always in Non-Secure state
*/
    mov x0,     #SCR_EL3_RW 
    orr x0, x0, #SCR_EL3_SMD 
    orr x0, x0, #SCR_EL3_EA 
    orr x0, x0, #SCR_EL3_FIQ 
    orr x0, x0, #SCR_EL3_IRQ 
    orr x0, x0, #SCR_EL3_NS
    msr SCR_EL3, x0         /* msr S3_6_c1_c1_0, x0 */


DisableA53_SWT3:
    ldr  x0, =0x4010C010  /* SWT_3_SR */
    ldr  w1, =0xC520
    str  w1, [x0]
    ldr  w1, =0xD928
    str  w1, [x0]
    ldr  x0, =0x4010C000
    ldr  w1, =0xFF000040
    str  w1, [x0]
    b    _INIT_DATA_BSS


/* Zero fill the bss segment */
_INIT_DATA_BSS:
    ldr  x0, =__bss_start__
    ldr  x1, =__bss_end__
    mov  x2, #0
1:
    str x2, [x0]
    add x0, x0, #8
    cmp x1, x2
    bcc 1b

__SYSTEM_INIT:
    bl SystemInit
    MOV   x0, #1              // NS=1
    BIC   x0, x0, #(1 << 1)   // IRQ=1 IRQs routed to EL1
    BIC   x0, x0, #(1 << 2)   // FIQ=1 FIQs routed to EL1
    BIC   x0, x0, #(1 << 3)   // EA=1  SError routed to EL1
    ORR   x0, x0, #(1 << 8)   // HCE=1 HVC instructions are enabled
    ORR   x0, x0, #(1 << 10)  // RW=1  Next EL down uses AArch64
    ORR   x0, x0, #(1 << 11)  // ST=1  Secure EL1 can access timers
    MSR   SCR_EL3, x0

    // Configure HCR_EL2
    // ------------------
    BIC   w0, wzr, #(1 << 3)       // FMO=1
    BIC   x0, x0, #(1 << 4)        // IMO=1
    ORR   x0, x0, #(1 << 31)       // RW=1     NS.EL1 is AArch64
                                    // TGE=0     Entry to NS.EL1 is possible
                                    // VM=0     Stage 2 MMU disabled
    MSR   HCR_EL2, x0
    // Set up VMPIDR_EL2/VPIDR_EL1
    // ---------------------------
    MRS   x0, MIDR_EL1
    MSR   VPIDR_EL2, x0
    MRS   x0, MPIDR_EL1
    MSR   VMPIDR_EL2, x0

    // Set VMID
    // ---------
    MSR   VTTBR_EL2, xzr
 
    // Set SCTLRs for EL1/2 to safe values
    // ------------------------------------
    MSR   SCTLR_EL2, xzr
    MSR   SCTLR_EL1, xzr
 
    ADR   x0, _MAIN
    MSR   ELR_EL3, x0
 
    mov   x0, 0x03c5
    MSR   spsr_el3, x0
    ERET

_MAIN:
    /* Clear DAIF bits to ensure interrupts are not masked */
    msr DAIFClr,#0xf
    bl MMU_Init
    bl main
