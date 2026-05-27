/*
 * Copyright 2025 NXP.
 */

.section  ".systeminit","ax"

.balign 8
.globl read_SCTLR_EL3
read_SCTLR_EL3:
    mrs x0, SCTLR_EL3       /* mrs x0, S3_6_C1_C0_0 */
    ret

.balign 8
.globl write_SCTLR_EL3
write_SCTLR_EL3:
    msr SCTLR_EL3, x0       /* msr S3_6_C1_C0_0, x0 */
    ret

.balign 8
.globl read_SCR_EL3
read_SCR_EL3:
    mrs x0, SCR_EL3         /* mrs x0, S3_6_C1_C1_0 */
    ret

.balign 8
.globl write_SCR_EL3
write_SCR_EL3:
    msr SCR_EL3, x0         /* msr S3_6_C1_C1_0, x0 */
    ret

.balign 8
.globl read_CPUECTLR_EL1
read_CPUECTLR_EL1:
    mrs x0, S3_1_C15_C2_1   /* mrs x0, CPUECTLR_EL1 */
    ret

.balign 8
.globl write_CPUECTLR_EL1
write_CPUECTLR_EL1:
    msr S3_1_C15_C2_1, x0   /* msr CPUECTLR_EL1, x0 */
    ret

.balign 8
.globl read_CSSELR_EL1
read_CSSELR_EL1:
    mrs x0, CSSELR_EL1      /* mrs x0, S3_2_C0_C1_0 */
    ret

.balign 8
.globl write_CSSELR_EL1
write_CSSELR_EL1:
    msr CSSELR_EL1, x0      /* msr S3_2_C0_C1_0, x0 */
    ret

.balign 8
.globl read_CCSIDR_EL1
read_CCSIDR_EL1:
    mrs x0, CCSIDR_EL1      /* mrs x0, S3_1_C0_C0_0 */
    ret

.balign 8
.globl write_DCCISW
write_DCCISW:
    DC CISW, x0
    ret

.balign 8
.globl write_ICIALLU
write_ICIALLU:
    IC IALLU
    ret

.balign 8
.globl write_CPTR_EL3
write_CPTR_EL3:
    msr CPTR_EL3, x0        /* msr S3_6_C1_C1_2, x0 */
    ret

.balign 8
.globl write_CPTR_EL2
write_CPTR_EL2:
    msr CPTR_EL2, x0        /* msr S3_0_C1_C0_2, x0 */
    ret

.balign 8
.globl write_CPACR_EL1
write_CPACR_EL1:
    msr CPACR_EL1, x0       /* msr S3_5_C1_C0_2, x0 */
    ret

.balign 8
.globl write_MAIR_EL3
write_MAIR_EL3:
    msr MAIR_EL3, x0        /* msr S3_6_C10_C2_0, x0 */
    ret

.balign 8
.globl write_TCR_EL3
write_TCR_EL3:
    msr TCR_EL3, x0         /* msr S3_6_C2_C0_2, x0 */
    ret

.balign 8
.globl write_TTBR0_EL3
write_TTBR0_EL3:
    msr TTBR0_EL3, x0       /* msr S3_6_C2_C0_0, x0 */
    ret

.balign 8
.globl read_MPIDR_EL1
read_MPIDR_EL1:
    mrs x0, MPIDR_EL1       /* mrs x0, S3_0_C0_C0_5 */
    ret

.balign 8
.globl read_ICC_SRE_EL1
read_ICC_SRE_EL1:
    mrs x0, S3_0_C12_C12_5  /* mrs x0, ICC_SRE_EL1 */
    ret

.balign 8
.globl write_ICC_SRE_EL1
write_ICC_SRE_EL1:
    msr S3_0_C12_C12_5, x0  /* msr ICC_SRE_EL1, x0 */
    ret

.balign 8
.globl read_ICC_SRE_EL2
read_ICC_SRE_EL2:
    mrs x0, S3_4_C12_C9_5   /* mrs x0, ICC_SRE_EL2 */
    ret

