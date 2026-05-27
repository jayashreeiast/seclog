/**
*   @file      interrupts.h
*   @version   24.12.0
*
*   @brief     AUTOSAR Bootloader - interrupts header file.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Bootloader_Cfg.h"

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#if (STD_ON == BL_USE_SAFETY)
#include "SafetyBase_Cfg.h"

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

#define INT_NUM_SOURCES (224)

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** @brief Pointer to interrupt service routine */
typedef void (*int_function_t)(void);

/** @brief Pointer to exception handler */
typedef void (*exc_function_t)(void);

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
/**
@brief      FCCU Alarm Interrupt routine.
 */
void FCCU_ALARM_ISR(void);
/**
@brief      FCCU Configuration Timeout ISR.
 */
void FCCU_TIMEOUT_ISR(void);
/**
@brief Interrupt handler for core-to-core lock-request/release interrupts
 */
void ISR_LockManager(void);

/**
@brief      Interrupt handler for SWT0 IRQ
 */
void ISR_SWT0(void);
/**
@brief      Interrupt handler for SWT1 IRQ
 */
void ISR_SWT1(void);
/**
@brief      Interrupt handler for SWT2 IRQ
 */
void ISR_SWT2(void);
/**
@brief      Interrupt handler for SWT3 IRQ
 */
void ISR_SWT3(void);
/**
@brief      Interrupt handler for SWT4 IRQ
 */
void ISR_SWT4(void);
/**
@brief      Interrupt handler for SWT5 IRQ
 */
void ISR_SWT5(void);
/**
@brief      Interrupt handler for SWT6 IRQ
 */
void ISR_SWT6(void);
/**
@brief      Interrupt handler for SWT7 IRQ
 */
void ISR_SWT7(void);
/**
@brief Interrupt handler for SBSW IRQ
 */
void ISR_SBSW(void);
/**
 * @brief Interrupt handler for EDMA_0 CHANNELS 0-15
 */
void ISR_EDMA_0_CH0_15(void);
/**
 * @brief Interrupt handler for EDMA_1 CHANNELS 0-15
 */
void ISR_EDMA_1_CH0_15(void);
/**
 * @brief Interrupt service routine for incoming sCheck events
 */
void ISR_sCheckEvent(void);

#endif /* STD_ON == BL_USE_SAFETY */

#ifdef __cplusplus
}
#endif

#endif /*INTERRUPTS_H */

/** @} */
