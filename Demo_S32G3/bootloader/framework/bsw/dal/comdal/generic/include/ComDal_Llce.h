/**

*   @file              ComDal_Llce.h
*   project            AUTOSAR 4.4 GOLDVIP 
*   platform           CORTEXM 
*   @version           24.12.0 
* 
*   @brief             Integration Framework - ComDal header file for LLCE implementation.
*   @details           ComDal high level interface implementation for handling LLCE driver.
*
* 
*   Autosar Version    4.4.0 
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120 
* 
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP 
*   All Rights Reserved. 
* 
=====================================================================================================================*/

#ifndef COMDAL_LLCE_H
#define COMDAL_LLCE_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "ComDal_Cfg.h"

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Llce.h
*/
#define COMDAL_LLCE_VENDOR_ID                           43
#define COMDAL_LLCE_MODULE_ID                           100

#define COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_LLCE_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_LLCE_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_LLCE_SW_MAJOR_VERSION                    24
#define COMDAL_LLCE_SW_MINOR_VERSION                    12
#define COMDAL_LLCE_SW_PATCH_VERSION                    0


/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
#if COMDAL_MAX_LLCE_DESCRIPTORS > 0U

FUNC(void, AUTOMATIC) ComDal_Llce_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, LLCE_APPL_CONST) ConfigPtr);
FUNC(void, AUTOMATIC) ComDal_Llce_DeInit(void);
FUNC(void, AUTOMATIC) ComDal_Llce_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(ComDal_ReturnType, AUTOMATIC) ComDal_Llce_ReadData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer, VAR(uint8, AUTOMATIC) LlceDataLen);
FUNC(void, AUTOMATIC) ComDal_Llce_WriteData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxDataString, VAR(uint8, AUTOMATIC) LlceDataLen);
FUNC(uint16, COMDAL_APP_CODE) ComDal_Llce_GetMessageHandlerId(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_LoadFirmware(void);
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*COMDAL_LLCE_H*/
/** @} */

