/**
*   @file           ComDal.h
*   @version        24.12.0
*
*   @brief          Integration Framework - ComDal high level interface implementation.
*   @details        ComDal high level interface implementation.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*==================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/

#ifndef COMDAL_UART_H
#define COMDAL_UART_H

#ifdef __cplusplus
extern "C"{
#endif
/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "ComDal_Cfg.h"

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Uart.h
*/
#define COMDAL_UART_VENDOR_ID                           43
#define COMDAL_UART_MODULE_ID                           100

#define COMDAL_UART_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_UART_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_UART_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_UART_SW_MAJOR_VERSION                    24
#define COMDAL_UART_SW_MINOR_VERSION                    12
#define COMDAL_UART_SW_PATCH_VERSION                    0
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/* Check if source file and COMDAL UART header file are of the same vendor */
#if (COMDAL_UART_VENDOR_ID != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Uart.h and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and COMDAL UART header file are of the same Autosar version */
#if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_MINOR_VERSION != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_REVISION_VERSION != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Uart.h and ComDal_Cfg.h are different"
#endif

/* Check if source file and COMDAL UART header file are of the same Software version */
#if ((COMDAL_UART_SW_MAJOR_VERSION != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_UART_SW_MINOR_VERSION != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_UART_SW_PATCH_VERSION != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Uart.h and ComDal_Cfg.h are different"
#endif

#if COMDAL_MAX_UART_DESCRIPTORS > 0U

FUNC(void, COMDAL_APP_CODE) ComDal_Uart_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_Init(const ComDal_ConfigType *pConfig);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_DeInit(void);
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Uart_WriteData(
    ComDal_ChannelType ComChannel, char *pWriteBuffer, uint16 u16WriteBufferSize);
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Uart_ReadData(
    ComDal_ChannelType ComChannel, char *pReadBuffer, uint16 *pBufferSize);
FUNC(ComDal_State, COMDAL_APP_CODE) ComDal_Uart_GetChannelStatus(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);

#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#ifdef __cplusplus
}
#endif

#endif /*COMDAL_UART_H*/
/** @} */
