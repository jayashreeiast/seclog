/*===================================================================================================*/
/*
*   @file              SysDal_Timer.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - SysDal timers module
*   @details           This module is responsible for hardware timer configuration and will provide
*                      time measurement services
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef SYSDAL_TIMER_H
#define SYSDAL_TIMER_H

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
#include "Std_Types.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define SYSDAL_TIMER_VENDOR_ID                              43

#define SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION               4
#define SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION               4
#define SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION            0

#define SYSDAL_TIMER_SW_MAJOR_VERSION                       24
#define SYSDAL_TIMER_SW_MINOR_VERSION                       12
#define SYSDAL_TIMER_SW_PATCH_VERSION                       0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if header file and SYSDAL TIMER types header file are of the same vendor */
#if (SYSDAL_TIMER_VENDOR_ID != STD_VENDOR_ID)
    #error "SysDal_Timer.h and StandardTypes.h have different vendor ids"
#endif

/* Check if header file and SYSDAL TIMER types header file are of the same Autosar version */
#if ((SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION != STD_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION != STD_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION != STD_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Timer.h and StandardTypes.h are different"
#endif

/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/**
* @brief          Cortex M SysTick registers
* @details        This SysTick structure is valid for cores M4, M7, M33.
*/
typedef struct {
    volatile uint32 CSR;                    /**< SysTick Control and Status Register, offset: 0x0 */
    volatile uint32 RVR;                    /**< SysTick Reload Value Register, offset: 0x4 */
    volatile uint32 CVR;                    /**< SysTick Current Value Register, offset: 0x8 */
    volatile uint32 CALIB;                  /**< SysTick Calibration Value Register, offset: 0xC */
} SysTick_RegistersType;

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/* S32 SysTick - Peripheral instance base addresse */
#define S32_SYSTICK_BASE_REG                    (0xE000E010u)

/* Peripheral S32 SysTick base pointer */
#define S32_SYSTICK_REG                         ((SysTick_RegistersType *)S32_SYSTICK_BASE_REG)

/* S32 SysTick enable command */
#define S32_SYSTICK_ENABLE(x)                   (((uint32)(((uint32)(x))<<0x0))&0x1)

/* S32 SysTick reload command */
#define S32_SYSTICK_RELOAD(x)                   (((uint32)(((uint32)(x-1))<<0x0))&0xFFFFFF)

/* S32 SysTick current value command */
#define S32_SYSTICK_CURRENT(x)                  (((uint32)(((uint32)(x))<<0x0))&0xFFFFFF)

/* S32 SysTick interrupt command */
#define S32_SYSTICK_TICKINT(x)                  (((uint32)(((uint32)(x))<<0x1))&0x2)

/* S32 SysTick clock source command */
#define S32_SYSTICK_CLKSOURCE(x)                (((uint32)(((uint32)(x))<<0x2))&0x4)

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
* @brief      Initialize the SysDal timer
* @details    None
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_Init(void);

/*===================================================================================================*/
/**
* @brief      Deinitialize the SysDal timer
* @details    None
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_DeInit(void);

/*===================================================================================================*/
/**
* @brief      Get the timer overflows counter
* @details    The timer overflows counter reflects how many times the timer reseted his value to 0x0
* 
* @param[out] pTimerOverflows - number of elapsed timer overflows
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_GetOverflows(P2VAR(uint32, AUTOMATIC, RTM_APPL_DATA) pTimerOverflows);

/*===================================================================================================*/
/**
* @brief      Get the timer current ticks value
* @details    The UP/DOWN counting direction will be internally handled, and the value returned
*             represents the number of ticks until the timer overflow.
*
* @param[out] pTimerTicks - current number of timer ticks
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_GetTicks(P2VAR(uint32, AUTOMATIC, RTM_APPL_DATA) pTimerTicks);

#ifdef __cplusplus
}
#endif

#endif /* SYSDAL_TIMER_H */
