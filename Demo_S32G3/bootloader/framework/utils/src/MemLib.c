/**
*   @file           MemLib.c
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - Memory Operations Library interface implementation.
*   @details        Memory Library Operations interface implementation.
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

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdlib.h>

#include "MemLib.h"

/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/
/*================================================================================================*/
/**
* @brief      Copy a number of bytes from source array to destination array.
*
* @param[in]  pDest      Destination array
* @param[in]  pSrc       Source array
* @param[in]  u32Size    Number of bytes to be copied
*
* @return     None
*/
/*================================================================================================*/
void MemLib_MemCpy(void *pDest, const void *pSrc, uint32 u32Size)
{
    uint32 u32Index = 0U;
    
    for (; u32Index < u32Size; u32Index++)
    {
        ((uint8 *) pDest)[u32Index] = ((uint8 *) pSrc)[u32Index];
    }
}

/*================================================================================================*/
/**
* @brief     Function used to copy a source array to destination.
* @details   This is safe for overlapping memory regions, as the source buffer is firstly transferred to a temporary buffer.
*
* @param[in] pDest       The destination buffer to copy data to.
* @param[in] pSrc        The source buffer to copy data from.
* @param[in] u32Size     The size of the data to copy (in bytes).
* 
* @return    None
*/
/*================================================================================================*/
void MemLib_MemMove(void *pDest, const void *pSrc, uint32 u32Size)
{
    uint8 *pTemp = (uint8 *) malloc(sizeof(uint8) * u32Size);
    uint32 u32Index = 0U;

    /* Check if the temporary buffer allocation failed or the source/destination are invalid. */
    if ((NULL != pTemp) &&
        (NULL != pDest && NULL != pSrc))
    {
        for (u32Index = 0U; u32Index < u32Size; u32Index++)
        {
            pTemp[u32Index] = ((const uint8 *) pSrc)[u32Index];
        }
        for (u32Index = 0U; u32Index < u32Size; u32Index++)
        {
            ((uint8 *) pDest)[u32Index] = pTemp[u32Index];
        }

        free(pTemp);
    }
}

/*================================================================================================*/
/**
* @brief     Overwrite a number of destination bytes with an input byte.
*
* @param[in] pDest       The destination array
* @param[in] u8Data      The input value to overwrite with
* @param[in] u32Count    Number of bytes to overwrite
*
* @return    None
*/
/*================================================================================================*/
void MemLib_MemSet(void *pDest, uint8 u8Data, uint32 u32Count)
{
    uint32 u32Index = 0U;

    while (u32Count--)
    {
        ((uint8 *) pDest)[u32Index++] = u8Data;
    }
}

/*================================================================================================*/
/**
* @brief     Overwrite a number of destination bytes with an input byte using 8 bytes at a time
*
* @param[in] pDest       The destination array
* @param[in] u8Data      The input value to overwrite with
* @param[in] u32Count    Number of bytes to overwrite
*
* @return    None
*/
/*================================================================================================*/
void MemLib_MemSet64(void *pDest, uint8 u8Data, uint32 u32Count)
{
    uint32 u32Index = 0U;
    u32Count = u32Count / 8;
    while (u32Count--)
    {
        ((uint64 *) pDest)[u32Index++] = u8Data;
    }
}

/*================================================================================================*/
/**
* @brief     Compare the first number of bytes of two memory strings.
*
* @param[in] pStr1       Pointer to the first memory string to be compared
* @param[in] pStr2       Pointer to the second memory string to be compared
* @param[in] u32Count    Number of bytes to be compared
*
* @return    The difference between the first two corresponding bytes that are different:
*                        < 0 if the first different byte from the pStr1 is less than the one from pStr2
*                        = 0 if both memory areas' contents are the same
*                        > 0 if the first different byte from the pStr1 is greater than the one from pStr2
*/
/*================================================================================================*/
sint16 MemLib_MemCmp(const void *pStr1, const void *pStr2, uint32 u32Count)
{
    uint32 u32Index = 0U;
    sint16 s16Status = 0;
    
    if ((NULL_PTR != pStr1) && (NULL_PTR != pStr2) && (0U != u32Count))
    {
        for (; u32Index < u32Count - 1 &&
               ((uint8 *) pStr1)[u32Index] == ((uint8 *) pStr2)[u32Index]; u32Index++);
        
        s16Status = ((sint16) ((uint8 *) pStr1)[u32Index]) - ((sint16) ((uint8 *) pStr2)[u32Index]);
    }    
    
    return s16Status;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
