/*================================================================================================*/
/**
*   @file    nvic.c
*   @version 24.12.0
*
*   @brief   Integration Framework - NVIC driver
*   @details Functions that manage interrupts and exceptions via the NVIC
*
*    (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/
/*=================================================================================================
*    Platform      :      CORTEXM
*    Build Version :      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
==================================================================================================*/

#include "Std_Types.h"
#include "nvic.h"
#include "S32G399A_NVIC.h"
#include "S32G399A_SCB.h"

/*==================================================================================================
*                                        LOCAL MACROS
==================================================================================================*/
/*==================================================================================================
*                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/
#define FEATURE_NVIC_PRIO_BITS (4U)

/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
#define PLATFORM_START_SEC_CODE
#include "Platform_MemMap.h"
/**
* @brief Set Priority Grouping
* @details The function sets the priority grouping field using the required unlock sequence.
*  The parameter PriorityGroup is assigned to the field SCB->AIRCR [10:8] PRIGROUP field.
*   Only values from 0..7 are used.
*   In case of a conflict between priority grouping and available
*   priority bits (__NVIC_PRIO_BITS), the smallest possible priority group is set
*/
/*================================================================================================*/
void NVIC_SetPriorityGrouping(uint32 PriorityGroup)
{
    /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
    S32_SCB->AIRCR = (S32_SCB->AIRCR & (~0x700UL)) | PriorityGroup;
}

/*================================================================================================*/
/**
* @brief Enable External Interrupt
* @details The function enables a device-specific interrupt in the NVIC interrupt controller.
*/
/*================================================================================================*/
void NVIC_EnableIRQ(uint8 IRQn)
{
    S32_NVIC->ISER[(uint32)(IRQn) >> 5U] = (uint32)(1UL << ((uint32)(IRQn) & (uint32)0x1FU));
}

/*================================================================================================*/
/**
* @brief Disable External Interrupt
* @details The function disables a device-specific interrupt in the NVIC interrupt controller
*/
/*================================================================================================*/
void NVIC_DisableIRQ(uint8 IRQn)
{
    S32_NVIC->ICER[(uint32)(IRQn) >> 5U] = (uint32)(1UL << ((uint32)(IRQn) & (uint32)0x1FU));
}

/*================================================================================================*/
/**
* @brief Set Interrupt Priority
* @details The function sets the priority of an interrupt.
*/
/*================================================================================================*/
void NVIC_SetPriority(uint8 IRQn, uint8 priority)
{
    uint8 shift = (uint8) (8U - FEATURE_NVIC_PRIO_BITS);
    S32_NVIC->IP[(uint32)(IRQn)] = (uint8)(((((uint32)priority) << shift)) & 0xFFUL);
}

#define PLATFORM_STOP_SEC_CODE
#include "Platform_MemMap.h"
