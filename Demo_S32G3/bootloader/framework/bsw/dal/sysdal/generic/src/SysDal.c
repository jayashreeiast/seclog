/**
*   @file           SysDal.c
*   @version        24.12.0
*
*   @brief          Integration Framework - SysDal Abstraction Layer component
*   @details        This file implements the SysDal interface to application layer.
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
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/
/*============================================================================================================================
==============================================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "SysDal.h"
#include "Mcu.h"
#include "sys_init.h"
#include "modules.h"

#if defined(AUTOSAR_OS_NOT_USED)
#include "SysDal_IntSch.h"

#if (SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON)
#include "Gpt.h"
#endif /* (SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON) */

#endif/*AUTOSAR_OS_NOT_USED*/

#if(SYSDAL_LOW_POWER_MODE == STD_ON)

#include "Icu.h"

#if(INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON)
#include "Adc.h"
#endif /* (INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON) */

#endif /* (SYSDAL_LOW_POWER_MODE == STD_ON) */

#include "sys_init.h"
#ifdef S32K396
#include "S32K39_DCM_GPR.h"
#endif
/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
#define SYSDAL_VENDOR_ID_C                       43

#define SYSDAL_AR_RELEASE_MAJOR_VERSION_C        4
#define SYSDAL_AR_RELEASE_MINOR_VERSION_C        4
#define SYSDAL_AR_RELEASE_REVISION_VERSION_C     0

#define SYSDAL_SW_MAJOR_VERSION_C                24
#define SYSDAL_SW_MINOR_VERSION_C                12
#define SYSDAL_SW_PATCH_VERSION_C                0
/*===================================================================================================================
*                                     FILE VERSION CHECKS
====================================================================================================================*/
/* Check if source file and SYSDAL header file are of the same vendor */
#if (SYSDAL_VENDOR_ID_C != SYSDAL_VENDOR_ID)
    #error "SysDal.c and SysDal.h have different vendor ids"
#endif

/* Check if source file and SYSDAL header file are of the same Autosar version */
#if ((SYSDAL_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_AR_RELEASE_MINOR_VERSION_C != SYSDAL_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_AR_RELEASE_REVISION_VERSION_C != SYSDAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal.c and SysDal.h are different"
#endif

