/*
 * Copyright 2025 NXP.
 */

#ifdef __cplusplus
extern "C" {
#endif


/* Generic Handler */
void undefined_handler(void);

/* EL3 Exception Table Handlers */
void vel3_curr_el_sp0_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_sp0_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_sp0_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_sp0_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_spx_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_spx_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_spx_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel3_curr_el_spx_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch64_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch64_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch64_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch64_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch32_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch32_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch32_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel3_lower_el_aarch32_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));

/* EL2 Exception Table Handlers */
void vel2_curr_el_sp0_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_sp0_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_sp0_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_sp0_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_spx_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_spx_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_spx_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel2_curr_el_spx_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch64_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch64_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch64_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch64_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch32_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch32_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch32_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel2_lower_el_aarch32_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));

/* EL1 Exception Table Handlers */
void vel1_curr_el_sp0_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_sp0_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_sp0_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_sp0_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_spx_sync_handler(void)        __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_spx_irq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_spx_fiq_handler(void)         __attribute__ ((weak, alias("undefined_handler")));
void vel1_curr_el_spx_serror_handler(void)      __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch64_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch64_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch64_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch64_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch32_sync_handler(void)   __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch32_irq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch32_fiq_handler(void)    __attribute__ ((weak, alias("undefined_handler")));
void vel1_lower_el_aarch32_serror_handler(void) __attribute__ ((weak, alias("undefined_handler")));


void undefined_handler(void)
{
    while(1){};
}

#ifdef __cplusplus
}
#endif
