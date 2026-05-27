/*===================================================================================================*/
/*
*   @file              SysDal_Rtm_Types.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Types file for SysDal RTM component
*   @details           This file implements the SysDal RTM Types
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef SYSDAL_RTM_TYPES_H
#define SYSDAL_RTM_TYPES_H

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
#include "DatetimeLib.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define SYSDAL_RTM_TYPES_VENDOR_ID                            43

#define SYSDAL_RTM_TYPES_AR_RELEASE_MAJOR_VERSION             4
#define SYSDAL_RTM_TYPES_AR_RELEASE_MINOR_VERSION             4
#define SYSDAL_RTM_TYPES_AR_RELEASE_REVISION_VERSION          0

#define SYSDAL_RTM_TYPES_SW_MAJOR_VERSION                     24
#define SYSDAL_RTM_TYPES_SW_MINOR_VERSION                     12
#define SYSDAL_RTM_TYPES_SW_PATCH_VERSION                     0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if header file and SysDal_Rtm_Cfg header file are of the same vendor */
#if (SYSDAL_RTM_TYPES_VENDOR_ID != SYSDAL_RTM_CFG_VENDOR_ID)
    #error "SysDal_Rtm_Types.h and SysDal_Rtm_Cfg.h have different vendor ids"
#endif

/* Check if header file and SysDal_Rtm_Cfg header file are of the same Autosar version */
#if ((SYSDAL_RTM_TYPES_AR_RELEASE_MAJOR_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_RTM_TYPES_AR_RELEASE_MINOR_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_RTM_TYPES_AR_RELEASE_REVISION_VERSION != SYSDAL_RTM_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Rtm_Types.h and SysDal_Rtm_Cfg.h are different"
#endif

/* Check if header file and SysDal_Rtm_Cfg header file are of the same software version */
#if ((SYSDAL_RTM_TYPES_SW_MAJOR_VERSION != SYSDAL_RTM_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_RTM_TYPES_SW_MINOR_VERSION != SYSDAL_RTM_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_RTM_TYPES_SW_PATCH_VERSION != SYSDAL_RTM_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Rtm_Types.h and SysDal_Rtm_Cfg.h are different"
#endif

/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/**
* @brief        This enum contains types of measurements
*/
typedef enum
{
    RTM_MODE_ONESHOT = 0,                   /**< @brief One time measurement */
    RTM_MODE_CONTINUOUS                     /**< @brief Moving average measurement*/
} SysDal_Rtm_MeasModeType;

/**
* @brief        This enum defines possible states of measurement results
*/
typedef enum
{
    RTM_MEASUREMENT_INVALID = 0,            /**< @brief Measurement is disabled or is not completed yet */
    RTM_MEASUREMENT_COMPLETED,              /**< @brief Measurement is completed and new results are available */
    RTM_MEASUREMENT_OBSOLETE                /**< @brief Measurement is completed but the results were fetched before */
} SysDal_Rtm_MeasurementStatusType;

/**
* @brief          Timer time-point snapshot
* @details        Represents a time-point snapshot of a timer
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)      u32TimerOverflows;                  /**< @brief The timer overflows counter */
    VAR(uint32, AUTOMATIC)      u32TimerTicks;                      /**< @brief The timer ticks number */
} SysDal_Rtm_TimepointType;

/**
* @brief          Moving average data
* @details        This structure contains data necessary for the moving average calculation
*/
typedef struct
{
    VAR(uint64, AUTOMATIC)      aSeriesData[RTM_MOVAVG_SERIES_SIZE];/**< @brief Ring buffer to hold moving average series data */
    VAR(uint64, AUTOMATIC)      u64SeriesSum;                       /**< @brief Moving average series data sum */
    VAR(uint8, AUTOMATIC)       u8Index;                            /**< @brief Ring buffer index where next measurement will be stored */
    VAR(uint8, AUTOMATIC)       u8Counter;                          /**< @brief The number of measurements in this series */
} SysDal_Rtm_MovingAverageType;

