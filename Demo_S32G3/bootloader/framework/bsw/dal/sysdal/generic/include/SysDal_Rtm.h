/*===================================================================================================*/
/*
*   @file              SysDal_Rtm.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Run Time Measurements SW component
*   @details           This application performs code flows execution measurements
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef SYSDAL_RTM_H
#define SYSDAL_RTM_H

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================*/
/*                                         INCLUDE FILES                                             
   1) system and project includes
   2) needed interfaces from external units
   3) internal and external interfaces from this unit
*/   
/*===================================================================================================*/
#include "SysDal_Rtm_Cfg.h"
#include "SysDal_Rtm_Types.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define SYSDAL_RTM_VENDOR_ID                            43

#define SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION             4
#define SYSDAL_RTM_AR_RELEASE_MINOR_VERSION             4
#define SYSDAL_RTM_AR_RELEASE_REVISION_VERSION          0

#define SYSDAL_RTM_SW_MAJOR_VERSION                     24
#define SYSDAL_RTM_SW_MINOR_VERSION                     12
#define SYSDAL_RTM_SW_PATCH_VERSION                     0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if header file and SysDal_Rtm_Cfg header file are of the same vendor */
#if (SYSDAL_RTM_VENDOR_ID != SYSDAL_RTM_CFG_VENDOR_ID)
    #error "SysDal_Rtm.h and SysDal_Rtm_Cfg.h have different vendor ids"
#endif

/* Check if header file and SYSDAL RTM types header file are of the same vendor */
#if (SYSDAL_RTM_VENDOR_ID != SYSDAL_RTM_TYPES_VENDOR_ID)
    #error "SysDal_Rtm.h and SysDal_Rtm_Types.h have different vendor ids"
#endif

/* Check if header file and SysDal_Rtm_Cfg header file are of the same Autosar version */
#if ((SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_MINOR_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_REVISION_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Rtm.h and SysDal_Rtm_Cfg.h are different"
#endif

/* Check if header file and SYSDAL RTM types header file are of the same Autosar version */
#if ((SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION != SYSDAL_RTM_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_MINOR_VERSION != SYSDAL_RTM_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_REVISION_VERSION != SYSDAL_RTM_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Rtm.h and StandardTypes.h are different"
#endif

/* Check if header file and SysDal_Rtm_Cfg header file are of the same software version */
#if ((SYSDAL_RTM_SW_MAJOR_VERSION != SYSDAL_RTM_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_RTM_SW_MINOR_VERSION != SYSDAL_RTM_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_RTM_SW_PATCH_VERSION != SYSDAL_RTM_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Rtm.h and SysDal_Rtm_Cfg.h are different"
#endif

/* Check if header file and SYSDAL RTM types header file are of the same software version */
#if ((SYSDAL_RTM_SW_MAJOR_VERSION != SYSDAL_RTM_TYPES_SW_MAJOR_VERSION) || \
     (SYSDAL_RTM_SW_MINOR_VERSION != SYSDAL_RTM_TYPES_SW_MINOR_VERSION) || \
     (SYSDAL_RTM_SW_PATCH_VERSION != SYSDAL_RTM_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Rtm.h and SysDal_Rtm_Types.h are different"
#endif

/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL CONSTANTS                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL VARIABLES                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL FUNCTIONS                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/**
* @brief      Initialize the RTM feature
* @details    The RTM module will configure the SysTick as default timer. If SysTick is not
*             available another GPT timer shall be configured.
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_Init(void);

/*===================================================================================================*/
/**
* @brief      De-initialize the RTM feature
* @details    De-initializes the used timers (SysTick/General purpose timers)
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_DeInit(void);

/*===================================================================================================*/
/**
* @brief      Start an RTM measurement
* @details    This function will mark the starting point of a measurement
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_StartMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId);

/*===================================================================================================*/
/**
* @brief      Stop an RTM measurement
* @details    This function will mark the stop point of a measurement
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_StopMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId);

/*===================================================================================================*/
/**
* @brief      Enable a measurement
* @details    The start/stop measurement routines will only capture the timestamp if the measurement is enabled
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_EnableMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId);

/*===================================================================================================*/
/**
* @brief      Disable a measurement
* @details    The start/stop measurement routines will only capture the timestamp if the measurement is enabled
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_DisableMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId);

/*===================================================================================================*/
/**
* @brief      Check if there are new results for a given measurement
* @details    This function returns the current result status for a given measurement
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     RTM_MEASUREMENT_INVALID - measurement is disabled or is not completed yet
*             RTM_MEASUREMENT_COMPLETED - measurement is completed and new results are available
*             RTM_MEASUREMENT_OBSOLETE - measurement is completed but the results were fetched before
*/
/*===================================================================================================*/
FUNC(SysDal_Rtm_MeasurementStatusType, AUTOMATIC) SysDal_Rtm_GetMeasurementStatus(VAR(uint8, AUTOMATIC) u8MeasurementId);

/*===================================================================================================*/
/**
* @brief      Get Oneshot measurement result
* @details    The result will be expressed in engineering units: hours, minutes, seconds,
*             milliseconds and microseconds
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* @param[out] pResult - pointer to measurement result structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_GetOneshotMeasResult(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                      P2VAR(SysDal_Rtm_OneshotResultType, AUTOMATIC, RTM_APPL_DATA) pResult);

/*===================================================================================================*/
/**
* @brief      Get Continuous measurement result
* @details    The result will be expressed in engineering units: hours, minutes, seconds,
*             milliseconds and microseconds
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* @param[out] pResult - pointer to measurement result structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_GetContinuousMeasResult(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                         P2VAR(SysDal_Rtm_ContinuousResultType, AUTOMATIC, RTM_APPL_DATA) pResult);

/*===================================================================================================*/
/**
* @brief      Get Oneshot measurement result in ticks
* @details    The time interval will be expressed in timer ticks
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* @param[out] pResult - pointer to measurement result structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_GetOneshotTicksResult(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                       P2VAR(SysDal_Rtm_OneshotTicksResultType, AUTOMATIC, RTM_APPL_DATA) pResult);

/*===================================================================================================*/
/**
* @brief      Get Continuous measurement result in ticks
* @details    The time interval will be expressed in timer ticks
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* @param[out] pResult - pointer to measurement result structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_GetContinuousTicksResult(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                          P2VAR(SysDal_Rtm_ContinuousTicksResultType, AUTOMATIC, RTM_APPL_DATA) pResult);

#ifdef __cplusplus
}
#endif

#endif /* SYSDAL_RTM_H */
