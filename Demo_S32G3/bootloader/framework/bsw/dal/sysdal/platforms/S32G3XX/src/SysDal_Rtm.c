/*===================================================================================================*/
/*
*   @file              SysDal_Rtm.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Run Time Measurements SW component
*   @details           This application performs code flows execution measurement
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

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
#include "SysDal_Rtm.h"

#if (SYSDAL_RTM_ENABLED == STD_ON)

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
#include "SysDal.h"
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

#include "SysDal_Timer.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define SYSDAL_RTM_VENDOR_ID_C                              43

#define SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION_C               4
#define SYSDAL_RTM_AR_RELEASE_MINOR_VERSION_C               4
#define SYSDAL_RTM_AR_RELEASE_REVISION_VERSION_C            0

#define SYSDAL_RTM_SW_MAJOR_VERSION_C                       24
#define SYSDAL_RTM_SW_MINOR_VERSION_C                       12
#define SYSDAL_RTM_SW_PATCH_VERSION_C                       0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if source file and SYSDAL RTM header file are of the same vendor */
#if (SYSDAL_RTM_VENDOR_ID_C != SYSDAL_RTM_VENDOR_ID)
    #error "SysDal_Rtm.c and SysDal_Rtm.h have different vendor ids"
#endif

/* Check if source file and SYSDAL RTM header file are of the same Autosar version */
#if ((SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_MINOR_VERSION_C != SYSDAL_RTM_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_REVISION_VERSION_C != SYSDAL_RTM_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Rtm.c and SysDal_Rtm.h are different"
#endif

/* Check if source file and SYSDAL RTM header file are of the same Software version */
#if ((SYSDAL_RTM_SW_MAJOR_VERSION_C != SYSDAL_RTM_SW_MAJOR_VERSION) || \
     (SYSDAL_RTM_SW_MINOR_VERSION_C != SYSDAL_RTM_SW_MINOR_VERSION) || \
     (SYSDAL_RTM_SW_PATCH_VERSION_C != SYSDAL_RTM_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Rtm.c and SysDal_Rtm.h are different"
#endif

/* Check if source file and SYSDAL RTM configuration header file are of the same vendor */
#if (SYSDAL_RTM_VENDOR_ID_C != SYSDAL_RTM_CFG_VENDOR_ID)
    #error "SysDal_Rtm.c and SysDal_Rtm_Cfg.h have different vendor ids"
#endif

/* Check if source file and SYSDAL RTM configuration header file are of the same Autosar version */
#if ((SYSDAL_RTM_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_MINOR_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_RTM_AR_RELEASE_REVISION_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Rtm.c and SysDal_Rtm_Cfg.h are different"
#endif

/* Check if source file and SYSDAL RTM configuration header file are of the same software version */
#if ((SYSDAL_RTM_SW_MAJOR_VERSION_C != SYSDAL_RTM_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_RTM_SW_MINOR_VERSION_C != SYSDAL_RTM_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_RTM_SW_PATCH_VERSION_C != SYSDAL_RTM_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Rtm.c and SysDal_Rtm_Cfg.h are different"
#endif

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/* Define maximum value for a moving average */
#define RTM_MOVAVG_MAX_VALUE            0xFFFFFFFFFFFFFFFFULL

/* Define minimum value for a moving average */
#define RTM_MOVAVG_MIN_VALUE            0x00ULL

/*===================================================================================================*/
/*                                             LOCAL VARIABLES                                       */
/*===================================================================================================*/

/* Runtime measurements ID counter */
static VAR(uint32, AUTOMATIC) SysDal_Rtm_u32IdCounter = 0;

/*===================================================================================================*/
/*                                            GLOBAL VARIABLES                                       */ 
/*===================================================================================================*/

/* List of configuread RTM measurements */ 
extern VAR(SysDal_Rtm_MeasurementType, AUTOMATIC) SysDal_Rtm_Measurements[];

#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
/* Result buffers for Oneshot measurements */
static VAR(SysDal_Rtm_OneshotMeasurementType, AUTOMATIC) SysDal_Rtm_OneshotResults[SYSDAL_RTM_ONESHOT_MEAS_NUMBER] = {0};
#endif /* SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U */

#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
/* Result buffers for Continuous measurements */
static VAR(SysDal_Rtm_ContinuousMeasurementType, AUTOMATIC) SysDal_Rtm_ContinuousResults[SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER] = {0};

/* List of moving averages */
static VAR(SysDal_Rtm_MovingAverageType, AUTOMATIC) SysDal_Rtm_aMovingAverages[SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER] = {0};
#endif /* (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */

/*===================================================================================================*/
/*                                    LOCAL FUNCTION PROTOTYPES                                      */
/*===================================================================================================*/

static FUNC(void, AUTOMATIC) SysDal_Rtm_GetTotalTicksByMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                                   P2VAR(uint64, AUTOMATIC, RTM_APPL_DATA) pTotalTicks);
static FUNC(void, AUTOMATIC) SysDal_Rtm_GetTotalTicksByTimepoint(P2VAR(SysDal_Rtm_TimepointType, AUTOMATIC, RTM_APPL_DATA) pTimepoint,
                                                                 P2VAR(uint64, AUTOMATIC, RTM_APPL_DATA) pTotalTicks);
                                                                 
