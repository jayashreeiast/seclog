/*===================================================================================================*/
/*
*   @file              Console.c
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

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "Console.h"
#include "PrintLib.h"
#if (CONSOLE_ENABLED == STD_ON)
#include "SysDal.h"
#include "ComDal.h"
#endif /* (CONSOLE_ENABLED == STD_ON) */

#include <stdio.h>
#include <stdarg.h>

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/
/*================================================================================================*/
/**
* @brief      Function to print log messages.
* @details    This function prints messages to the channel configured in the following format: 
*             "{tag} {hexadecimal priority level} {message}.
*
* @param[in] Tag          The identifier for which component sent the message. (eg Bootloader)
* @param[in] ComChannel   Logical BSW ComDal channel id.
* @param[in] Level        Priority level (CONSOLE_INFO, CONSOLE_DEBUG)
* @param[in] Format       The message that will be displayed.
* @param[in] ...          Arguments for format specification.
* 
* @return    None
*/
/*================================================================================================*/
#if (CONSOLE_ENABLED == STD_ON)
void ConsoleLog(const char* Tag, uint8_t ComChannel, uint8_t Level, const char* Format, ...) 
{
    volatile char format_buffer[CONSOLE_BUFFER_SIZE];
    volatile char console_buffer[CONSOLE_BUFFER_SIZE];

    va_list va;
    va_start(va, Format);
    PrintLib_vSnprintf((char *)format_buffer, CONSOLE_BUFFER_SIZE, Format, va);
    va_end(va);

    PrintLib_Snprintf((char *)console_buffer, CONSOLE_BUFFER_SIZE, "%s 0x%h %s", Tag, Level, format_buffer);
  
    ComDal_WriteData(ComChannel, (char *)console_buffer, StrLib_StringLen((char *)console_buffer));

#if (SYSDAL_TASK_SCHEDULING == STD_OFF)
    do
    {
        ComDal_Main();
    } while (COMDAL_STATE_TX_ONGOING == ComDal_GetChannelStatus(ComChannel));
#endif /* (SYSDAL_TASK_SCHEDULING == STD_OFF) */
}
#endif /* (CONSOLE_ENABLED == STD_ON) */


#ifdef __cplusplus
}
#endif
