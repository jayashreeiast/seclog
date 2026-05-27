/**
*   @file           StringLib.h
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - String Operations Library interface header.
*   @details        String Library Operations interface header.
*
*This file contains sample code only. It is not part of the production code deliverables.
*/
/*==================================================================================================
*   Project              : AUTOSAR 4.4 GOLDVIP
*   Platform             : CORTEXM
*   Peripheral           : USED_PERIPHERAL
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
==================================================================================================*/

#ifndef PRINTLIB_H
#define PRINTLIB_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdarg.h>

#include "Std_Types.h"

#include "StringLib.h"
/*===============================================================================================
                                     FUNCTION PROTOTYPES
===============================================================================================*/
uint8 PrintLib_vSnprintf(char *Buffer, uint8 Size, char const *Format, va_list Args);
uint8 PrintLib_Snprintf(char *Buffer, uint8 Size, char const *Format, ...);
/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif  /* PRINTLIB_H */
