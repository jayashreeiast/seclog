/*
 *  (c) Copyright 2006-2016 Freescale Semiconductor, Inc.
 *  Copyright 2017, 2019-2025 NXP
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
#include "sample_app_ethswt_initialization.h"
#include "sample_app_ethswt_gpt.h"
#include "sample_app_ethswt_eth.h"
#include "Mcal.h"
#if defined(S32G2XX)
    #include "S32G274A.h"
#elif defined(S32G3XX)
    #include "S32G399A.h"
#endif
#include "sys_init.h"
#include "Eth_43_PFE_Irq.h"
#include "Eth_43_PFE.h"
#include "Platform.h"

#if defined(USE_SERDES_DRIVER) && defined(CONFIGURE_SOC)
    #include "CDD_Serdes.h"
#endif
#ifdef NXP_LOG_ENABLED
#include "oal.h" /* Just for debug macros definition */
#endif
/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
/* Clock frequency of PIT (Mhz) */
#define CLOCK_PIT_TIME      133

/* Macros to access register MC_ME to check PFE clock */
#define MC_ME_BASE                0x40088000U
#define MC_ME_PRTN2_STAT          0x508U
#define MC_ME_PRTN_STAT_PCS       (1U << 0)

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

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

void SampleApp_Int_Init(void);

/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#if defined(USE_SERDES_DRIVER) && defined(CONFIGURE_SOC)
extern void Serdes_MainFunction(void);
#endif

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
extern ISR(PIT_0_ISR);
extern ISR(PIT_1_ISR);

/**
* @brief      SampleApp_Int_Init - Interrupt initialization
* @details    This function is called at the initialization stage to setup interrupts priorities
*
* @return     none
* @retval     none
*
* @pre None
* @post None
*/
void SampleApp_Int_Init(void)
{
    sys_registerIsrHandler(54,&PIT_1_ISR);
    sys_enableIsrSource(54, 0x0F);

    /*  Enable/register PFE interrupts */
#if (TRUE == PFE_CFG_HIF_IRQ_ENABLED)
#if (TRUE == ETH_43_PFE_USINGHIF0)
    sys_registerIsrHandler(190, &Eth_43_PFE_HifIrqHdlr_0); /* Register HIF0 interrupt handlers */
    sys_enableIsrSource(190, (uint8)0x03U); /* Enable interrupt for HIF0 */
#endif /* ETH_43_PFE_USINGHIF0 */
#if (TRUE == ETH_43_PFE_USINGHIF1)
    sys_registerIsrHandler(191, &Eth_43_PFE_HifIrqHdlr_1); /* Register HIF1 interrupt handlers */
    sys_enableIsrSource(191, (uint8)0x03U); /* Enable interrupt for HIF1 */
#endif /* ETH_43_PFE_USINGHIF1 */
#if (TRUE == ETH_43_PFE_USINGHIF2)
    sys_registerIsrHandler(192, &Eth_43_PFE_HifIrqHdlr_2); /* Register HIF2 interrupt handlers */
    sys_enableIsrSource(192, (uint8)0x03U); /* Enable interrupt for HIF2 */
#endif /* ETH_43_PFE_USINGHIF2 */
#if (TRUE == ETH_43_PFE_USINGHIF3)
    sys_registerIsrHandler(193, &Eth_43_PFE_HifIrqHdlr_3); /* Register HIF3 interrupt handlers */
    sys_enableIsrSource(193, (uint8)0x03U); /* Enable interrupt for HIF3 */
#endif /* ETH_43_PFE_USINGHIF3 */
#if (TRUE == ETH_43_PFE_USINGHIF_NOCPY)
    sys_registerIsrHandler(195, &Eth_43_PFE_HifNoCpyIrqHdlr); /* Register HIF NOCPY interrupt handlers */
    sys_enableIsrSource(195, (uint8)0x03U); /* Enable interrupt for HIF NO_CPY */
#endif /* ETH_43_PFE_USINGHIF_NOCPY */
#endif /* PFE_CFG_HIF_IRQ_ENABLED */

#if (TRUE == PFE_CFG_BMU_IRQ_ENABLED)
    sys_registerIsrHandler(194, &Eth_43_PFE_BmuIrqHdlr); /* Register BMU interrupt handlers */
    sys_enableIsrSource(194, (uint8)0x03U); /* Enable interrupt for BMU */
#endif /* PFE_CFG_BMU_IRQ_ENABLED */
}

