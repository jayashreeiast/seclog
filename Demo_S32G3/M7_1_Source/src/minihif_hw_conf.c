/*
 *  Copyright 2019-2022, 2025 NXP
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
 *
 */
/**
* @brief    Hardware configuration for detached minihif driver.
* @details  Function configure_minihif_hw configures a HIF interface, an EMAC, and logical interfaces
*           to be used from DETACHED minihif driver running in another application.
*
*           This code is expected to run in the application containing also PFE MCAL driver. It calls
*           some PFE specific API from the MCAL driver. It also needs includes from MCAL driver,
*           including the pfe_cfg.h and pfe_platform_cfg.h.
*
*   @addtogroup EXAMPLE_APP
*   @{
*/

/* Minihif API */
#include "pfe_minihif_drv.h"
#include "pfe_platform.h"

/* Option to enable/disable errata ERR051211 workaround (default is enabled) */
#define PFE_HIF_ERRATA_ERR051211_WORKAROUND_ENABLED TRUE

/* Keep in sync with detached MINIHIF application */
#define MINI_RX_RING_VA     ((void *)(0x34110000U))
#define MINI_TX_RING_VA     ((void *)(0x34111000U))
#define MINI_RX_WB_RING_VA  ((void *)(0x34112000U))
#define MINI_TX_WB_RING_VA  ((void *)(0x34112800U))

#if (TRUE == PFE_HIF_ERRATA_ERR051211_WORKAROUND_ENABLED)
/* Due to HW issue, we will reserve more 72 BDs for Rx ring for errata ERR051211 workaround */
#define MINI_RX_BD_LENGTH   (32U + 72U)
#else
#define MINI_RX_BD_LENGTH   (32U)
#endif  /* PFE_HIF_ERRATA_ERR051211_WORKAROUND_ENABLED */

#define MINI_TX_BD_LENGTH   16U

#define ETH_43_PFE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_43_PFE_MemMap.h"

static pfe_minihif_drv_t minihif = {0U};

#define ETH_43_PFE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

/*  Note that minihif initialization requires prior MCAL driver initialization,
    call function Eth_43_PFE_Init first.
    
    For hardware configuration (EMACs, HIFs) we rely on master driver and application.
    We are expecting the EMACs and HIFs to be configured in L2 bridge mode.
    The minihif user should use MAC addresses (not injection) for traffic steering.
*/
void configure_minihif_hw(void)
{

    if(EOK != pfe_minihif_drv_create(&minihif, MINIHIF_HW_USE, FALSE))
    {
        NXP_LOG_ERROR("Minihifs creation failed\n");
        return;
    }

    if( (EOK != pfe_minihif_drv_attach_ring(&minihif, TRUE, MINI_RX_RING_VA, MINI_RX_WB_RING_VA, MINI_RX_BD_LENGTH))
     || (EOK != pfe_minihif_drv_attach_ring(&minihif, FALSE, MINI_TX_RING_VA, MINI_TX_WB_RING_VA, MINI_TX_BD_LENGTH))
      )
    {
        NXP_LOG_ERROR("MINIHIF - fixed buffers configuration failed\n");
        return;
    }

    if (EOK != pfe_minihif_drv_init(&minihif))
    {
        NXP_LOG_ERROR("minihif_drv_init failed\n");
        return;
    }
    if (EOK != pfe_minihif_drv_start_rx(&minihif))
    {
        NXP_LOG_ERROR("HIF driver reception start failed\n");
        return;
    }
    if (EOK != pfe_minihif_drv_start_tx(&minihif))
    {
        NXP_LOG_ERROR("HIF driver transmission start failed\n");
        return;
    }
}

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"
