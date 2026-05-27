/*
 * Copyright 2025 NXP.
 */

.section  ".exc_vector","ax"
/*
 * The FreeRTOS IRQ handler, which is implemented in the RTOS port layer.
 */
.extern FreeRTOS_IRQ_Handler 
.extern FreeRTOS_SWI_Handler 

.set freertos_vector_base, _freertos_vector_table
/* .set vel3_curr_el_sp0_sync_handler, FreeRTOS_SWI_Handler */
/* .set vel3_curr_el_spx_sync_handler, FreeRTOS_SWI_Handler */
/* .set vel2_curr_el_sp0_sync_handler, FreeRTOS_SWI_Handler */
/* .set vel2_curr_el_spx_sync_handler, FreeRTOS_SWI_Handler */
.set vel1_curr_el_sp0_sync_handler, FreeRTOS_SWI_Handler 
.set vel1_curr_el_spx_sync_handler, FreeRTOS_SWI_Handler 

/* .set vel3_curr_el_sp0_irq_handler,  FreeRTOS_IRQ_Handler */
/* .set vel3_curr_el_spx_irq_handler,  FreeRTOS_IRQ_Handler */
/* .set vel2_curr_el_sp0_irq_handler,  FreeRTOS_IRQ_Handler */
/* .set vel2_curr_el_spx_irq_handler,  FreeRTOS_IRQ_Handler */
.set vel1_curr_el_sp0_irq_handler,  FreeRTOS_IRQ_Handler 
.set vel1_curr_el_spx_irq_handler,  FreeRTOS_IRQ_Handler

/* ======================= EXCEPTION VECTOR TABLE - EL 3 ======================= */
.balign 2048
.global ETABLE_EL3

ETABLE_EL3:

.balign 128
vel3_curr_el_sp0_sync:
    b vel3_curr_el_sp0_sync_handler

.balign 128
vel3_curr_el_sp0_irq:
    b vel3_curr_el_sp0_irq_handler

.balign 128
vel3_curr_el_sp0_fiq:
    b vel3_curr_el_sp0_fiq_handler

.balign 128
vel3_curr_el_sp0_serror:
    b vel3_curr_el_sp0_serror_handler

.balign 128
vel3_curr_el_spx_sync:
    b vel3_curr_el_spx_sync_handler

.balign 128
vel3_curr_el_spx_irq:
    b vel3_curr_el_spx_irq_handler

.balign 128
vel3_curr_el_spx_fiq:
    b vel3_curr_el_spx_fiq_handler

.balign 128
vel3_curr_el_spx_serror:
    b vel3_curr_el_spx_serror_handler

.balign 128
vel3_lower_el_aarch64_sync:
    b vel3_lower_el_aarch64_sync_handler

.balign 128
vel3_lower_el_aarch64_irq:
    b vel3_lower_el_aarch64_irq_handler

.balign 128
vel3_lower_el_aarch64_fiq:
    b vel3_lower_el_aarch64_fiq_handler

.balign 128
vel3_lower_el_aarch64_serror:
    b vel3_lower_el_aarch64_serror_handler

.balign 128
vel3_lower_el_aarch32_sync:
    b vel3_lower_el_aarch32_sync_handler

.balign 128
vel3_lower_el_aarch32_irq:
    b vel3_lower_el_aarch32_irq_handler

.balign 128
vel3_lower_el_aarch32_fiq:
    b vel3_lower_el_aarch32_fiq_handler

.balign 128
vel3_lower_el_aarch32_serror:
    b vel3_lower_el_aarch32_serror_handler

.balign 128
.size ETABLE_EL3, . - ETABLE_EL3


/* ======================= EXCEPTION VECTOR TABLE - EL 2 ======================= */
.balign 2048
.global	ETABLE_EL2
ETABLE_EL2:

.balign 128
vel2_curr_el_sp0_sync:
    b vel2_curr_el_sp0_sync_handler

.balign 128
vel2_curr_el_sp0_irq:
    b vel2_curr_el_sp0_irq_handler

.balign 128
vel2_curr_el_sp0_fiq:
    b vel2_curr_el_sp0_fiq_handler

.balign 128
vel2_curr_el_sp0_serror:
    b vel2_curr_el_sp0_serror_handler

.balign 128
vel2_curr_el_spx_sync:
    b vel2_curr_el_spx_sync_handler

