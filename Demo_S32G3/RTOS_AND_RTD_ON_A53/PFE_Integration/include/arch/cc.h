/*
 *  Copyright 2023-2025 NXP
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

#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <stdlib.h>
#include "oal_util_autosar.h"

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
NXP_LOG_RAW_ERROR(message); handler;} } while(0)

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(param) do {NXP_LOG_DEBUG param;} while(0)
#endif /* LWIP_PLATFORM_DIAG */

#if !defined(BKPT_ASM)

#if defined ( __DCC__ )
#define BKPT_ASM __asm ("BKPT 0\n\t")
#else
#define BKPT_ASM __asm ("BKPT #0\n\t")
#endif

#endif /* BKPT_ASM */

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) do { BKPT_ASM; } while(1)
#endif /* LWIP_PLATFORM_ASSERT */

/* C runtime functions redefined */
#define LWIP_RAND() ((u32_t)rand())

#define PPP_INCLUDE_SETTINGS_HEADER

#endif /* LWIP_ARCH_CC_H */