/* Check if source file and SYSDAL header file are of the same Software version */
#if ((SYSDAL_SW_MAJOR_VERSION_C != SYSDAL_SW_MAJOR_VERSION) || \
     (SYSDAL_SW_MINOR_VERSION_C != SYSDAL_SW_MINOR_VERSION) || \
     (SYSDAL_SW_PATCH_VERSION_C != SYSDAL_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal.c and SysDal.h are different"
#endif

/* Check if source file and SYSDAL configuration header file are of the same vendor */
#if (SYSDAL_VENDOR_ID_C != SYSDAL_CFG_VENDOR_ID)
    #error "SysDal.c and SysDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and SYSDAL configuration header file are of the same Autosar version */
#if ((SYSDAL_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_AR_RELEASE_MINOR_VERSION_C != SYSDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_AR_RELEASE_REVISION_VERSION_C != SYSDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal.c and SysDal_Cfg.h are different"
#endif

/* Check if source file and SYSDAL configuration header file are of the same software version */
#if ((SYSDAL_SW_MAJOR_VERSION_C != SYSDAL_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_SW_MINOR_VERSION_C != SYSDAL_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_SW_PATCH_VERSION_C != SYSDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal.c and SysDal_Cfg.h are different"
#endif
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/
SysDal_EcuModeType SysDal_EcuMode = SYSDAL_STATE_STARTUP;
SysDal_EcuModeType SysDal_EcuModeRequest = SYSDAL_STATE_RUN;
extern const SysDal_ConfigType SysDal_Config;
extern volatile boolean Wkup_Rtc;

/*ADC Data buffer used for cyclic activity*/
#ifdef INTAPP_CYCLIC_WAKEUP_TASK    /* If Cyclic activity is enabled */
Adc_ValueGroupType DataBufferPtr[1];
#endif /* INTAPP_CYCLIC_WAKEUP_TASK */

#ifdef IFWK_USE_WDG
#if (ENABLE_WDG_EXPIRE_TEST == STD_ON)
boolean SysDal_RequestWdgRst = FALSE;
#endif /* (ENABLE_WDG_EXPIRE_TEST == STD_ON) */
#endif /* IFWK_USE_WDG */

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
#if defined(AUTOSAR_OS_NOT_USED) && (SYSDAL_SYSTIME == STD_ON)
/* The counter is incremented each time the interrupt scheduler ISR is called. */
volatile uint32_t SysDal_u32SysTimerCounter = 0U;
#endif /* defined(AUTOSAR_OS_NOT_USED) && (SYSDAL_SYSTIME == STD_ON) */

/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/
extern ISR (Adc_Adcdig_EndGroupConvUnit0);

#if (SYSDAL_LOW_POWER_MODE == STD_ON)
FUNC (void, SYSDAL_APP_CODE ) SysDal_PrepareGoToSleep(SysDal_EcuModeType SleepMode);
#endif /* #if (SYSDAL_LOW_POWER_MODE == STD_ON) */

FUNC(void, SYSDAL_APP_CODE) SysDal_StartUpInit(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_WakeUpInit(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_PrepareGoToRun(void);
FUNC(void, SYSDAL_APP_CODE) SysDal_StartSysTimer(void);
#if (SYSDAL_LOW_POWER_MODE == STD_ON)
FUNC(void, SYSDAL_APP_CODE) WakeUpNotification(void);
FUNC(void, SYSDAL_APP_CODE) SleepNotification(void);
#endif/* SYSDAL_LOW_POWER_MODE == STD_ON */
/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/
#if (SYSDAL_CYCLIC_ACTIVITY == STD_ON)  /*If Cyclic activity is enabled*/
FUNC(void, SYSDAL_APP_CODE) SysDal_CyclicActivity(void)
{
    Wkup_Rtc=FALSE;
    Mcu_SetMode(SYSDAL_LIMITRUN_MODE);
/*Stop triggering CyclicActivity*/
    Gpt_StopTimer(SYSDAL_CYCLIC_ACTIVITY_TIMER);
    Gpt_DisableNotification(SYSDAL_CYCLIC_ACTIVITY_TIMER);
/*Stop triggering CyclicActivity*/
    Adc_StartGroupConversion(ADCWAKEUPGROUP);
    while (ADC_STREAM_COMPLETED != Adc_GetGroupStatus(ADCWAKEUPGROUP))
    {
        /* wait conversion finish */
    }
    Adc_ReadGroup(ADCWAKEUPGROUP, &DataBufferPtr[ADCWAKEUPCHANNEL]);
    if(DataBufferPtr[ADCWAKEUPCHANNEL] >= ANALOGWAKEUPTHRESHOLD)
    {
        /*wakeup*/
        SysDal_WakeUpInit();
    }
    else
    {
/*Start triggering CyclicActivity*/
        Gpt_EnableNotification(SYSDAL_CYCLIC_ACTIVITY_TIMER);
        Gpt_StartTimer(SYSDAL_CYCLIC_ACTIVITY_TIMER, SYSDAL_CYCLIC_ACTIVITY_TRIGGER_TICKS);
/*Start triggering CyclicActivity*/
        /*go to sleep*/
        Mcu_SetMode(SYSDAL_SLEEP_MODE);
    }
}
#endif  /* SYSDAL_CYCLIC_ACTIVITY */
/**
@brief      SysDal_StartUpInit - System initialization function
@details    Shall be used to initialize clock sources and block zero drivers

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_StartUpInit(void)
{
    SysDal_Config.InitBlockStartUpCallout();
    SysDal_McuPlatformInitSeq();
    SysDal_Config.InitBlockZeroCallout();
#if defined(AUTOSAR_OS_NOT_USED)
    SysDal_Config.InterruptsEnableCallout();
#endif /* (AUTOSAR_OS_NOT_USED == STD_ON) */

    SysDal_EcuMode = SYSDAL_STATE_STARTUP;
}

/**
@brief      SysDal_WakeUpInit - System initialization function called after StartUpInit
@details    Shall be used to reinitialise the system after transition from a Low Power mode

@return     None

@pre None
@post None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_WakeUpInit(void)
{
    SysDal_PrepareGoToRun();
    SysDal_Config.InitBlockOneCallout();
#if defined(AUTOSAR_OS_NOT_USED)
#if (SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON)
    SysDal_IntSch_StartScheduler();
#endif /* SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON */
#else /* (AUTOSAR_OS_NOT_USED == STD_ON) */
    StartOS(Mode01);
#endif /* (AUTOSAR_OS_NOT_USED == STD_ON) */
    SysDal_EcuModeRequest = SYSDAL_STATE_RUN;
}

/**
@brief      SysDal_PrepareGoToRun - Function used to prepare entering the configured Run mode
@details    Actions before entering RUN mode:
            -Disable Wakeup
            -Start scheduling

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_PrepareGoToRun(void)
{
#if (SYSDAL_LOW_POWER_MODE == STD_ON)
   SysDal_Config.ClearWakeupEventsCallout();
#endif /* SYSDAL_LOW_POWER_MODE == STD_ON */
   if(SYSDAL_STATE_STARTUP != SysDal_EcuMode)
   {
        Mcu_SetMode((Mcu_ModeType)SYSDAL_RUN_MODE);

        Mcu_InitClock((Mcu_ClockType) SYSDAL_RUN_CLOCKS_CONFIG);
#if(MCU_NO_PLL==STD_OFF)
        while ( MCU_PLL_LOCKED != Mcu_GetPllStatus() )
        {
        /* wait until all enabled PLLs are locked */
        }
        /* switch system clock tree to PLL */
        Mcu_DistributePllClock();
#endif/*MCU_NO_PLL == STD_OFF*/
    }
}

