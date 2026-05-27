/*===================================================================================================*/
/*
*   @file              DatetimeLib.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Date time library
*   @details           This module implements date time related functions
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef DATETIMELIB_H
#define DATETIMELIB_H

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
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/**
* @brief          Time interval structure
* @details        This structure represents a time interval length
*/
typedef struct
{
    uint16  u16Hours;                   /**< @brief The time interval Hours component */
    uint8   u8Minutes;                  /**< @brief The time interval Minutes component */
    uint8   u8Seconds;                  /**< @brief The time interval Seconds component */
    uint16  u16Milliseconds;            /**< @brief The time interval Milliseconds component */
    uint16  u16Microseconds;            /**< @brief The time interval Microseconds component */
} Datetime_IntervalType;

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL CONSTANTS                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL VARIABLES                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL FUNCTIONS                                            */
/*===================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* DATETIMELIB_H */