#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
static FUNC(void, AUTOMATIC) SysDal_Rtm_SaveResult_OneShot(VAR(uint8, AUTOMATIC) u8MeasurementId);
#endif /* SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U */

#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
static FUNC(void, AUTOMATIC) SysDal_Rtm_SaveResult_MovAvg(VAR(uint8, AUTOMATIC) u8MeasurementId);
#endif /* (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */

static FUNC(void, AUTOMATIC) SysDal_Rtm_TicksToDatetime(VAR(uint64, AUTOMATIC) u64Ticks,
                                                        P2VAR(Datetime_IntervalType, AUTOMATIC, RTM_APPL_DATA) pDatetime);

/*===================================================================================================*/
/*                                      LOCAL FUNCTIONS                                              */
/*===================================================================================================*/


/*===================================================================================================*/
/**
* @brief      Convert an RTM time interval in timer ticks
* @details    The conversion is done taking into account the timer clock frequency and timer period
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* @param[out] pTotalTicks - total number of ticks
* 
* @return     None
*/
/*===================================================================================================*/
static FUNC(void, AUTOMATIC) SysDal_Rtm_GetTotalTicksByMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId,
                                                                   P2VAR(uint64, AUTOMATIC, RTM_APPL_DATA) pTotalTicks)
{
    VAR(uint64, AUTOMATIC) u64StartTicks;
    VAR(uint64, AUTOMATIC) u64StopTicks;

    /* Convert timer snapshots to ticks */
    SysDal_Rtm_GetTotalTicksByTimepoint(&SysDal_Rtm_Measurements[u8MeasurementId].Start, &u64StartTicks);
    SysDal_Rtm_GetTotalTicksByTimepoint(&SysDal_Rtm_Measurements[u8MeasurementId].Stop, &u64StopTicks);

    /* Get the delta value between start and stop */
    *pTotalTicks = u64StopTicks - u64StartTicks;
}


/*===================================================================================================*/
/**
* @brief      Convert a timer timepoint in timer ticks
* @details    The conversion is done based on the timer period
* 
* @param[in]  pTimepoint - Pointer to a timer timepoint
* @param[out] pTotalTicks - Total number of ticks
* 
* @return     None
*/
/*===================================================================================================*/
static FUNC(void, AUTOMATIC) SysDal_Rtm_GetTotalTicksByTimepoint(P2VAR(SysDal_Rtm_TimepointType, AUTOMATIC, RTM_APPL_DATA) pTimepoint,
                                                                 P2VAR(uint64, AUTOMATIC, RTM_APPL_DATA) pTotalTicks)
{
    *pTotalTicks = (uint64)(pTimepoint->u32TimerOverflows + 1) * (uint64)SYSDAL_RTM_TIMER_PERIOD - pTimepoint->u32TimerTicks;
}