#if (SYSDAL_LOW_POWER_MODE == STD_ON)
/**
@brief      SysDal_PrepareGoToSleep - Function used to prepare entering the configured LowPower mode
@details    Actions before going to sleep:
            -Stop scheduling
            -Enable wakeup
            -Deinit software modules

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_PrepareGoToSleep(SysDal_EcuModeType SysDalSleep)
{
#if ((SYSDAL_TASK_SCHEDULING == STD_ON) || (SYSDAL_SYSTIME == STD_ON))
    SysDal_IntSch_StopScheduler();
#endif /* ((SYSDAL_TASK_SCHEDULING == STD_ON) || (SYSDAL_SYSTIME == STD_ON)) */
    SysDal_Config.SetWakeupEventsCallout();
    sys_disableAllInterrupts();/*exclusive area*/
    SysDal_Config.DeInitBlockOneCallout();
    sys_enableAllInterrupts();/*exclusive area*/
    if(SysDalSleep == SYSDAL_STATE_SLEEP)
    {
        SysDal_EcuMode = SYSDAL_STATE_SLEEP;
#if (SYSDAL_CYCLIC_ACTIVITY == STD_ON)  /*If Cyclic activity is enabled*/
        Mcu_InitClock((Mcu_ClockType) SYSDAL_LIMITRUN_CLOCKS_CONFIG);
        Gpt_EnableNotification(SYSDAL_CYCLIC_ACTIVITY_TIMER);
        Gpt_StartTimer(SYSDAL_CYCLIC_ACTIVITY_TIMER, SYSDAL_CYCLIC_ACTIVITY_TRIGGER_TICKS);
#endif /* SYSDAL_CYCLIC_ACTIVITY */
#ifdef S32K396
        IP_DCM_GPR->DCMRWF1 |= 0x00040000;
#endif
        Mcu_SetMode((Mcu_ModeType)SYSDAL_SLEEP_MODE);
        /*when resume from sleep make sure to resume all interrupts*/
        sys_enableAllInterrupts();
    }
    else
    {
     /*Do Nothing*/
   }
}
#endif /* SYSDAL_LOW_POWER_MODE == STD_ON */

/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

/**
@brief      SysDal_Init - Function called to initialise the system
@details    Shall be used to initialise the configured system

@return     None
*/
FUNC(void, ICU_CODE) SysDal_Init(void)
{
    SysDal_StartUpInit();

    SysDal_WakeUpInit();
}

/**
@brief      SysDal_DeInit - Function called to Deinitialise the system
@details    Shall be used to deinitialise the configured system

@return     None
*/
FUNC(void, ICU_CODE) SysDal_DeInit(void)
{
#if ((SYSDAL_TASK_SCHEDULING == STD_ON) || (SYSDAL_SYSTIME == STD_ON))
    SysDal_IntSch_StopScheduler();
#endif /* ((SYSDAL_TASK_SCHEDULING == STD_ON) || (SYSDAL_SYSTIME == STD_ON)) */
    SysDal_Config.DeInitBlockOneCallout();
}

#if defined(AUTOSAR_OS_NOT_USED)
/**
* @brief      Disable all registered interrupt sources
*
* @return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_DisableAllInterruptSources(void)
{
    SysDal_Config.InterruptsDisableCallout();
}
#endif /* defined(AUTOSAR_OS_NOT_USED) */

/**
@brief      WakeUpNotification - Notification called by the configured wakeup source
@details    Shall be used to reinitialise the system from configured Low Power mode

@return     None
*/
#if (SYSDAL_LOW_POWER_MODE == STD_ON)

FUNC(void, SYSDAL_APP_CODE) WakeUpNotification(void)
{
#if (SYSDAL_CYCLIC_ACTIVITY == STD_ON)  /*If Cyclic activity is enabled*/
    Gpt_StopTimer(SYSDAL_CYCLIC_ACTIVITY_TIMER);
    Gpt_DisableNotification(SYSDAL_CYCLIC_ACTIVITY_TIMER);
    Wkup_Rtc=FALSE;
#endif /* SYSDAL_CYCLIC_ACTIVITY */
    SysDal_WakeUpInit();
}

FUNC(void, SYSDAL_APP_CODE) SleepNotification(void)
{
    SysDal_WriteModeRequest(SYSDAL_STATE_SLEEP);
}
#endif/* SYSDAL_LOW_POWER_MODE == STD_ON */