.balign 128
vel2_curr_el_spx_irq:
    b vel2_curr_el_spx_irq_handler

.balign 128
vel2_curr_el_spx_fiq:
    b vel2_curr_el_spx_fiq_handler

.balign 128
vel2_curr_el_spx_serror:
    b vel2_curr_el_spx_serror_handler

.balign 128
vel2_lower_el_aarch64_sync:
    b vel2_lower_el_aarch64_sync_handler

.balign 128
vel2_lower_el_aarch64_irq:
    b vel2_lower_el_aarch64_irq_handler

.balign 128
vel2_lower_el_aarch64_fiq:
    b vel2_lower_el_aarch64_fiq_handler

.balign 128
vel2_lower_el_aarch64_serror:
    b vel2_lower_el_aarch64_serror_handler

.balign 128
vel2_lower_el_aarch32_sync:
    b vel2_lower_el_aarch32_sync_handler

.balign 128
vel2_lower_el_aarch32_irq:
    b vel2_lower_el_aarch32_irq_handler

.balign 128
vel2_lower_el_aarch32_fiq:
    b vel2_lower_el_aarch32_fiq_handler

.balign 128
vel2_lower_el_aarch32_serror:
    b vel2_lower_el_aarch32_serror_handler

.balign 128
.size ETABLE_EL2, . - ETABLE_EL2


/* ======================= EXCEPTION VECTOR TABLE - EL1 ======================= */
.balign 4096
.global	ETABLE_EL1
.globl _freertos_vector_table
.set _freertos_vector_table, ETABLE_EL1

ETABLE_EL1:

.balign 128
vel1_curr_el_sp0_sync:
    b vel1_curr_el_sp0_sync_handler

.balign 128
vel1_curr_el_sp0_irq:
    b vel1_curr_el_sp0_irq_handler

.balign 128
vel1_curr_el_sp0_fiq:
    b vel1_curr_el_sp0_fiq_handler

.balign 128
vel1_curr_el_sp0_serror:
    b vel1_curr_el_sp0_serror_handler

.balign 128
vel1_curr_el_spx_sync:
    b vel1_curr_el_spx_sync_handler

.balign 128
vel1_curr_el_spx_irq:
    b vel1_curr_el_spx_irq_handler

.balign 128
vel1_curr_el_spx_fiq:
    b vel1_curr_el_spx_fiq_handler

.balign 128
vel1_curr_el_spx_serror:
    b vel1_curr_el_spx_serror_handler

.balign 128
vel1_lower_el_aarch64_sync:
    b vel1_lower_el_aarch64_sync_handler

.balign 128
vel1_lower_el_aarch64_irq:
    b vel1_lower_el_aarch64_irq_handler

.balign 128
vel1_lower_el_aarch64_fiq:
    b vel1_lower_el_aarch64_fiq_handler

.balign 128
vel1_lower_el_aarch64_serror:
    b vel1_lower_el_aarch64_serror_handler

.balign 128
vel1_lower_el_aarch32_sync:
    b vel1_lower_el_aarch32_sync_handler

.balign 128
vel1_lower_el_aarch32_irq:
    b vel1_lower_el_aarch32_irq_handler

.balign 128
vel1_lower_el_aarch32_fiq:
    b vel1_lower_el_aarch32_fiq_handler

.balign 128
vel1_lower_el_aarch32_serror:
    b vel1_lower_el_aarch32_serror_handler

.balign 128
.size ETABLE_EL1, . - ETABLE_EL1


/* ======================= INTERRUPT VECTOR TABLE - COMMON ======================= */
.section  ".intc_vector","aw"

.balign 4096
.globl VTABLE
VTABLE:
.dword undefined_handler /*0*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*10*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*20*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*30*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler /*40*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler
.dword undefined_handler /*50*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*60*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*70*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*80*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*90*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler 
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*100*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*110*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*120*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*130*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*140*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*150*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*160*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*170*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*180*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*190*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*200*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*210*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*220*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*230*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*240*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*250*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*260*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*270*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*280*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*290*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*300*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*310*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*320*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*330*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*340*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*350*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*360*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*370*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*380*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*390*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*400*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*410*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*420*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*430*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*440*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*450*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*460*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*470*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*480*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*490*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*500*/
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler
.dword undefined_handler /*510*/
.dword undefined_handler

.size VTABLE, . - VTABLE
