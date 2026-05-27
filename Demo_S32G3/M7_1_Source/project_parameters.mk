#  Copyright 2020-2025 NXP
#
#  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
#  be used strictly in accordance with the applicable license terms. By
#  expressly accepting such terms or by downloading, installing, activating
#  and/or otherwise using the software, you are agreeing that you have read,
#  and that you agree to comply with and are bound by, such license terms. If
#  you do not agree to be bound by the applicable license terms, then you may
#  not retain, install, activate or otherwise use the software.
#
#  This file contains sample code only. It is not part of the production code deliverables.

#The path to the compiler instalation dir
GHS_DIR ?= c:/ghs/comp_202014
GCC_DIR ?= c:/NXP/S32DS.3.5/S32DS/build_tools/gcc_v9.2/gcc-9.2-arm32-eabi
DIAB_DIR ?= c:/WindRiver/compilers/diab-7.0.3.0

#The path to the Tresos plugins directory
TRESOS_DIR ?= C:/EB/tresos
PLUGINS_DIR ?= C:/NXP/SW32G_RTD_4.4_5.0.0/eclipse/plugins
TRESOS_WORKSPACE_DIR ?= Tresos_M7_1_Project/PFE_Master_S32G3
# TRESOS_WORKSPACE_DIR ?= $(TRESOS_DIR)/workspace
FW_DIR ?= pfe_firmware
LWIP_DIR ?= ../lwip

#RTD modules used
MCAL_MODULE_LIST :=  BaseNXP Det Dem Gpt Mcu Mcl Rte Platform EthSwt EthTrcv
ifeq ($(SLAVE_DRIVER),FALSE)
    MCAL_MODULE_LIST += Port
    ifeq ($(USE_SERDES_DRIVER),TRUE)
        MCAL_MODULE_LIST += Serdes
    endif
endif

#The package name for the RTD release
AR_MCAL_PKG_NAME = TS_T40D11M50I0R0

#The package name for the PFE MCAL release
AR_PFE_PKG_NAME = TS_T40D11M15I0R0
