/**
*   @file           MemLib.h
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - Memory Operations Library interface header.
*   @details        Memory Library Operations interface header.
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

#ifndef MEMLIB_H
#define MEMLIB_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "Std_Types.h"

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
/* Align a memory buffer (in bytes) to a given byte size (x aligned y) */
#define ALIGN_BYTE_OP(x, y)  (((x) >> (y)) << (y))

/*  Implements modulo y (x modulo y) */
#define MOD_BYTE_OP(x,y)  ((x) - ALIGN_BYTE_OP(x, y))

/*===============================================================================================
                                     DEFINES AND MACROS
===============================================================================================*/
/* Shift values needed by shift operations */
#define BITS_IN_BYTE 8

/* Shift data left by 8 X positions */
#define BYTE_SHIFT_LEFT(data, positions) ((data) << ((positions) * BITS_IN_BYTE))

/* Shift data right by 8 X positions */
#define BYTE_SHIFT_RIGHT(data, positions) ((data) >> ((positions) * BITS_IN_BYTE))

/*===============================================================================================
                                     FUNCTION PROTOTYPES
===============================================================================================*/
/* Copy the first number of bytes from pSrc to pDest. */
void MemLib_MemCpy(void *pDest, const void *pSrc, uint32 u32Size);

/* Copy Size bytes from source to destination through an intermediary buffer. */
void MemLib_MemMove(void *pDest, const void *pSrc, uint32 u32Size);

/* Set the first number of bytes in pDest to u8Data. */
void MemLib_MemSet(void *pDest, uint8 u8Data, uint32 u32Count);

/* Set the first number of bytes in pDest to u8Data using 64 bytes write*/
void MemLib_MemSet64(void *pDest, uint8 u8Data, uint32 u32Count);

/* Compare the first number of bytes between two memory strings. */
sint16 MemLib_MemCmp(const void *pStr1, const void *pStr2, uint32 u32Count);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MEMLIB_H */
