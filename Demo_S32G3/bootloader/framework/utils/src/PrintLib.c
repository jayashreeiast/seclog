/**
*   @file           MemLib.c
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - Print Library interface implementation.
*   @details        Print Library interface implementation.
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

#ifdef __cplusplus
extern "C"{
#endif


/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/

#include "PrintLib.h"
/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/
/*================================================================================================*/
/**
* @brief    Function that formats the text based on arguments.
*
* @param[in]  Buffer    Buffer where the resulting string is stored.
* @param[in]  Size      Maximum number of bytes to be used in the buffer.
* @param[in]  Format    The message that will be displayed.
* @param[in]  Args      Arguments for format specification.
*
* @return   The number of characters written in the buffer.
*/
/*================================================================================================*/
uint8 PrintLib_vSnprintf(char *Buffer, uint8 Size, char const *Format, va_list Args) 
{
    uint8 Length = 0U;
    uint8 Base = 10;

    sint32 Num = 0U;
    char *StrArg = 0U;
    uint8 ArgLength = 0U;
    uint8 u8Index = 0U;

    for (u8Index = 0; Format[u8Index] != 0 && Size - Length > 0; u8Index++)
    {
        if (Format[u8Index] == '%') 
        {
            u8Index++;
            switch (Format[u8Index]) 
            {
                case '%':
                    Buffer[Length++] = '%';
                    break;
                case 'c':
                    Buffer[Length++] = va_arg(Args, char);
                    break;
                case 'h':
                    Base = 16;
                    /* Fall through to decimal case. */
                case 'd': ;
                    Num = va_arg(Args, sint32);
                    Length += StrLib_Itoa(Num, Buffer + Length, Base);
                    break;
                case 's': ;
                    StrArg = va_arg(Args, char *);
                    ArgLength = StrLib_StringLen(StrArg);
                    StrLib_StrCpy(StrArg, Buffer + Length, ArgLength);
                    Length += ArgLength;
                    break;
                default:
                    /* Do nothing */
                    break;
            }
        } 
        else 
        {
            Buffer[Length++] = Format[u8Index];
        }
    }

    if (Length > Size) 
    {
        Length = Size - 1;
    }
    Buffer[Length] = '\0';

    return Length;
}

/*================================================================================================*/
/**
* @brief    Function that formats the text based on arguments.
*
* @param[in]  Buffer    Buffer where the resulting string is stored.
* @param[in]  Size      Maximum number of bytes to be used in the buffer.
* @param[in]  Format    The message that will be displayed.
* @param[in]  Args      Arguments for format specification.
*
* @return   The number of characters written in the buffer.
*/
/*================================================================================================*/
uint8 PrintLib_Snprintf(char *Buffer, uint8 Size, char const *Format, ...) 
{
    va_list va;
    uint8 Length;

    va_start(va, Format);
    Length = PrintLib_vSnprintf(Buffer, Size, Format, va);
    va_end(va);

    return Length;
}
/*================================================================================================*/
#ifdef __cplusplus
}
#endif
