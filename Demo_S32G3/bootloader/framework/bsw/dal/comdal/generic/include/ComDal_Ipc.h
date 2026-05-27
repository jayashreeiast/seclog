/**
*   @file           ComDal_Ipc.h
*   @version        24.12.0
*
*   @brief          Integration Framework - ComDal Abstraction Layer IPC component
*   @details        This file implements the ComDal Inter-processor Communication interface.
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

#ifndef COMDAL_IPC_H
#define COMDAL_IPC_H

#ifdef __cplusplus
extern "C"{
#endif

#if COMDAL_MAX_IPC_DESCRIPTORS > 0U
/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Ipc.h
*/
#define COMDAL_IPC_VENDOR_ID                           43
#define COMDAL_IPC_MODULE_ID                           100

#define COMDAL_IPC_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_IPC_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_IPC_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_IPC_SW_MAJOR_VERSION                    24
#define COMDAL_IPC_SW_MINOR_VERSION                    12
#define COMDAL_IPC_SW_PATCH_VERSION                    0

FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) ConfigPtr);
FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_DeInit(void);
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Ipc_ReadData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer, VAR(uint16, AUTOMATIC) RxDataLen);
FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_WriteData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxData, VAR(uint16, AUTOMATIC) TxDataLen);

#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

#ifdef __cplusplus
}
#endif

#endif /* COMDAL_IPC_H */
