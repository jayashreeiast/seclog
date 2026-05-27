/**
*   @file      Saf_Stubs.h
*   @version   24.12.0
*
*   @brief     AUTOSAR Bootloader - Safety Stubs header file.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/

#ifndef SAF_STUBS_H
#define SAF_STUBS_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "StandardTypes.h"
#include "Bootloader_Cfg.h"

#if (STD_ON == BL_USE_SAFETY)

#include "Bist.h"

#include "sys_init.h"
#include "SafetyBase_PmicWrapper.h"

#include "mSel.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
/**
 * @brief Size of array for NCF Injected Faults.
 */
#define NCF_INJECTED_FAULTS_ARRAY_SIZE                 (6UL)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/**
 * @brief Structure to hold information about forwarding interrupts/exceptions to sCheck routines.
 */
typedef struct sCheck_FwdStruct
{
    /** @brief Exception Service Routine - Data Abort */
    sCheck_ExceptionHandlerType ESR_DA;
#if (SAFETY_BASE_PLATFORM_ARM == SAFETY_BASE_ARMV8_RARCH)
    /** @brief Exception Service Routine - Hypervisor call */
    sCheck_ExceptionHandlerType ESR_HYP;
#endif /*SAFETY_BASE_PLATFORM_ARM == SAFETY_BASE_ARMV8_RARCH*/
    /** @brief Interrupt Service Routine - SWT0 */
    sCheck_InterruptHandlerType ISR_SWT[8];

    /** @brief Interrupt Service Routine - SBSW */
    sCheck_InterruptHandlerType ISR_SBSW;

} sCheck_FwdType;

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
/**
@brief Hold information about forwarding interrupts/exceptions to sCheck routines.
 */
extern sCheck_FwdType sCheck_Fwd[SCHECK_CORE_NUM];

/**
@brief NCF faults to test transition into Degraded mode.
 */
extern uint8 u8NcfInjectedFault[NCF_INJECTED_FAULTS_ARRAY_SIZE];

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
/**
@brief Stub callback notification for BIST.
 */
void Bist_NotificationFunction(void);

/**
@brief Function used to get core Id.
 */
uint32 sys_getCoreId(void);

/**
@brief For S32XX derivatives that had configured Normal Mode
 */
Std_ReturnType mSel_UserInitNormal(void);

/**
@brief For S32XX derivatives that had configured Degraded Mode
 */
Std_ReturnType mSel_UserInitDegraded_0(void);

#if (STD_ON == MSEL_NVM_STORAGE)
/**
@brief For S32XX derivatives that had enabled NVM Storage the NVM read low level functions has to be implemented
 */
Std_ReturnType mSel_NvmRead(uint8 *data, const uint32 size, const mSel_NvmDataItemType Selection);

/**
@brief For S32XX derivatives that had enabled NVM Storage the NVM write low level functions has to be implemented
 */
Std_ReturnType mSel_NvmWrite(const uint8 *data, const uint32 size, const mSel_NvmDataItemType Selection);

#endif /*(STD_ON == MSEL_NVM_STORAGE)*/

/**
@brief Initialization of the array with NCF indexes injected by example demo code.
 */
void NcfInjectionInit(void);

/**
@brief      Execute NCF injection based on previously set NCF array.
*/
Std_ReturnType NcfInjectionExecution(void);

#endif /* (STD_ON == BL_USE_SAFETY) */

#ifdef __cplusplus
}
#endif

#endif /*SAF_STUBS_H*/

/** @} */