/**
* @brief          This structure represents a measurement configuration
* @details        This structure will store data about measurement configuration
*/
typedef struct
{
    VAR(boolean, AUTOMATIC)                          bEnabled;      /**< @brief Measurement enable flag */
    VAR(SysDal_Rtm_TimepointType, AUTOMATIC)         Start;         /**< @brief Measurement start point */
    VAR(SysDal_Rtm_TimepointType, AUTOMATIC)         Stop;          /**< @brief Measurement stop point */
    VAR(SysDal_Rtm_MeasModeType, AUTOMATIC)          eMode;         /**< @brief Measurement mode */
    CONST(uint8, AUTOMATIC)                          u8ResultIndex; /**< @brief Measurement result index */
} SysDal_Rtm_MeasurementType;

/**
* @brief          This structure represents a Oneshot measurement
* @details        This structure will store data about Oneshot measurements result
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                           u32Id;         /**< @brief Measurement result ID */
    VAR(SysDal_Rtm_MeasurementStatusType, AUTOMATIC) Status;        /**< @brief Measurement result status */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                           u32Timestamp;  /**< @brief Measurement result timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(uint64, AUTOMATIC)                           u64Value;      /**< @brief Measurement result value */
} SysDal_Rtm_OneshotMeasurementType;

/**
* @brief          This structure represents a Continuous measurement
* @details        This structure will store data about Continuous measurements result
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                           u32Id;         /**< @brief Measurement result ID */
    VAR(SysDal_Rtm_MeasurementStatusType, AUTOMATIC) Status;        /**< @brief Measurement result status */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                           u32Timestamp;  /**< @brief Measurement result timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(uint64, AUTOMATIC)                           u64Value;      /**< @brief Measurement result value */
    VAR(uint64, AUTOMATIC)                           u64MinValue;   /**< @brief Measurement result min value */
    VAR(uint64, AUTOMATIC)                           u64MaxValue;   /**< @brief Measurement result max value */
} SysDal_Rtm_ContinuousMeasurementType;

/**
* @brief        This structure contains the result of a Oneshot RTM measurement
*
* @details      The measurement result contains time intervals which are expressed in engineering
*               units (hours, minutes, etc)
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                  u32Id;          /**< @brief The measurement ID */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                  u32Timestamp;   /**< @brief The measurement timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(Datetime_IntervalType, AUTOMATIC)   Value;          /**< @brief The measurement result value */
} SysDal_Rtm_OneshotResultType;

/**
* @brief        This structure contains the result of a Continuous RTM measurement
*
* @details      The measurement result contains time intervals which are expressed in engineering
*               units (hours, minutes, etc)
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                  u32Id;          /**< @brief The measurement ID */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                  u32Timestamp;   /**< @brief The measurement timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(Datetime_IntervalType, AUTOMATIC)   Value;          /**< @brief The measurement result value */
    VAR(Datetime_IntervalType, AUTOMATIC)   MinValue;       /**< @brief The measurement min value */
    VAR(Datetime_IntervalType, AUTOMATIC)   MaxValue;       /**< @brief The measurement max value */
} SysDal_Rtm_ContinuousResultType;

/**
* @brief        This structure contains the result of a Oneshot RTM measurement in ticks
*
* @details      The measurement result contains time intervals which are expressed in timer ticks
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                  u32Id;          /**< @brief The measurement ID */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                  u32Timestamp;   /**< @brief The measurement timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(uint64, AUTOMATIC)                  u64Value;       /**< @brief The measurement result value */
} SysDal_Rtm_OneshotTicksResultType;

/**
* @brief        This structure contains the result of a Continuous RTM measurement in ticks
*
* @details      The measurement result contains time intervals which are expressed in timer ticks
*/
typedef struct
{
    VAR(uint32, AUTOMATIC)                  u32Id;          /**< @brief The measurement ID */
#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    VAR(uint32, AUTOMATIC)                  u32Timestamp;   /**< @brief The measurement timestamp */
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */
    VAR(uint64, AUTOMATIC)                  u64Value;       /**< @brief The measurement result value */
    VAR(uint64, AUTOMATIC)                  u64MinValue;    /**< @brief The measurement min value */
    VAR(uint64, AUTOMATIC)                  u64MaxValue;    /**< @brief The measurement max value */
} SysDal_Rtm_ContinuousTicksResultType;


#ifdef __cplusplus
}
#endif

#endif /* SYSDAL_RTM_TYPES_H */
