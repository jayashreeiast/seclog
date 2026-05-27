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
/*==================================================================================================
==================================================================================================*/

#ifndef STRINGLIB_H
#define STRINGLIB_H

#ifdef __cplusplus
extern "C"{
#endif

#include "Std_Types.h"
/*=============================================================================================*/
/*                                       DEFINES AND MACROS                                    */
/*=============================================================================================*/
#define STRING(x) #x
#define MACRO_TO_STRING(x) STRING(x)
/*===============================================================================================
                                     FUNCTION PROTOTYPES
===============================================================================================*/
void StrLib_StrCpy(char* InputStr, char * OututStr, uint8 StrLen);
void StrLib_StrCat(char *DestStr, const char *SrcStr);
void StrLib_InsertCharToStr(char * InputStr, char DataChar, uint8 CharPosition, uint8 StrLen);
void StrLib_ReverseStringN(char * DataStr, uint32 length);
void StrLib_ReverseString(char * DataStr);
void StrLib_RemoveFirstN(char *DataString, uint32 Count);
uint8  StrLib_Ctoi(char DataChar);
uint8 StrLib_StrCmp(char * DataStr1, char * DataStr2);
uint8 StrLib_GetCharIndex(char * DataString, char DataChar);
uint32 StrLib_StringLen(char * DataStr);
uint32 StrLib_Atoi(char * DataChar);
int StrLib_Itoa(uint32 num, char * DataStr, uint8 base);
/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif  /* STRINGLIB_H */