.balign 8
.globl write_ICC_SRE_EL2
write_ICC_SRE_EL2:
    msr S3_4_C12_C9_5, x0   /* msr ICC_SRE_EL2, x0 */
    ret

.balign 8
.globl read_ICC_SRE_EL3
read_ICC_SRE_EL3:
    mrs x0, S3_6_C12_C12_5  /* mrs x0, ICC_SRE_EL3 */
    ret

.balign 8
.globl write_ICC_SRE_EL3
write_ICC_SRE_EL3:
    msr S3_6_C12_C12_5, x0  /* msr ICC_SRE_EL3, x0 */
    ret

.balign 8
.globl read_ICC_PMR_EL1
read_ICC_PMR_EL1:
    mrs x0, S3_0_C4_C6_0    /* mrs x0, ICC_PMR_EL1 */
    ret

.balign 8
.globl write_ICC_PMR_EL1
write_ICC_PMR_EL1:
    msr S3_0_C4_C6_0, x0    /* msr ICC_PMR_EL1, x0 */
    ret

.balign 8
.globl read_ICC_BPR0_EL1
read_ICC_BPR0_EL1:
    mrs x0, S3_0_C12_C8_3   /* mrs x0, ICC_BPR0_EL1 */
    ret

.balign 8
.globl write_ICC_BPR0_EL1
write_ICC_BPR0_EL1:
    msr S3_0_C12_C8_3, x0   /* msr ICC_BPR0_EL1, x0 */
    ret

.balign 8
.globl read_ICC_BPR1_EL1
read_ICC_BPR1_EL1:
    mrs x0, S3_0_C12_C12_3  /* mrs x0, ICC_BPR1_EL1 */
    ret

.balign 8
.globl write_ICC_BPR1_EL1
write_ICC_BPR1_EL1:
    msr S3_0_C12_C12_3, x0  /* msr ICC_BPR1_EL1, x0 */
    ret

.balign 8
.globl read_ICC_IGRPEN0_EL1
read_ICC_IGRPEN0_EL1:
    mrs x0, S3_0_C12_C12_6  /* mrs x0, ICC_IGRPEN0_EL1 */
    ret

.balign 8
.globl write_ICC_IGRPEN0_EL1
write_ICC_IGRPEN0_EL1:
    msr S3_0_C12_C12_6, x0  /* msr ICC_IGRPEN0_EL1, x0 */
    ret

.balign 8
.globl read_ICC_IGRPEN1_EL3
read_ICC_IGRPEN1_EL3:
    mrs x0, S3_6_C12_C12_7  /* mrs x0, ICC_IGRPEN1_EL3 */
    ret

.balign 8
.globl write_ICC_IGRPEN1_EL3
write_ICC_IGRPEN1_EL3:
    msr S3_6_C12_C12_7, x0  /* msr ICC_IGRPEN1_EL3, x0 */
    ret

.balign 8
.globl read_ICC_CTLR_EL3
read_ICC_CTLR_EL3:
    mrs x0, S3_6_C12_C12_4  /* mrs x0, ICC_CTLR_EL3 */
    ret

.balign 8
.globl write_ICC_CTLR_EL3
write_ICC_CTLR_EL3:
    msr S3_6_C12_C12_4, x0  /* msr ICC_CTLR_EL3, x0 */
    ret

.balign 8
.globl write_ICC_SGI0R_EL1
write_ICC_SGI0R_EL1:
    msr S3_0_C12_C11_7, x0  /* msr ICC_SGI0R_EL1, x0 */
    ret

.balign 8
.globl write_ICC_SGI1R_EL1
write_ICC_SGI1R_EL1:
    msr S3_0_C12_C11_5, x0  /* msr ICC_SGI1R_EL1, x0 */
    ret

.balign 8
.globl write_ICC_ASGI1R_EL1
write_ICC_ASGI1R_EL1:
    msr S3_0_C12_C11_6, x0  /* msr ICC_ASGI1R_EL1, x0 */
    ret


/* Generic Timer */

.balign 8
.globl write_CNTFRQ_EL0
write_CNTFRQ_EL0:
    msr CNTFRQ_EL0, x0  /* msr S3_3_C14_C0_0, x0 */
    ret

