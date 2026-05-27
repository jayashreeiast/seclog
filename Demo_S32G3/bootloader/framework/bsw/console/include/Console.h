/*===================================================================================================*/
/*
*   @file              Console.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Console log library
*   @details           This module implements console log functions
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef CONSOLE_H
#define CONSOLE_H

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
#include "BasicTypes.h"
#include "Std_Types.h"
#include "StringLib.h"

/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/
#define CONSOLE_BUFFER_SIZE      255U    /* Console buffer size in bytes. */

#define CONSOLE_ERROR            0x01     /* Errors that do not affect the entire application, but from which the application cannot recover. */
#define CONSOLE_INFO             0x02     /* Messages that explain the usual course of events. */

#if (CONSOLE_ENABLED == STD_ON)
#define CONSOLE_LOG(...) ConsoleLog(MACRO_TO_STRING(CONSOLE_TAG), CONSOLE_COMDAL_CHANNEL, __VA_ARGS__)
#else
#define CONSOLE_LOG(...)
#endif /* (CONSOLE_ENABLED == STD_ON) */

/*===================================================================================================*/
/*                                     FUNCTION PROTOTYPES                                           */
/*===================================================================================================*/
#if (CONSOLE_ENABLED == STD_ON)
/* Print log messages using ComDal interface. */
void ConsoleLog(const char* Tag, uint8_t ComChannel, uint8_t Level, const char* Format, ...);
#endif /* (CONSOLE_ENABLED == STD_ON) */

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