/**
@brief      SysDal_Main - Main function for System Driver abstraction module
@details    Shall be used to compute the desired LowPower mode based on request (from VDR)
            Shall be used to set the request LowPower

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_Main(void)
{
#if (SYSDAL_LOW_POWER_MODE == STD_ON)
    SysDal_EcuMode =  SysDal_EcuModeRequest;
    if(SYSDAL_STATE_SLEEP == SysDal_EcuModeRequest)
    {
        SysDal_PrepareGoToSleep(SYSDAL_STATE_SLEEP);
    }
    else
    {
        /*Do nothing*/
    }
#endif /* SYSDAL_LOW_POWER_MODE == STD_ON */

#ifdef IFWK_USE_WDG
    #if (ENABLE_WDG_EXPIRE_TEST == STD_ON)
        if(TRUE == SysDal_RequestWdgRst)
        {
            SysDal_RequestWdgRst = FALSE;
            SysDal_Wdg_SetTriggerCondition(0);
        }
        else
        {
    #endif  /* (ENABLE_WDG_EXPIRE_TEST == STD_ON) */
            SysDal_Wdg_SetTriggerCondition(SYSDAL_WDG_TRIGGER_VALUE);
    #if (ENABLE_WDG_EXPIRE_TEST == STD_ON)
        }
    #endif /* (ENABLE_WDG_EXPIRE_TEST == STD_ON) */
#endif /* IFWK_USE_WDG */
}

/**
@brief      SysDal_Wait - Function used to make the CPU wait for a specific period of time
@details    Shall be used to request a timeout for hardware modules initialization

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_Wait(VAR(uint32, AUTOMATIC) SysDal_Timeout)
{
    volatile uint32 u32TmpTimeout = SysDal_Timeout;
    
    while(u32TmpTimeout--)
    {
        ASM_KEYWORD("nop");
    }
}

#if (SYSDAL_LOW_POWER_MODE == STD_ON)
/**
@brief      SysDal_WriteModeRequest - Function used to request for setting a Power mode
@details    Shall be used to request a configured LowPower mode

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_WriteModeRequest(SysDal_EcuModeType Mode)
{
    SysDal_EcuModeRequest = Mode;
}

/**
@brief      SysDal_GetPowerMode - Function used to get current power mode
@details    This function is currently a stub.

@return     None
*/

FUNC(SysDal_EcuModeType, SYSDAL_APP_CODE ) SysDal_GetCurrentPowerMode(void)
{
    return SysDal_EcuMode;
}
#endif /* SYSDAL_LOW_POWER_MODE == STD_ON */

/**
@brief      SysDal_ExecuteSwReset - Function used to request a SW reset.
@details    This function is currently a stub.

@return     None
*/
FUNC(void, SYSDAL_APP_CODE) SysDal_ExecuteSwReset(void)
{
#ifdef IFWK_USE_WDG
#if (ENABLE_WDG_EXPIRE_TEST == STD_ON)
    SysDal_RequestWdgRst = TRUE;
#endif /* (ENABLE_WDG_EXPIRE_TEST == STD_ON) */
#endif    /* (IFWK_USE_WDG) */
}

/*Function used to mask the interrupts*/
FUNC(void, SYSDAL_APP_CODE) SysDal_DisableAllInterrupts(void)
{
#if  (MCAL_PLATFORM_ARM != MCAL_ARM_RARCH) /* if platform is NOT ARM R */
    /* Setup all the needed interrupts(and their priorities) */
    sys_disableAllInterrupts();
#endif /* MCAL_PLATFORM_ARM */
}

/*Function used to unmask the interrupts*/
FUNC(void, SYSDAL_APP_CODE) SysDal_EnableAllInterrupts(void)
{
#if  (MCAL_PLATFORM_ARM != MCAL_ARM_RARCH) /* if platform is NOT ARM R */
    /* Setup all the needed interrupts(and their priorities) */
    sys_enableAllInterrupts();
#endif /* MCAL_PLATFORM_ARM */
}

#if defined(AUTOSAR_OS_NOT_USED) && (SYSDAL_SYSTIME == STD_ON)
/*================================================================================================*/
/**
* @brief        Return the current system time in milliseconds
* @details      This function returns the system time based on internal scheduler timer.
*
* @return       The current system time in milliseconds
*/
/*================================================================================================*/
FUNC(uint32, SYSDAL_APP_CODE) SysDal_GetSystemTimeMs(void)
{
    return SysDal_u32SysTimerCounter;
}
#endif /* defined(AUTOSAR_OS_NOT_USED) && (SYSDAL_SYSTIME == STD_ON) */

#ifdef __cplusplus
}
#endif
/** @} */
