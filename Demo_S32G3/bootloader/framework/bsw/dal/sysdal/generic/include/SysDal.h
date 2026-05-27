/**
*   @file           SysDal.h
*   @version        24.12.0
*
*   @brief          Integration Framework - SysDal high level interface implementation.
*   @details        SysDal high level interface implementation.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup SYSDAL
*   @{
*/
/*==================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/
/*============================================================================================================================
==================================================================================================*/

#ifndef SYSDAL_H
#define SYSDAL_H

#ifdef __cplusplus
extern "C"{
#endif
/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "Mcal.h"

#include "SysDal_Cfg.h"
#include "SysDal_IntSch.h"
/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          SysDal.h
*/
#define SYSDAL_VENDOR_ID                           43
#define SYSDAL_MODULE_ID                           102

#define SYSDAL_AR_RELEASE_MAJOR_VERSION            4
#define SYSDAL_AR_RELEASE_MINOR_VERSION            4
#define SYSDAL_AR_RELEASE_REVISION_VERSION         0

#define SYSDAL_SW_MAJOR_VERSION                    24
#define SYSDAL_SW_MINOR_VERSION                    12
#define SYSDAL_SW_PATCH_VERSION                    0
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/**
* @file           SysDal.h
*/
/* Check if source file and SYSDAL header file are of the same vendor */
#if (SYSDAL_VENDOR_ID != SYSDAL_CFG_VENDOR_ID)
    #error "SysDal.h and SysDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and SYSDAL INTSCH header file are of the same vendor */
#if (SYSDAL_VENDOR_ID != SYSDAL_INTSCH_VENDOR_ID)
    #error "SysDal.h and SysDal_IntSch.h have different vendor ids"
#endif

/* Check if source file and SYSDAL header file are of the same Autosar version */
#if ((SYSDAL_AR_RELEASE_MAJOR_VERSION != SYSDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_AR_RELEASE_MINOR_VERSION != SYSDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_AR_RELEASE_REVISION_VERSION != SYSDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal.h and SysDal_Cfg.h are different"
#endif

/* Check if source file and SYSDAL INTSCH header file are of the same Autosar version */
#if ((SYSDAL_AR_RELEASE_MAJOR_VERSION != SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_AR_RELEASE_MINOR_VERSION != SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_AR_RELEASE_REVISION_VERSION != SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal.h and SysDal_IntSch.h are different"
#endif

/* Check if source file and SYSDAL header file are of the same Software version */
#if ((SYSDAL_SW_MAJOR_VERSION != SYSDAL_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_SW_MINOR_VERSION != SYSDAL_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_SW_PATCH_VERSION != SYSDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal.h and SysDal_Cfg.h are different"
#endif

/* Check if source file and SYSDAL INTSCH header file are of the same Software version */
#if ((SYSDAL_SW_MAJOR_VERSION != SYSDAL_INTSCH_SW_MAJOR_VERSION) || \
     (SYSDAL_SW_MINOR_VERSION != SYSDAL_INTSCH_SW_MINOR_VERSION) || \
     (SYSDAL_SW_PATCH_VERSION != SYSDAL_INTSCH_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal.h and SysDal_IntSch.h are different"
#endif

/*===================================================================================================================
                          GLOBAL TYPEDEFS
====================================================================================================================*/
/**
* @brief          Structure to define run States of the system
* @details        Data structure containing all the states the System can be. 
*/
typedef enum
{
    SYSDAL_STATE_OFF = 0,    /**< @brief PowerOff state of the system. */
    SYSDAL_STATE_RUN,        /**< @brief configured Run state of the system. */
    SYSDAL_STATE_SLEEP,      /**< @brief configured LowPower state of the system. */
    SYSDAL_STATE_DEEPSLEEP,  /**< @brief configured LowPower state of the system. */
    SYSDAL_STATE_STARTUP,    /**< @brief StartUp state of the system. */
    SYSDAL_STATE_WAKEUP,     /**< @brief WakeUp state of the system. */
    SYSDAL_STATE_SHUTDOWN    /**< @brief PowerDown state of the system. */
}SysDal_EcuModeType;

typedef void (*SysDal_CalloutType)(void);    /**< @brief pointer to configured collouts functions of SysDal. */

/**
* @brief          Structure to configure PWM internal channels
* @details        Data structure containing the callout functions of SysDal
*                 to enable the system and change the states
*/
typedef struct
{
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) InitBlockStartUpCallout;             /**< @brief callout function used to Init the System. */
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) InitBlockZeroCallout;             /**< @brief callout function used to Init the System. */
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) InitBlockOneCallout;              /**< @brief callout function used to Init the System. */
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) InterruptsEnableCallout;          /**< @brief callout function to set the interrupts used by the System. */                   
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) InterruptsDisableCallout;         /**< @brief disable the previously registered interrupts during init */
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) DeInitBlockOneCallout;            /**< @brief callout function used before going to sleep to deinit the system. */
#if (SYSDAL_LOW_POWER_MODE == STD_ON)
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) SetWakeupEventsCallout;           /**< @brief callout function used before going to sleep to set WakeUp sources. */
    CONST(SysDal_CalloutType, SAMPLE_APP_CONST) ClearWakeupEventsCallout;          /**< @brief callout function use at wakeup to clear WakeUp sources. */
