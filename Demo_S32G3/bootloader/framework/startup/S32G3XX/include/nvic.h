/*================================================================================================*/
/**
*   @file    nvic.h
*   @version 24.12.0
*
*   @brief   Integration Framework - header file for NVIC driver
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

#ifndef NVIC_H
#define NVIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
/*==================================================================================================
*                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/
#define NVIC_BASEADDR 0xE000E100
#define NVIC_ISER_OFFSET(id)      (uint32)(((id) >> 5) << 2)
#define NVIC_ICER_OFFSET(id)      (0x80 + (uint32)(((id) >> 5) << 2))
#define NVIC_IPRO_OFFSET(id)      (0x300 + (uint32)(((id) >> 2) << 2))
#define NVIC_IPRO_MASK(id)        (uint32)(0xFFU << (((id) % 4) * 8))
#define NVIC_IPRO_SHIFT(id, prio) (uint32)((prio) << (((id) % 4) * 8))

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/


/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/
#define PLATFORM_START_SEC_CODE
#include "Platform_MemMap.h"

void NVIC_SetPriorityGrouping(uint32 PriorityGroup);
void NVIC_EnableIRQ(uint8 IRQn);
void NVIC_DisableIRQ(uint8 IRQn);
void NVIC_SetPriority(uint8 IRQn, uint8 priority);

#define PLATFORM_STOP_SEC_CODE
#include "Platform_MemMap.h"

#ifdef __cplusplus
}
#endif

#endif /*NVIC_H*/
