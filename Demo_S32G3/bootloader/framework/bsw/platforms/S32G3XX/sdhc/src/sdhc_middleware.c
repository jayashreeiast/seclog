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

#include "sdhc_middleware.h"

static Usdhc_Ip_StateType usdhc_State;

/*! @brief USDHC instance init configuration */
const Usdhc_Ip_ConfigType *g_usdhc_config = &EepUsdhcControllerCfg_0;

/*! @brief USDHC State Structure declaration */
Usdhc_Ip_StateType *g_usdhc_state = &usdhc_State;