#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
/*===================================================================================================*/
/**
* @brief      Save oneshot measurement results
* @details    The result will be saved in local RTM data structure and can be provided on request
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
static FUNC(void, AUTOMATIC) SysDal_Rtm_SaveResult_OneShot(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    VAR(uint64, AUTOMATIC) u64CurrentMeasurementTicks;
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_OneshotMeasurementType, AUTOMATIC, RTM_APPL_DATA) pResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    pResult = &SysDal_Rtm_OneshotResults[pMeasurement->u8ResultIndex];

    /* Save measurement ID */
    SysDal_Rtm_u32IdCounter++;
    pResult->u32Id = SysDal_Rtm_u32IdCounter;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
    /* Save measurement timestamp*/
    pResult->u32Timestamp = SysDal_GetSystemTimeMs();
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

    /* Save oneshot measurement result */
    SysDal_Rtm_GetTotalTicksByMeasurement(u8MeasurementId, &u64CurrentMeasurementTicks);
    pResult->u64Value = u64CurrentMeasurementTicks;

    /* Mark the measurement as new */
    pResult->Status = RTM_MEASUREMENT_COMPLETED;

    /* This is a oneshot measurement, disable here the measurement */
    pMeasurement->bEnabled = FALSE;
}
#endif /* (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U) */


#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
/*===================================================================================================*/
/**
* @brief      Save results for moving average measurements
* @details    The result will be saved in local RTM data structure and can be provided on request.
*             Moving average formula: average value of the last RTM_MOVAVG_SERIES_SIZE measurements.
*             E.g.: RTM_MOVAVG_SERIES_SIZE = 4; MA = (m1 + m2 + m3 + m4) / 4
*             On each measurement, we subtract the first value and add the new measurement value.
*             MA = [(m1 + m2 + m3 + m4) - m1 + m5 ] / 4
*             The values are stored in a ring buffer.
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
static FUNC(void, AUTOMATIC) SysDal_Rtm_SaveResult_MovAvg(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    VAR(uint64, AUTOMATIC) u64CurrentMeasurementTicks;
    VAR(uint64, AUTOMATIC) u64FirstMeasurementTicks;
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_MovingAverageType, AUTOMATIC, RTM_APPL_DATA) pMovAvg;
    P2VAR(SysDal_Rtm_ContinuousMeasurementType, AUTOMATIC, RTM_APPL_DATA) pResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];

    pMovAvg = &SysDal_Rtm_aMovingAverages[pMeasurement->u8ResultIndex];

    /* Get measurement result */
    SysDal_Rtm_GetTotalTicksByMeasurement(u8MeasurementId, &u64CurrentMeasurementTicks);

    /* Get first measurement in series */
    u64FirstMeasurementTicks = pMovAvg->aSeriesData[pMovAvg->u8Index];
    
    /* Replace first measurement in series with the new measurement */
    pMovAvg->aSeriesData[pMovAvg->u8Index] = u64CurrentMeasurementTicks;
    
    /* Increment ring buffer index of series data */
    pMovAvg->u8Index = (pMovAvg->u8Index + 1) % RTM_MOVAVG_SERIES_SIZE;

    /* Update the sum of measurements in series */
    pMovAvg->u64SeriesSum += (u64CurrentMeasurementTicks - u64FirstMeasurementTicks);

    /* Increase measurements counter only up to RTM_MOVAVG_SERIES_SIZE */
    if (pMovAvg->u8Counter < RTM_MOVAVG_SERIES_SIZE)
    {
        pMovAvg->u8Counter += 1;
    }

    if (RTM_MOVAVG_SERIES_SIZE == pMovAvg->u8Counter)
    {
        /* Get the pointer to the result structure */
        pResult = &SysDal_Rtm_ContinuousResults[pMeasurement->u8ResultIndex];

        /* Save measurement ID */
        SysDal_Rtm_u32IdCounter++;
        pResult->u32Id = SysDal_Rtm_u32IdCounter;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
        /* Save measurement timestamp */
        pResult->u32Timestamp = SysDal_GetSystemTimeMs();
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

        /* Save measurement moving average */
        pResult->u64Value = pMovAvg->u64SeriesSum / RTM_MOVAVG_SERIES_SIZE;

        /* Save minimum moving average value */
        if (pResult->u64Value < pResult->u64MinValue)
        {
            pResult->u64MinValue = pResult->u64Value;
        }

        /* Save maximum moving average value */
        if (pResult->u64Value > pResult->u64MaxValue)
        {
            pResult->u64MaxValue = pResult->u64Value;
        }

        /* Mark the measurement as new */
        pResult->Status = RTM_MEASUREMENT_COMPLETED;
    }
}
#endif /*(SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */


/*===================================================================================================*/
/**
* @brief      Convert ticks to datetime interval type
* @details    This function converts a given number of timer ticks into engineering units (hours, minutes, etc)
*             based on the timer frequency.
*
* @param[in]  u64Ticks - the given number of timer ticks
* @param[out] pDatetime - the resulted datetime interval
* 
* @return     None
*/
/*===================================================================================================*/
static FUNC(void, AUTOMATIC) SysDal_Rtm_TicksToDatetime(VAR(uint64, AUTOMATIC) u64Ticks,
                                                        P2VAR(Datetime_IntervalType, AUTOMATIC, RTM_APPL_DATA) pDatetime)
{
    pDatetime->u16Hours = u64Ticks / SYSDAL_RTM_TICKS_PER_HOUR;

    u64Ticks = u64Ticks % SYSDAL_RTM_TICKS_PER_HOUR;
    pDatetime->u8Minutes = u64Ticks / SYSDAL_RTM_TICKS_PER_MINUTE;

    u64Ticks = u64Ticks % SYSDAL_RTM_TICKS_PER_MINUTE;
    pDatetime->u8Seconds = u64Ticks / SYSDAL_RTM_TICKS_PER_SECONDS;

    u64Ticks = u64Ticks % SYSDAL_RTM_TICKS_PER_SECONDS;
    pDatetime->u16Milliseconds = u64Ticks / SYSDAL_RTM_TICKS_PER_MILLISECONDS;

    u64Ticks = u64Ticks % SYSDAL_RTM_TICKS_PER_MILLISECONDS;
    pDatetime->u16Microseconds = u64Ticks / SYSDAL_RTM_TICKS_PER_MICROSECONDS;
}


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
FUNC(void, AUTOMATIC) SysDal_Rtm_Init(void)
{
    SysDal_Timer_Init();
}

/*===================================================================================================*/
/**
* @brief      DeInitialize the RTM feature
* @details    Disables the hardware resources used for timing (SysTick/General Purpose Timer)
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_DeInit(void)
{
    SysDal_Timer_DeInit();
}

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
FUNC(void, AUTOMATIC) SysDal_Rtm_StartMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    VAR(uint32, AUTOMATIC) u32Counter = 0;
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];

    if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
    {
        if ((TRUE == pMeasurement->bEnabled) || (pMeasurement->eMode == RTM_MODE_ONESHOT))
        {
            SysDal_Timer_GetOverflows(&u32Counter);
            pMeasurement->Start.u32TimerOverflows = u32Counter;

            SysDal_Timer_GetTicks(&u32Counter);
            pMeasurement->Start.u32TimerTicks = u32Counter;
        }
    }
}


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
FUNC(void, AUTOMATIC) SysDal_Rtm_StopMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    VAR(uint32, AUTOMATIC) u32Counter = 0;
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];

    if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
    {
        if ((TRUE == pMeasurement->bEnabled) || (pMeasurement->eMode == RTM_MODE_ONESHOT))
        {
            SysDal_Timer_GetOverflows(&u32Counter);
            pMeasurement->Stop.u32TimerOverflows = u32Counter;
            
            SysDal_Timer_GetTicks(&u32Counter);
            pMeasurement->Stop.u32TimerTicks = u32Counter;

            switch(pMeasurement->eMode)
            {
#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
                case RTM_MODE_ONESHOT:
                    SysDal_Rtm_SaveResult_OneShot(u8MeasurementId);
                    break;
#endif /* SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U */

#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)                
                case RTM_MODE_CONTINUOUS:
                    SysDal_Rtm_SaveResult_MovAvg(u8MeasurementId);
                    break;
#endif /*(SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */                
                default:
                    break;
            }
        }
    }
}