/**
* @brief      SampleAppInitTask - Sample Application Initialization
* @details    This function is called at the initialization stage to initialize all MCAL drivers
*
* @return     Returns the value of success
* @retval     E_OK or E_NOT_OK
*
* @pre None
* @post None
*/

Std_ReturnType SampleAppInitTask(void)
{
    /* local variables here */
    Std_ReturnType stdRet = E_OK;
#ifdef NXP_LOG_ENABLED
    volatile Gpt_ValueType TimeNow = 0U;
    volatile uint32 time1_ms = 0U;
    volatile uint32 time2_ms = 0U;
#endif
    /* === PFE interfaces initialization ======================================== */
    Eth_43_PFE_PreInit(NULL_PTR);

    /* === MCU ================================================================== */
    Mcu_Init(NULL_PTR);
#if defined(CONFIGURE_SOC)
    Mcu_InitClock(McuClockSettingConfig_0);
    while ( MCU_PLL_LOCKED != Mcu_GetPllStatus() ){}
    Mcu_DistributePllClock();
    Mcu_SetMode(McuModeSettingConf_0);
#endif /* CONFIGURE_SOC */

    /* === PLATFORM ============================================================= */
    Platform_Init(NULL_PTR);

    /* === Init Interrupts ====================================================== */
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
    OsIf_Trusted_Call(SampleApp_Int_Init);
#else
    SampleApp_Int_Init();
#endif
    /* === GPT ================================================================== */
    SampleAppGptInit();

    /* === SERDES =============================================================== */
#if defined(USE_SERDES_DRIVER) && defined(CONFIGURE_SOC)
    /* Initialize serdes with MCU in mode McuModeSettingConf_0 */
    Serdes_Init(NULL_PTR);

    /* Change mode to release all PCIe peripherals (i.e. SERDES) from reset */
    Mcu_SetMode(McuModeSettingConf_1);
    Mcu_InitClock(McuClockSettingConfig_1);
    while ( MCU_PLL_LOCKED != Mcu_GetPllStatus() ){}
    Mcu_DistributePllClock();

    /* Serdes initialization is asynchronous.
    Wait until the module is fully initialized */
    while (SERDES_INIT_PENDING == Serdes_GetJobResult())
    {
        Serdes_MainFunction();
    }

    /* Re-initialize clock muxes to enable SERDES clocks to EMACs */
    Mcu_InitClock(McuClockSettingConfig_1);
    while ( MCU_PLL_LOCKED != Mcu_GetPllStatus() ){}
    Mcu_DistributePllClock();

    /* Check that clock partition2 (i.e. PFE) is being clocked */
    if(MC_ME_PRTN_STAT_PCS != (hal_read32(MC_ME_BASE + MC_ME_PRTN2_STAT) & MC_ME_PRTN_STAT_PCS))
    {   /* It is not => we cannot  access any PFE register */
        NXP_LOG_ERROR("PFE is not clocked! Aborting initialization.\n");
        return E_NOT_OK;
    }

#endif /* USE_SERDES_DRIVER && CONFIGURE_SOC */

    /* === ETH, Switch and Transceiver ========================================== */
#ifdef NXP_LOG_ENABLED
    TimeNow = Gpt_GetTimeElapsed(0U);
    time1_ms = pitIsrCount + (((uint32)(TimeNow))/(CLOCK_PIT_TIME*1000));
#endif /* NXP_LOG_ENABLED */

    stdRet = SampleAppEthInit();

#ifdef NXP_LOG_ENABLED
    TimeNow = Gpt_GetTimeElapsed(0U);
    time2_ms = pitIsrCount + (((uint32)(TimeNow))/(CLOCK_PIT_TIME*1000));
    NXP_LOG_INFO("Performance measurement after Ethernet initialization: %ums\n", (uint_t)time2_ms);
    NXP_LOG_INFO("Time for Ethernet initialization: %ums\n", (uint_t)time2_ms - (uint_t)time1_ms);
#endif

    return stdRet;
}
/*================================================================================================*/

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef __cplusplus
}
#endif

