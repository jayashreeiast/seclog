/*================================================================================================*/
/*
*   @file              interrupts.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @version           24.12.0
*
*   @brief             Interrupt handlers.
*   @details           Interrupt handlers use for safety context.
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "sys_init.h"
#include "Saf_Stubs.h"
#if (STD_ON == BL_USE_SAFETY)
#include "interrupts.h"

#if (SAFETY_BASE_PLATFORM_ARM != SAFETY_BASE_ARMV8_AARCH64)
#include "mSel.h"
#endif /*SAFETY_BASE_PLATFORM_ARM != SAFETY_BASE_ARMV8_AARCH64*/

#include "Bootloader_Specific.h"
#include "S32G399A_COMMON.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
int_function_t g_INT_vectors[INT_NUM_SOURCES];

#if (SAFETY_BASE_PLATFORM_ARM != SAFETY_BASE_ARMV8_AARCH64)
uint32 u32SecondsCounter;
#endif /*SAFETY_BASE_PLATFORM_ARM != SAFETY_BASE_ARMV8_AARCH64)*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
/**
@brief       ISR_SWTx
@details     This is ISR for SWT interrupts
@param[in]   u8SwtId     Index of the SWT with active IRQ
 */
static void ISR_SWTx(uint8 u8SwtId)
{
    if( u8SwtId < 8U )
    {
        uint32 coreId = sys_getCoreId();

        if( sCheck_Fwd[coreId].ISR_SWT[u8SwtId] != NULL_PTR )
        {
            /* sCheck takes care of the interrupt */
            sCheck_Fwd[coreId].ISR_SWT[u8SwtId]();
        }
        else
        {
            /* Trap */
            while(1U);
        }
    }
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
@brief      Interrupt handler for SWT0 IRQ
@details    Interrupt handler for Software timer dedicated to M7 Core
 */
void ISR_SWT0(void)
{
    ISR_SWTx(0U);
}

/**
@brief      Interrupt handler for SWT1 IRQ
@details    Interrupt handler for Software timer dedicated to M7 Core
 */
void ISR_SWT1(void)
{
    ISR_SWTx(1U);
}

/**
@brief      Interrupt handler for SWT2 IRQ
@details    Interrupt handler for Software timer dedicated to M7 Core
 */
void ISR_SWT2(void)
{
    ISR_SWTx(2U);
}

/**
@brief      Interrupt handler for SWT3 IRQ
@details    Interrupt handler for Software timer dedicated to M7 Core
 */
void ISR_SWT3(void)
{
    ISR_SWTx(3U);
}

/**
@brief      Interrupt handler for SWT4 IRQ
@details    Interrupt handler for Software timer dedicated to A53 Core
 */
void ISR_SWT4(void)
{
    ISR_SWTx(4U);
}

/**
@brief      Interrupt handler for SWT5 IRQ
@details    Interrupt handler for Software timer dedicated to A53 Core
 */
void ISR_SWT5(void)
{
    ISR_SWTx(5U);
}

/**
@brief      Interrupt handler for SWT6 IRQ
@details    Interrupt handler for Software timer dedicated to A53 Core
 */
void ISR_SWT6(void)
{
    ISR_SWTx(6U);
}

/**
@brief      Interrupt handler for SWT7 IRQ
@details    Interrupt handler for Software timer dedicated to A53 Core
 */
void ISR_SWT7(void)
{
    ISR_SWTx(7U);
}

/**
@brief Interrupt handler for SBSW IRQ
 */
void ISR_SBSW(void)
{
    uint32 coreId = sys_getCoreId();

    if( sCheck_Fwd[coreId].ISR_SBSW != NULL_PTR )
    {
        /* sCheck takes care of the interrupt */
        sCheck_Fwd[coreId].ISR_SBSW();
    }
    else
    {
        /* Trap */
        while(1U);
    }
}

/**
 * @brief Interrupt service routine for incoming sCheck events
 */
void ISR_sCheckEvent(void)
{
    uint32 isrStatus;
    uint32 i;

    /* Read interrupt status register - MSI Interrupt 1 on CM7_0 */
    isrStatus = REG_READ32(MSCM_IRCP4ISR1);
    /* Clear interrupt status register - MSI Interrupt 1 on CM7_0 */
    REG_WRITE32(MSCM_IRCP4ISR1, isrStatus);

    for( i = 0UL; i < 8UL; i++ )
    {
        if( (isrStatus & (1UL << i)) != 0UL )
        {
            sCheck_RecvEvent( (sCheck_CoreIdType)i );
        }
    }
}

#endif /* STD_ON == BL_USE_SAFETY */

#ifdef __cplusplus
}
#endif
