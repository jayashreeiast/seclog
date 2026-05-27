/*
 * Copyright 2025 NXP.
 */

#ifndef GENERIC_TIMER_H
#define GENERIC_TIMER_H

#include <stdint.h>


#define CNTP_CTL_EL0_ENABLE_MASK     (1U)
#define CNTP_CTL_EL0_IMASK_MASK      (2U)


void write_CNTFRQ_EL0(uint64_t);
uint64_t read_CNTFRQ_EL0(void);
uint64_t read_CNTPCT_EL0(void);
void write_CNTKCTL_EL1(uint64_t);
uint64_t read_CNTKCTL_EL1(void);
void write_CNTP_CTL_EL0(uint64_t);
uint64_t read_CNTP_CTL_EL0(void);
void write_CNTP_CVAL_EL0(uint64_t);
uint64_t read_CNTP_CVAL_EL0(void);
void write_CNTP_TVAL_EL0(uint64_t);
uint64_t read_CNTP_TVAL_EL0(void);
void write_CNTPS_CTL_EL1(uint64_t);
uint64_t read_CNTPS_CTL_EL1(void);
void write_CNTPOFF_EL2(uint64_t);
uint64_t read_CNTPOFF_EL2(void);
void write_CNTPS_CVAL_EL1(uint64_t);
uint64_t read_CNTPS_CVAL_EL1(void);
void write_CNTPS_TVAL_EL1(uint64_t);
uint64_t read_CNTPS_TVAL_EL1(void);


#endif /* GENERIC_TIMER_H */