.balign 8
.globl read_CNTFRQ_EL0
read_CNTFRQ_EL0:
    mrs x0, CNTFRQ_EL0  /* mrs x0, S3_3_C14_C0_0 */
    ret

.balign 8
.globl read_CNTPCT_EL0
read_CNTPCT_EL0:
    isb
    mrs x0, CNTPCT_EL0  /* mrs x0, S3_3_C14_C0_1 */
    ret

.balign 8
.globl write_CNTKCTL_EL1
write_CNTKCTL_EL1:
    msr CNTKCTL_EL1, x0  /* msr S3_0_C14_C1_0, x0 */
    ret

.balign 8
.globl read_CNTKCTL_EL1
read_CNTKCTL_EL1:
    mrs x0, CNTKCTL_EL1  /* mrs x0, S3_0_C14_C1_0 */
    ret

.balign 8
.globl write_CNTP_CTL_EL0
write_CNTP_CTL_EL0:
    msr CNTP_CTL_EL0, x0  /* msr S3_3_C14_C2_1, x0 */
    ret

.balign 8
.globl read_CNTP_CTL_EL0
read_CNTP_CTL_EL0:
    mrs x0, CNTP_CTL_EL0  /* mrs x0, S3_3_C14_C2_1 */
    ret

.balign 8
.globl write_CNTP_CVAL_EL0
write_CNTP_CVAL_EL0:
    msr CNTP_CVAL_EL0, x0  /* msr S3_3_C14_C2_2, x0 */
    ret

.balign 8
.globl read_CNTP_CVAL_EL0
read_CNTP_CVAL_EL0:
    mrs x0, CNTP_CVAL_EL0  /* mrs x0, S3_3_C14_C2_2 */
    ret

.balign 8
.globl write_CNTP_TVAL_EL0
write_CNTP_TVAL_EL0:
    msr CNTP_TVAL_EL0, x0  /* msr S3_3_C14_C2_0, x0 */
    ret

.balign 8
.globl read_CNTP_TVAL_EL0
read_CNTP_TVAL_EL0:
    mrs x0, CNTP_TVAL_EL0  /* mrs x0, S3_3_C14_C2_0 */
    ret

.balign 8
.globl write_CNTPS_CTL_EL1
write_CNTPS_CTL_EL1:
    msr CNTPS_CTL_EL1, x0  /* msr S3_7_C14_C2_1, x0 */
    ret

.balign 8
.globl read_CNTPS_CTL_EL1
read_CNTPS_CTL_EL1:
    mrs x0, CNTPS_CTL_EL1  /* mrs x0, S3_7_C14_C2_1 */
    ret

.balign 8
.globl write_CNTPOFF_EL2
write_CNTPOFF_EL2:
    msr S3_4_C14_C0_6, x0  /* msr CNTPOFF_EL2, x0 */
    ret

.balign 8
.globl read_CNTPOFF_EL2
read_CNTPOFF_EL2:
    mrs x0, S3_4_C14_C0_6  /* mrs x0, CNTPOFF_EL2 */
    ret

.balign 8
.globl write_CNTPS_CVAL_EL1
write_CNTPS_CVAL_EL1:
    msr CNTPS_CVAL_EL1, x0  /* msr S3_7_C14_C2_2, x0 */
    ret

.balign 8
.globl read_CNTPS_CVAL_EL1
read_CNTPS_CVAL_EL1:
    mrs x0, CNTPS_CVAL_EL1  /* mrs x0, S3_7_C14_C2_2 */
    ret

.balign 8
.globl write_CNTPS_TVAL_EL1
write_CNTPS_TVAL_EL1:
    msr CNTPS_TVAL_EL1, x0  /* msr S3_7_C14_C2_0, x0 */
    ret

.balign 8
.globl read_CNTPS_TVAL_EL1
read_CNTPS_TVAL_EL1:
    mrs x0, CNTPS_TVAL_EL1  /* mrs x0, S3_7_C14_C2_0 */
    ret

