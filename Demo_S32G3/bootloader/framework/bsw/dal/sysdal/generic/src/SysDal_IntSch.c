/**
*   @file           SysDal_IntSch.c
*   @version        24.12.0
*
*   @brief          Integration Framework - SysDal scheduler implementation.
*   @details        SysDal scheduler implementation.
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
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "SysDal.h"
#include "SysDal_IntSch.h"

#if defined(AUTOSAR_OS_NOT_USED)

#if (SYSDAL_SYSTIME == STD_ON || SYSDAL_TASK_SCHEDULING == STD_ON)
#include "Gpt.h"
#endif /* (SYSDAL_SYS_TIME == STD_ON || SYSDAL_TASK_SCHEDULING == STD_ON) */

#endif /* AUTOSAR_OS_NOT_USED */

#include "sys_init.h"

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
#define SYSDAL_INTSCH_VENDOR_ID_C                       43

#define SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION_C        4
#define SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION_C        4
#define SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION_C     0

#define SYSDAL_INTSCH_SW_MAJOR_VERSION_C                24
#define SYSDAL_INTSCH_SW_MINOR_VERSION_C                12
#define SYSDAL_INTSCH_SW_PATCH_VERSION_C                0
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/**
* @file           SysDal_IntSch.c
*/
/* Check if source file and SYSDAL_INTSCH header file are of the same vendor */
#if (SYSDAL_INTSCH_VENDOR_ID_C != SYSDAL_VENDOR_ID)
    #error "SysDal_IntSch.c and SysDal.h have different vendor ids"
#endif

/* Check if source file and SYSDAL header file are of the same Autosar version */
#if ((SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION_C != SYSDAL_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION_C != SYSDAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_IntSch.c and SysDal.h are different"
#endif