/*===================================================================================================*/
/**
* @brief      Enable a measurement
* @details    The start/stop measurement routines will only capture the timestamp is measurement is enabled
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_EnableMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    VAR(uint8, AUTOMATIC) u8ResultIndex;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    u8ResultIndex = pMeasurement->u8ResultIndex;

    /* Check if RTM measurement ID is valid */
    if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
    {
        /* Check if measurement is not already enabled */
        if (FALSE == pMeasurement->bEnabled)
        {
            pMeasurement->bEnabled = TRUE;

            /* Reset Oneshot measurement data */
            if (RTM_MODE_ONESHOT == pMeasurement->eMode)
            {
#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
                SysDal_Rtm_OneshotResults[u8ResultIndex].u64Value = 0x0ULL;
                SysDal_Rtm_OneshotResults[u8ResultIndex].Status = RTM_MEASUREMENT_INVALID;
#endif /* SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U */
            }
            /* Reset Continuous measurement data */
            else
            {
#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)  
                SysDal_Rtm_ContinuousResults[u8ResultIndex].u64Value = 0x0ULL;
                SysDal_Rtm_ContinuousResults[u8ResultIndex].u64MinValue = RTM_MOVAVG_MAX_VALUE;
                SysDal_Rtm_ContinuousResults[u8ResultIndex].u64MaxValue = RTM_MOVAVG_MIN_VALUE;
                SysDal_Rtm_ContinuousResults[u8ResultIndex].Status = RTM_MEASUREMENT_INVALID;
#endif /* (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */
            }
        }
    }
}


/*===================================================================================================*/
/**
* @brief      Disable a measurement
* @details    The start/stop measurement routines will only capture the timestamp is measurement is enabled
* 
* @param[in]  u8MeasurementId - ID of the RTM measurement. This ID must not exceed the maximum
*             number of possible parallel measurements
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_DisableMeasurement(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
    {
        SysDal_Rtm_Measurements[u8MeasurementId].bEnabled = FALSE;
    }
}


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
FUNC(SysDal_Rtm_MeasurementStatusType, AUTOMATIC) SysDal_Rtm_GetMeasurementStatus(VAR(uint8, AUTOMATIC) u8MeasurementId)
{
    VAR(SysDal_Rtm_MeasurementStatusType, AUTOMATIC) MeasurementResultStatus = RTM_MEASUREMENT_INVALID;
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];

    /* Check if RTM measurement ID is valid */
    if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
    {
        if (RTM_MODE_ONESHOT == pMeasurement->eMode)
        {
#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
            MeasurementResultStatus = SysDal_Rtm_OneshotResults[pMeasurement->u8ResultIndex].Status;
#endif /* SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U */
        }
        else
        {
#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
            MeasurementResultStatus = SysDal_Rtm_ContinuousResults[pMeasurement->u8ResultIndex].Status;
#endif /* SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U */
        }
    }

    return MeasurementResultStatus;
}


#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
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
                                                      P2VAR(SysDal_Rtm_OneshotResultType, AUTOMATIC, RTM_APPL_DATA) pResult)
{
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_OneshotMeasurementType, AUTOMATIC, RTM_APPL_DATA) pRtmResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    pRtmResult = &SysDal_Rtm_OneshotResults[pMeasurement->u8ResultIndex];

    if (NULL_PTR != pResult)
    {
        /* Check if RTM measurement ID is valid */
        if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
        {
            /* Check if the result is available */
            if ((RTM_MODE_ONESHOT == pMeasurement->eMode) && (RTM_MEASUREMENT_INVALID != pRtmResult->Status))
            {
                pResult->u32Id = pRtmResult->u32Id;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
                pResult->u32Timestamp = pRtmResult->u32Timestamp;
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

                SysDal_Rtm_TicksToDatetime(pRtmResult->u64Value, &pResult->Value);

                pRtmResult->Status = RTM_MEASUREMENT_OBSOLETE;
            }
        }
    }
}
#endif /* (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U) */


