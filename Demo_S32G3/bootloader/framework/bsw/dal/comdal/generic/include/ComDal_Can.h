/**
*   @file           ComDal_Can.h
*   @version        24.12.0
*
*   @brief          Integration Framework - ComDal header file for CAN implementation.
*   @details        ComDal high level interface implementation for handling CAN driver.
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
/*============================================================================================================================
==================================================================================================*/

#ifndef COMDAL_CAN_H
#define COMDAL_CAN_H

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Can.h
*/
#define COMDAL_CAN_VENDOR_ID                           43
#define COMDAL_CAN_MODULE_ID                           100

#define COMDAL_CAN_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_CAN_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_CAN_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_CAN_SW_MAJOR_VERSION                    24
#define COMDAL_CAN_SW_MINOR_VERSION                    12
#define COMDAL_CAN_SW_PATCH_VERSION                    0


/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
#if COMDAL_MAX_CAN_DESCRIPTORS > 0U

FUNC(void, AUTOMATIC) ComDal_Can_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, CAN_APPL_CONST) ConfigPtr);
FUNC(void, AUTOMATIC) ComDal_Can_DeInit(void);
FUNC(void, AUTOMATIC) ComDal_Can_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(ComDal_ReturnType, AUTOMATIC) ComDal_Can_ReadData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer, VAR(uint8, AUTOMATIC) CanDataLen);
FUNC(void, AUTOMATIC) ComDal_Can_WriteData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxDataString, VAR(uint8, AUTOMATIC) CanDataLen);
FUNC(uint16, COMDAL_APP_CODE) ComDal_Can_GetMessageHandlerId(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(ComDal_State, COMDAL_APP_CODE) ComDal_Can_GetChannelStatus(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
#endif

#ifdef __cplusplus
}
#endif

#endif /*COMDAL_H*/
/** @} */

