/**
*   @file           SysDal.h
*   @version        24.12.0
*
*   @brief          Integration Framework - SysDal high level interface implementation.
*   @details        SysDal high level interface implementation.
*
*This file contains sample code only. It is not part of the production code deliverables.
*/
/*==================================================================================================
*   Platform             : CORTEXM
*   Peripheral           : LPit
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
==================================================================================================*/
/*==================================================================================================
==================================================================================================*/

#ifndef SYSDAL_INTSCH_H
#define SYSDAL_INTSCH_H


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "SysDal_Cfg.h"

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
#define SYSDAL_INTSCH_VENDOR_ID                       43

#define SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION        4
#define SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION        4
#define SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION     0

#define SYSDAL_INTSCH_SW_MAJOR_VERSION                24
#define SYSDAL_INTSCH_SW_MINOR_VERSION                12
#define SYSDAL_INTSCH_SW_PATCH_VERSION                0

/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/* Check if source file and SYSDAL_INTSCH header file are of the same vendor */
#if (SYSDAL_INTSCH_VENDOR_ID != SYSDAL_CFG_VENDOR_ID)
    #error "SysDal_IntSch.c and SysDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and SYSDAL header file are of the same Autosar version */
#if ((SYSDAL_INTSCH_AR_RELEASE_MAJOR_VERSION != SYSDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_MINOR_VERSION != SYSDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_INTSCH_AR_RELEASE_REVISION_VERSION != SYSDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_IntSch.c and SysDal_Cfg.h are different"
#endif

/* Check if source file and SYSDAL header file are of the same Software version */
#if ((SYSDAL_INTSCH_SW_MAJOR_VERSION != SYSDAL_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_INTSCH_SW_MINOR_VERSION != SYSDAL_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_INTSCH_SW_PATCH_VERSION != SYSDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_IntSch.c and SysDal_Cfg.h are different"
#endif

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#if defined(AUTOSAR_OS_NOT_USED)
FUNC (void, SAMPLE_APP_CODE) SysDal_IntSch_StartScheduler(void);
FUNC (void, SAMPLE_APP_CODE) SysDal_IntSch_StopScheduler(void);
extern FUNC (void, SAMPLE_APP_CODE) SysDal_IntSch_TaskActivationIsr(void);
FUNC (void, SAMPLE_APP_CODE) SysDal_IntSch_TaskScheduler(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /*SYSDAL_INTSCH_H*/
