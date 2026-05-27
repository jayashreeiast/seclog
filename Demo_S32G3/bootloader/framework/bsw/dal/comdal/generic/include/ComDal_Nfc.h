/**
*   @file           ComDal_Nfc.h
*   @version        24.12.0
*
*   @brief          Integration Framework - ComDal header file for NFC implementation.
*   @details        ComDal high level interface implementation for handling NFC driver.
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

#ifndef COMDAL_NFC_H
#define COMDAL_NFC_H

#include "ComDal_Types.h"
#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Nfc.h
*/
#define COMDAL_NFC_VENDOR_ID                           43
#define COMDAL_NFC_MODULE_ID                           100

#define COMDAL_NFC_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_NFC_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_NFC_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_NFC_SW_MAJOR_VERSION                    24
#define COMDAL_NFC_SW_MINOR_VERSION                    12
#define COMDAL_NFC_SW_PATCH_VERSION                    0

/*===================================================================================================================
*                                          DEFINES AND MACROS
====================================================================================================================*/
#define COMDAL_NFC_TRANSMIT_TIMEOUT                    10000


#if COMDAL_MAX_NFC_DESCRIPTORS > 0U

FUNC(void, AUTOMATIC) ComDal_Nfc_Controller_Reset(void);
FUNC(void, AUTOMATIC) ComDal_Nfc_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, CAN_APPL_CONST) ConfigPtr);
FUNC(void, AUTOMATIC) ComDal_Nfc_DeInit(void);
FUNC(void, AUTOMATIC) ComDal_Nfc_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(void, COMDAL_APP_CODE) ComDal_Nfc_WriteData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxDataString, VAR(uint8, AUTOMATIC) DataLen);
FUNC(ComDal_ReturnType, AUTOMATIC) ComDal_Nfc_ReadData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer, VAR(uint8, AUTOMATIC) DataLen);

#endif

#ifdef __cplusplus
}
#endif

#endif /*COMDAL_NFC_H*/