/* Check if source file and SYSDAL header file are of the same Software version */
#if ((SYSDAL_INTSCH_SW_MAJOR_VERSION_C != SYSDAL_SW_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_SW_MINOR_VERSION_C != SYSDAL_SW_MINOR_VERSION) || \
     (SYSDAL_INTSCH_SW_PATCH_VERSION_C != SYSDAL_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_IntSch.c and SysDal.h are different"
#endif

/* Check if source file and SYSDAL_INTSCH header file are of the same vendor */
#if (SYSDAL_INTSCH_VENDOR_ID_C != SYSDAL_INTSCH_VENDOR_ID)
    #error "SysDal_IntSch.c and SysDal_IntSch.h have different vendor ids"
#endif

/* Check if source file and SYSDAL header file are of the same Autosar version */
#if ((SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION_C != SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION_C != SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_IntSch.c and SysDal_IntSch.h are different"
#endif

/* Check if source file and SYSDAL header file are of the same Software version */
#if ((SYSDAL_INTSCH_SW_MAJOR_VERSION_C != SYSDAL_INTSCH_SW_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_SW_MINOR_VERSION_C != SYSDAL_INTSCH_SW_MINOR_VERSION) || \
     (SYSDAL_INTSCH_SW_PATCH_VERSION_C != SYSDAL_INTSCH_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_IntSch.c and SysDal_IntSch.h are different"
#endif

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
#if (INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON)
volatile boolean Wkup_Rtc;
#endif /* (INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON) */

#if defined(AUTOSAR_OS_NOT_USED)

#if (SYSDAL_TASK_SCHEDULING == STD_ON)
#if (INTAPP_TASK_1_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task1_Time = 0U;
volatile boolean SysDal_IntSch_Task1_isActive = FALSE;
#endif /*(INTAPP_TASK_1_ENABLE == STD_ON)*/

#if (INTAPP_TASK_2_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task2_Time = 0U;
volatile boolean SysDal_IntSch_Task2_isActive = FALSE;
#endif /*(INTAPP_TASK_2_ENABLE == STD_ON)*/

#if (INTAPP_TASK_3_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task3_Time = 0U;
volatile boolean SysDal_IntSch_Task3_isActive = FALSE;
#endif /*(INTAPP_TASK_3_ENABLE == STD_ON)*/

#if (INTAPP_TASK_4_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task4_Time = 0U;
volatile boolean SysDal_IntSch_Task4_isActive = FALSE;
#endif /*(INTAPP_TASK_4_ENABLE == STD_ON)*/

#if (INTAPP_TASK_5_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task5_Time = 0U;
volatile boolean SysDal_IntSch_Task5_isActive = FALSE;
#endif /*(INTAPP_TASK_5_ENABLE == STD_ON)*/

#if (INTAPP_TASK_6_ENABLE == STD_ON)
volatile uint32  SysDal_IntSch_Task6_Time = 0U;
volatile boolean SysDal_IntSch_Task6_isActive = FALSE;
#endif /*(INTAPP_TASK_6_ENABLE == STD_ON)*/
#endif /* (SYSDAL_TASK_SCHEDULING == STD_ON) */
/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
#if (SYSDAL_SYSTIME == STD_ON)
extern volatile uint32_t SysDal_u32SysTimerCounter;
#endif /* (SYSDAL_SYSTIME == STD_ON) */

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#if (SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON)
static inline FUNC(void, SYSDAL_APP_CODE) SysDal_IntSch_HandleTimeSlices(void);

/**
* @brief      Increment each task's time slice and check if any task needs to be activated
*
* @return     None
*/
static inline FUNC(void, SYSDAL_APP_CODE) SysDal_IntSch_HandleTimeSlices(void)
{
    #if (INTAPP_TASK_1_ENABLE == STD_ON)
        SysDal_IntSch_Task1_Time++;
    #endif /*(INTAPP_TASK_1_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_2_ENABLE == STD_ON)
        SysDal_IntSch_Task2_Time++;
    #endif /*(INTAPP_TASK_2_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_3_ENABLE == STD_ON)
        SysDal_IntSch_Task3_Time++;
    #endif /*(INTAPP_TASK_3_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_4_ENABLE == STD_ON)
        SysDal_IntSch_Task4_Time++;
    #endif /*(INTAPP_TASK_4_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_5_ENABLE == STD_ON)
        SysDal_IntSch_Task5_Time++;
    #endif /*(INTAPP_TASK_5_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_6_ENABLE == STD_ON)
        SysDal_IntSch_Task6_Time++;
    #endif /*(INTAPP_TASK_6_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_1_ENABLE == STD_ON)
        if (SysDal_IntSch_Task1_Time >= INTAPP_TASK_1_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task1_Time = 0U;
            SysDal_IntSch_Task1_isActive = TRUE;
        }
    #endif /*(INTAPP_TASK_1_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_2_ENABLE == STD_ON)
        if (SysDal_IntSch_Task2_Time >= INTAPP_TASK_2_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task2_Time = 0U;
            SysDal_IntSch_Task2_isActive = TRUE;
        }
    #endif /*(INTAPP_TASK_2_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_3_ENABLE == STD_ON)
        if (SysDal_IntSch_Task3_Time >= INTAPP_TASK_3_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task3_Time = 0U;
            SysDal_IntSch_Task3_isActive = TRUE;
        }
    #endif /*(INTAPP_TASK_3_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_4_ENABLE == STD_ON)
        if (SysDal_IntSch_Task4_Time >= INTAPP_TASK_4_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task4_Time = 0U;
            SysDal_IntSch_Task4_isActive = TRUE;
        }
    #endif /*(INTAPP_TASK_4_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_5_ENABLE == STD_ON)
        if (SysDal_IntSch_Task5_Time >= INTAPP_TASK_5_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task5_Time = 0U;
            SysDal_IntSch_Task5_isActive = TRUE;
        }
    #endif /*(INTAPP_TASK_5_ENABLE == STD_ON)*/

    #if (INTAPP_TASK_6_ENABLE == STD_ON)
        if (SysDal_IntSch_Task6_Time >= INTAPP_TASK_6_ACTIVATION_TIME)
        {
            SysDal_IntSch_Task6_Time = 0U;
            SysDal_IntSch_Task6_isActive = TRUE;
        }
    #endif  /*(INTAPP_TASK_6_ENABLE == STD_ON)*/
}

/**
* @brief      Start the reference timer
*
* @return     None
*/
FUNC (void, SYSDAL_APP_CODE) SysDal_IntSch_StartScheduler(void)
{
    Gpt_EnableNotification(SYSDAL_SCH_GPT_TIMER_CHANNEL);
    Gpt_StartTimer(SYSDAL_SCH_GPT_TIMER_CHANNEL, GPT_SCH_TICKS);
}

/**
* @brief      SysDal_IntSch_StopScheduler - Stop Internal scheduler
* @details
*
* @return     None
*/
FUNC (void, SYSDAL_APP_CODE) SysDal_IntSch_StopScheduler(void)
{
    Gpt_StopTimer(SYSDAL_SCH_GPT_TIMER_CHANNEL);
    Gpt_DisableNotification(SYSDAL_SCH_GPT_TIMER_CHANNEL);
#if (SYSDAL_TASK_SCHEDULING == STD_ON)
#if (INTAPP_TASK_1_ENABLE == STD_ON)
    SysDal_IntSch_Task1_Time = 0U;
    SysDal_IntSch_Task1_isActive = FALSE;
#endif /*(INTAPP_TASK_1_ENABLE == STD_ON)*/

#if (INTAPP_TASK_2_ENABLE == STD_ON)
    SysDal_IntSch_Task2_Time = 0U;
    SysDal_IntSch_Task2_isActive = FALSE;
#endif /*(INTAPP_TASK_2_ENABLE == STD_ON)*/

#if (INTAPP_TASK_3_ENABLE == STD_ON)
    SysDal_IntSch_Task3_Time = 0U;
    SysDal_IntSch_Task3_isActive = FALSE;
#endif /*(INTAPP_TASK_3_ENABLE == STD_ON)*/

#if (INTAPP_TASK_4_ENABLE == STD_ON)
    SysDal_IntSch_Task4_Time = 0U;
    SysDal_IntSch_Task4_isActive = FALSE;
#endif /*(INTAPP_TASK_4_ENABLE == STD_ON)*/

#if (INTAPP_TASK_5_ENABLE == STD_ON)
    SysDal_IntSch_Task5_Time = 0U;
    SysDal_IntSch_Task5_isActive = FALSE;
#endif /*(INTAPP_TASK_5_ENABLE == STD_ON)*/

#if (INTAPP_TASK_6_ENABLE == STD_ON)
    SysDal_IntSch_Task6_Time = 0U;
    SysDal_IntSch_Task6_isActive = FALSE;
#endif /*(INTAPP_TASK_6_ENABLE == STD_ON)*/
#endif /* (SYSDAL_TASK_SCHEDULING == STD_ON) */
}

#if (INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON)
FUNC (void, SAMPLE_APP_CODE) SysDal_CyclicWakeup_Notif(void)
{
    Wkup_Rtc = TRUE;/*Limited RUN*/
}
#endif /* INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON */

/**
* @brief      Interrupt handler for round-robin scheduler and system timer increment
* @details
*
* @return     none
* @retval     none
*
* @pre None
* @post None
*/
FUNC (void, SYSDAL_APP_CODE) SysDal_IntSch_TaskActivationIsr(void)
{
    SysDal_DisableAllInterrupts();

#if (SYSDAL_SYSTIME == STD_ON)
    SysDal_u32SysTimerCounter++;
#endif /* SYSDAL_SYSTIME == STD_ON */

#if (SYSDAL_TASK_SCHEDULING == STD_ON)
    SysDal_IntSch_HandleTimeSlices();
#endif /* SYSDAL_TASK_SCHEDULING == STD_ON */

    SysDal_EnableAllInterrupts();
}

/**
* @brief      Internal round-robin scheduler
*
* @return     None
*/
FUNC (void, SYSDAL_APP_CODE) SysDal_IntSch_TaskScheduler(void)
{
    /* local variables here */
#if (INTAPP_PREHOOK_TASK_ENABLE == STD_ON) /**/
    INTAPP_PREHOOK_TASK();
#endif    /*(INTAPP_PREHOOK_TASK_ENABLE == STD_ON)*/
    while (1)
    {
#if (INTAPP_TASK_1_ENABLE == STD_ON) /**/
        if (TRUE == SysDal_IntSch_Task1_isActive)
        {
            SysDal_IntSch_Task1_isActive = FALSE;
            /* call Task 1 */
            INTAPP_TASK_1();
        }
#endif  /*(INTAPP_TASK_1_ENABLE == STD_ON)*/

#if (INTAPP_TASK_2_ENABLE == STD_ON)
        if (TRUE == SysDal_IntSch_Task2_isActive)
        {
            SysDal_IntSch_Task2_isActive = FALSE;
            /* call Task 2 */
            INTAPP_TASK_2();
        }
#endif  /*(INTAPP_TASK_2_ENABLE == STD_ON)*/

#if (INTAPP_TASK_3_ENABLE == STD_ON)
        if (TRUE == SysDal_IntSch_Task3_isActive)
        {
            SysDal_IntSch_Task3_isActive = FALSE;
            /* call Task 3 */
            INTAPP_TASK_3();
        }
#endif /*(INTAPP_TASK_3_ENABLE == STD_ON)*/


#if (INTAPP_TASK_4_ENABLE == STD_ON)
        if (TRUE == SysDal_IntSch_Task4_isActive)
        {
            SysDal_IntSch_Task4_isActive = FALSE;
            /* call Task 4 */
            INTAPP_TASK_4();
        }
#endif /*(INTAPP_TASK_4_ENABLE == STD_ON)*/

#if (INTAPP_TASK_5_ENABLE == STD_ON)
        if (TRUE == SysDal_IntSch_Task5_isActive)
        {
            SysDal_IntSch_Task5_isActive = FALSE;
            /* call Task 5 */
            INTAPP_TASK_5();
        }
#endif   /*(INTAPP_TASK_5_ENABLE == STD_ON)*/

#if (INTAPP_TASK_6_ENABLE == STD_ON)
        if (TRUE == SysDal_IntSch_Task6_isActive)
        {
            SysDal_IntSch_Task6_isActive = FALSE;
            /* call Task 6 */
            INTAPP_TASK_6();
        }
#endif  /*(INTAPP_TASK_6_ENABLE == STD_ON)*/

#if (INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON)
        if (TRUE == Wkup_Rtc)
        {
            INTAPP_CYCLIC_WAKEUP_TASK();
        }
#endif   /*(INTAPP_CYCLIC_WAKEUP_TASK_ENABLE == STD_ON)*/
#if (INTAPP_IDLE_TASK_ENABLE == STD_ON)
        INTAPP_IDLE_TASK();
#endif /*(INTAPP_IDLE_TASK_ENABLE == STD_ON)*/
    }
}
#endif /* (SYSDAL_TASK_SCHEDULING == STD_ON || SYSDAL_SYSTIME == STD_ON) */

#endif /* (AUTOSAR_OS_NOT_USED) */


/*================================================================================================*/

#ifdef __cplusplus
}
#endif
/** @} */
