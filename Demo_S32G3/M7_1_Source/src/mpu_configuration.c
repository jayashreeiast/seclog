/*
 *  Copyright 2022-2025 NXP
 *
 *
 *  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By
 *  expressly accepting such terms or by downloading, installing, activating
 *  and/or otherwise using the software, you are agreeing that you have read,
 *  and that you agree to comply with and are bound by, such license terms. If
 *  you do not agree to be bound by the applicable license terms, then you may
 *  not retain, install, activate or otherwise use the software.
 *
 *  This file contains sample code only. It is not part of the production code deliverables.
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "mpu_configuration.h"
/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/
#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

void sys_m7_cache_clean(void)
{
#ifdef D_CACHE_ENABLE
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;

    S32_SCB->CSSELR = 0U; /* select Level 1 data cache */
    MCAL_DATA_SYNC_BARRIER();
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCCISW = (((sets << SCB_DCCISW_SET_SHIFT) & (uint32)SCB_DCCISW_SET_MASK) |
                      ((ways << SCB_DCCISW_WAY_SHIFT) & (uint32)SCB_DCCISW_WAY_MASK));
        MCAL_DATA_SYNC_BARRIER();
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    MCAL_DATA_SYNC_BARRIER();
    S32_SCB->CSSELR = (uint32)((S32_SCB->CSSELR) | 1U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /* D_CACHE_ENABLE */

#ifdef I_CACHE_ENABLE
    S32_SCB->ICIALLU = 0UL;
#endif /* I_CACHE_ENABLE */ 
    MCAL_DATA_SYNC_BARRIER();
}

void sys_m7_cache_disable(void)
{
    sys_m7_cache_clean();
    S32_SCB->CCR &= ~((uint32)1U << 17U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
    S32_SCB->CCR &= ~((uint32)1U << 16U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
}

void sys_m7_cache_init(void)
{
#ifdef D_CACHE_ENABLE
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;
  
    /* init Data caches */
    S32_SCB->CSSELR = 0U; /* select Level 1 data cache */

    MCAL_DATA_SYNC_BARRIER();
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCISW = (((sets << SCB_DCISW_SET_SHIFT) & SCB_DCISW_SET_MASK) |
                      ((ways << SCB_DCISW_WAY_SHIFT) & SCB_DCISW_WAY_MASK));
        MCAL_DATA_SYNC_BARRIER();
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    MCAL_DATA_SYNC_BARRIER();
    S32_SCB->CCR |=  (uint32)SCB_CCR_DC_MASK; /* enable D-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /* D_CACHE_ENABLE */

#ifdef I_CACHE_ENABLE  
    /* init Code caches */
    S32_SCB->ICIALLU = 0UL; /* invalidate I-Cache */
    S32_SCB->CCR |=  (uint32)SCB_CCR_IC_MASK;  /* enable I-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /* I_CACHE_ENABLE */
}

/**
@brief      Mpu_Configuration - MPU configuration
@details    This function is called at the beginning of main function to increase the size of Non-Cacheable region

@return     none
@retval     none

@pre None
@post None
*/
void Mpu_Configuration(void)
{
    uint8 regionNum = 0U;

    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();

    /* Checking if cache is enable before */
    if (((((uint32)1U << (uint32)17U) & S32_SCB->CCR) != (uint32)0) || ((((uint32)1U << (uint32)16U) & S32_SCB->CCR) != (uint32)0))
    {
        /* Synchronize cache before update MPU */
        sys_m7_cache_disable();
    }

    /* Disable MPU */
    S32_MPU->CTRL &= ~S32_MPU_CTRL_ENABLE_MASK;

    /* Set mpu regions with customized region 5, 6, 7 */
    for (regionNum = 0U; regionNum < CPU_MPU_MEMORY_COUNT; regionNum++)
    {
        S32_MPU->RNR  = regionNum;
        S32_MPU->RBAR = rbar[regionNum];
        S32_MPU->RASR = rasr[regionNum];
    }
    
    /* Enable MPU */
    S32_MPU->CTRL |= S32_MPU_CTRL_ENABLE_MASK;
    MCAL_INSTRUCTION_SYNC_BARRIER();
    MCAL_DATA_SYNC_BARRIER();

#if defined(D_CACHE_ENABLE) || defined(I_CACHE_ENABLE)
    sys_m7_cache_init();
#endif /* defined(D_CACHE_ENABLE) || defined(I_CACHE_ENABLE) */
}
/*================================================================================================*/

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef __cplusplus
}
#endif