#endif/* SYSDAL_LOW_POWER_MODE */

}SysDal_ConfigType;
/*===================================================================================================================
                                       LOCAL CONSTANTS
====================================================================================================================*/

/*===================================================================================================================
                                       LOCAL VARIABLES
====================================================================================================================*/


/*===================================================================================================================
                                       GLOBAL CONSTANTS
====================================================================================================================*/


/*===================================================================================================================
                                       GLOBAL VARIABLES
====================================================================================================================*/


/*===================================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
====================================================================================================================*/


/*===================================================================================================================
                                       LOCAL FUNCTIONS
====================================================================================================================*/

/*===================================================================================================================
                                       GLOBAL FUNCTIONS
====================================================================================================================*/
FUNC(void, SYSDAL_APP_CODE) SysDal_Cyclic_Activity(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_Init(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_DeInit(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_Main(void);

FUNC (void, SYSDAL_APP_CODE ) SysDal_WakeUpInit(void);

#if (SYSDAL_LOW_POWER_MODE == STD_ON)
FUNC(void, SYSDAL_APP_CODE) SysDal_WriteModeRequest(SysDal_EcuModeType Mode);
FUNC(SysDal_EcuModeType, SYSDAL_APP_CODE ) SysDal_GetCurrentPowerMode(void);
#endif /* SYSDAL_LOW_POWER_MODE == STD_ON */

FUNC(void, SYSDAL_APP_CODE) SysDal_ExecuteSwReset(void);

/* Mask the interrupts */
FUNC(void, SYSDAL_APP_CODE) SysDal_DisableAllInterrupts(void);

/* Unmask all interrupts */
FUNC(void, SYSDAL_APP_CODE) SysDal_EnableAllInterrupts(void);

/* Timeout for hardware modules initialization */
FUNC(void, SYSDAL_APP_CODE) SysDal_Wait(VAR(uint32, AUTOMATIC) SysDal_Timeout);

#if defined(AUTOSAR_OS_NOT_USED)
/* Disable all interrupts sources registered during init. */
FUNC(void, SYSDAL_APP_CODE) SysDal_DisableAllInterruptSources(void);
#endif /* defined(AUTOSAR_OS_NOT_USED) */

/*================================================================================================*/
/**
* @brief        Return the current system time in milliseconds
* @details      This function returns the system time based on internal scheduler timer.
*
* @return       The current system time in milliseconds
*/
/*================================================================================================*/
FUNC(uint32, SYSDAL_APP_CODE) SysDal_GetSystemTimeMs(void);
#ifdef __cplusplus
}
#endif

#endif /*SYSDAL_H*/
/** @} */