#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
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
                                                         P2VAR(SysDal_Rtm_ContinuousResultType, AUTOMATIC, RTM_APPL_DATA) pResult)
{
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_ContinuousMeasurementType, AUTOMATIC, RTM_APPL_DATA) pRtmResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    pRtmResult = &SysDal_Rtm_ContinuousResults[pMeasurement->u8ResultIndex];

    if (NULL_PTR != pResult)
    {
        /* Check if RTM measurement ID is valid */
        if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
        {
            /* Check if the result is available */
            if ((RTM_MODE_CONTINUOUS == pMeasurement->eMode) && (RTM_MEASUREMENT_INVALID != pRtmResult->Status))
            {
                pResult->u32Id = pRtmResult->u32Id;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
                pResult->u32Timestamp = pRtmResult->u32Timestamp;
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

                SysDal_Rtm_TicksToDatetime(pRtmResult->u64Value, &pResult->Value);
                SysDal_Rtm_TicksToDatetime(pRtmResult->u64MinValue, &pResult->MinValue);
                SysDal_Rtm_TicksToDatetime(pRtmResult->u64MaxValue, &pResult->MaxValue);

                pRtmResult->Status = RTM_MEASUREMENT_OBSOLETE;
            }
        }
    }
}
#endif /* (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */


#if (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U)
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
                                                       P2VAR(SysDal_Rtm_OneshotTicksResultType, AUTOMATIC, RTM_APPL_DATA) pResult)
{
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_OneshotMeasurementType, AUTOMATIC, RTM_APPL_DATA) pRtmResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    pRtmResult = &SysDal_Rtm_OneshotResults[pMeasurement->u8ResultIndex];

    if (NULL_PTR != pResult)
    {
        /* Check if RTM measurement ID is valid */
        if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
        {
            /* Check if the result is available */
            if ((RTM_MODE_ONESHOT == pMeasurement->eMode) && (RTM_MEASUREMENT_INVALID != pRtmResult->Status))
            {
                pResult->u32Id = pRtmResult->u32Id;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
                pResult->u32Timestamp = pRtmResult->u32Timestamp;
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

                pResult->u64Value = pRtmResult->u64Value;

                pRtmResult->Status = RTM_MEASUREMENT_OBSOLETE;
            }
        }
    }
}
#endif /* (SYSDAL_RTM_ONESHOT_MEAS_NUMBER > 0U) */


#if (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U)
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
                                                          P2VAR(SysDal_Rtm_ContinuousTicksResultType, AUTOMATIC, RTM_APPL_DATA) pResult)
{
    P2VAR(SysDal_Rtm_MeasurementType, AUTOMATIC, RTM_APPL_DATA) pMeasurement;
    P2VAR(SysDal_Rtm_ContinuousMeasurementType, AUTOMATIC, RTM_APPL_DATA) pRtmResult;

    pMeasurement = &SysDal_Rtm_Measurements[u8MeasurementId];
    pRtmResult = &SysDal_Rtm_ContinuousResults[pMeasurement->u8ResultIndex];

    if (NULL_PTR != pResult)
    {
        /* Check if RTM measurement ID is valid */
        if (u8MeasurementId < SYSDAL_RTM_MAX_MEAS_NUMBER)
        {
            /* Check if the result is available */
            if ((RTM_MODE_CONTINUOUS == pMeasurement->eMode) && (RTM_MEASUREMENT_INVALID != pRtmResult->Status))
            {
                pResult->u32Id = pRtmResult->u32Id;

#if (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON)
                pResult->u32Timestamp = pRtmResult->u32Timestamp;
#endif /* (SYSDAL_RTM_TIMESTAMP_ENABLED == STD_ON) */

                pResult->u64Value = pRtmResult->u64Value;
                pResult->u64MinValue = pRtmResult->u64MinValue;
                pResult->u64MaxValue = pRtmResult->u64MaxValue;

                pRtmResult->Status = RTM_MEASUREMENT_OBSOLETE;
            }
        }
    }
}
#endif /* (SYSDAL_RTM_CONTINUOUS_MEAS_NUMBER > 0U) */

#endif /* (SYSDAL_RTM_ENABLED == STD_ON) */

/*===================================================================================================*/
#ifdef __cplusplus
}
#endif
