/**
*    @file        typedefs.h
*    @version     24.12.0
*
*    @brief       Integration Framework - global type definition header
*
*    (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/
/*=================================================================================================
*    Platform      :      CORTEXM
*    Build Version :      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
==================================================================================================*/

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_


#include <stdint.h>

#define SWCRTE_STD_ON   1U
#define SWCRTE_STD_OFF  0U
#define SWCRTE_TRUE   1U
#define SWCRTE_FALSE  0U

typedef unsigned char boolean_t;

/* Standard typedefs used by header files, based on ISO C standard */
typedef volatile int8_t vint8_t;
typedef volatile uint8_t vuint8_t;

typedef volatile int16_t vint16_t;
typedef volatile uint16_t vuint16_t;

typedef volatile int32_t vint32_t;
typedef volatile uint32_t vuint32_t;

typedef volatile int64_t vint64_t;
typedef volatile uint64_t vuint64_t;


#endif /* _TYPEDEFS_H_ */
