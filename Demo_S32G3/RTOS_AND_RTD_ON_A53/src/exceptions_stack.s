/*
 * Copyright 2025 NXP.
 */

.section  ".systeminit","ax"


/******************************************************************************
 * The function called by the FreeRTOS_IRQ_Handler() to call the actual
 * peripheral handler.
 * 
 * In this version of FreeRTOS (10.4.6), vApplicationIRQHandler() must
 * read value from interrupt acknowledge register (ICCIAR), store and restore it
 * and write ICCIAR to the EOI register for End IRQ processing.
 * 
 * If the application provides an define of FPUSafeIRQ then vApplicationIRQHandler()
 * will save the FPU registers, and then call the actual peripheral handler.
 *****************************************************************************/
.balign 8
.weak vApplicationIRQHandler
vApplicationIRQHandler:
    stp x0, x1, [sp, #-0x10]!
    stp x2, x3, [sp, #-0x10]!
    stp x4, x5, [sp, #-0x10]!
    stp x6, x7, [sp, #-0x10]!
    stp x29, x30, [sp, #-0x10]!

#if defined( FPUSafeIRQ )
    /* save the FPU registers, and then call the actual peripheral handler. */
    stp Q0, Q1, [sp,#-0x20]!
    stp Q2, Q3, [sp,#-0x20]!
    stp Q4, Q5, [sp,#-0x20]!
    stp Q6, Q7, [sp,#-0x20]!
    stp Q8, Q9, [sp,#-0x20]!
    stp Q10, Q11, [sp,#-0x20]!
    stp Q12, Q13, [sp,#-0x20]!
    stp Q14, Q15, [sp,#-0x20]!
    stp Q16, Q17, [sp,#-0x20]!
    stp Q18, Q19, [sp,#-0x20]!
    stp Q20, Q21, [sp,#-0x20]!
    stp Q22, Q23, [sp,#-0x20]!
    stp Q24, Q25, [sp,#-0x20]!
    stp Q26, Q27, [sp,#-0x20]!
    stp Q28, Q29, [sp,#-0x20]!
    stp Q30, Q31, [sp,#-0x20]!
#endif

    /* Read ICC_IAR1 */
    dsb sy
    mrs x0, S3_0_C12_C12_0       /* ICC_IAR1_EL1 */
    cmp x0, #1020                /* Invalid IDs */
    b.ge end_of_app_irq_handler
    ldr x1, =VTABLE
    add x1, x1, x0, LSL #3       /* Calculate interrupt vector */
    ldr x2, [x1]
    stp x0, x1, [sp, #-0x10]!    /* Save ICC_IAR1 value */
    blr x2                       /* Jump to interrupt vector */
    msr DAIFSET, #2
    dsb sy
    isb sy
    ldp x0, x1, [sp], #0x10      /* Restore ICC_IAR1 value */

    /* Write to ICC_EOIR1 */
    msr S3_0_C12_C12_1, x0       /* ICC_EOIR1_EL1 */

end_of_app_irq_handler:
#if defined( FPUSafeIRQ )
    /* Restore the FPU context. */
    ldp Q30, Q31, [sp], #0x20
    ldp Q28, Q29, [sp], #0x20
    ldp Q26, Q27, [sp], #0x20
    ldp Q24, Q25, [sp], #0x20
    ldp Q22, Q23, [sp], #0x20
    ldp Q20, Q21, [sp], #0x20
    ldp Q18, Q19, [sp], #0x20
    ldp Q16, Q17, [sp], #0x20
    ldp Q14, Q15, [sp], #0x20
    ldp Q12, Q13, [sp], #0x20
    ldp Q10, Q11, [sp], #0x20
    ldp Q8, Q9, [sp], #0x20
    ldp Q6, Q7, [sp], #0x20
    ldp Q4, Q5, [sp], #0x20
    ldp Q2, Q3, [sp], #0x20
    ldp Q0, Q1, [sp], #0x20
#endif
    ldp x29, x30, [sp], #0x10
    ldp x6, x7, [sp], #0x10
    ldp x4, x5, [sp], #0x10
    ldp x2, x3, [sp], #0x10
    ldp x0, x1, [sp], #0x10
    br x30


.balign 8
.weak IRQ_Handler
IRQ_Handler:
    sub sp, sp, #256    /* SP must be 16-byte aligned */
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]

    /* Read ICC_IAR1 */
    dsb sy
    mrs  x0, S3_0_C12_C12_0      /* ICC_IAR1_EL1 */
    cmp  x0, #1020               /* Invalid IDs */
    b.ge end_of_irq_handler
    ldr  x1, =VTABLE
    add  x1, x1, x0, LSL #3      /* Calculate interrupt vector */
    ldr  x2, [x1]
    str  x0, [sp, #16 * 15 + 8]  /* Save ICC_IAR1 value */
    blr  x2                      /* Jump to interrupt vector */
    ldr  x0, [sp, #16 * 15 + 8]  /* Restore ICC_IAR1 value */

    /* Write to ICC_EOIR1 */
    msr S3_0_C12_C12_1, x0        /* ICC_EOIR1_EL1 */
    dsb sy

end_of_irq_handler:
    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]
    add sp, sp, #256
    eret


.balign 8
.weak FIQ_Handler
FIQ_Handler:
    sub sp, sp, #256    /* SP must be 16-byte aligned */
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]

    /* Read ICC_IAR0 */
    dsb sy
    mrs  x0, S3_0_C12_C8_0       /* ICC_IAR0_EL1 */
    cmp  x0, #1020               /* Invalid IDs */
    b.ge end_of_fiq_handler
    ldr  x1, =VTABLE
    add  x1, x1, x0, LSL #3      /* Calculate interrupt vector */
    ldr  x2, [x1]
    str  x0, [sp, #16 * 15 + 8]  /* Save ICC_IAR0 value */
    blr  x2                      /* Jump to interrupt vector */
    ldr  x0, [sp, #16 * 15 + 8]  /* Restore ICC_IAR0 value */

    /* Write to ICC_EOIR0 */
    msr S3_0_C12_C8_1, x0        /* ICC_EOIR0_EL1 */
    dsb sy

end_of_fiq_handler:
    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]
    add sp, sp, #256
    eret

