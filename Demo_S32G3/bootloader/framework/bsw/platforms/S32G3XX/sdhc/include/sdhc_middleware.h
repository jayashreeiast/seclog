/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software. The production use license in
 * Section 2.3 is expressly granted for this software.
 *
 */

#ifndef SDHC_MIDDLEWARE_H
#define SDHC_MIDDLEWARE_H

#include "Usdhc_Ip_Cfg.h"

#ifdef SDHC_ENABLE_DEBUG
    /* Use PRINTF function from console.h */
    #define SDHC_DEBUG(message)    PRINTF(message)
#else
    /* Turn off SDHC DEBUG messages */
    #define SDHC_DEBUG(message)
#endif /* SDHC_ENABLE_DEBUG */

/* Usdhc host supports 1.8V operation */
#ifdef FEATURE_SDHC_HAS_V180_SUPPORT
#undef FEATURE_SDHC_HAS_V180_SUPPORT
#define FEATURE_SDHC_HAS_V180_SUPPORT                 (STD_OFF)
#else
#define FEATURE_SDHC_HAS_V180_SUPPORT                 (STD_OFF)
#endif

/* Usdhc transfer completed time out */
#define uSDHC_TC_TIMEOUT_MS      (1000U)

/*! @brief Bus width mode */
#define MMC_BUS_WIDTH_MODE       (USDHC_DATA_BUS_WIDTH_8BIT)

/*! @brief USDHC device instance number get from USDHC configuration */
#define USDHC_INSTANCE           (0U)

/*! @brief USDHC instance init configuration */
extern const Usdhc_Ip_ConfigType *g_usdhc_config;

/*! @brief USDHC State Structure declaration */
extern Usdhc_Ip_StateType *g_usdhc_state;

#endif /* SDHC_MIDDLEWARE_H */
