#
# Copyright 2022-2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#

include plat/nxp/s32/s32cc/s32g/s32g3/s32g3.mk

S32_BOARD_PATH		:= ${PLAT_SOC_PATH}/s32g399ardb3

PLAT_INCLUDES		+= -I${S32_BOARD_PATH}/include \
			   -Idrivers/nxp/flexspi/nor

DTB_FILE_NAME		?= s32g399a-rdb3.dtb

# =========================================================
# Enable FlexSPI NOR access in BL2
# (Direct NOR read/write for logging / diagnostics)
# =========================================================

BL2_SOURCES += \
	drivers/nxp/flexspi/nor/fspi.c

