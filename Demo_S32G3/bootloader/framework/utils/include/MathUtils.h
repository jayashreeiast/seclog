/**
*   @file           MathUtils.h
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - Common mathematical operations header
*   @details        Common mathematical operations header
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

#ifndef MATHUTILS_H
#define MATHUTILS_H

#ifdef __cplusplus
extern "C"{
#endif

/*===============================================================================================
                                        PARAMETERIZED MACROS
===============================================================================================*/
/* The absolute difference between two values */
#define ABS_DIFF(a, b) ((a) >= (b) ? (a) - (b) : (b) - (a))

/* The absolute value of a number */
#define ABS_VAL(a) (((a) < 0) ? -a : a)

/* The minimum value between two numbers */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* The maximum value between two numbers */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Fast modulo operator implementation */
#define MOD(a, b) ((a) >= (b) ? (a) % (b) : (a))

/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif  /* MATHUTILS_H */
