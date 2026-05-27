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

/*==================================================================================================
==================================================================================================*/

#ifndef MPU_CONFIGURATION_H
#define MPU_CONFIGURATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "typedefs.h"
#include "Mcal.h"
#include "oal_util_autosar.h"
#include "system.h"

#if defined(S32G2XX)
    #include "S32G274A_MPU.h"
    #include "S32G274A_SCB.h"
#elif defined(S32G3XX)
    #include "S32G399A_MPU.h"
    #include "S32G399A_SCB.h"
#endif
/*==================================================================================================
                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define CPU_MPU_MEMORY_COUNT (11U)
/*
  Region  Description         Start       End           Size[KB]  Type              Inner Cache Policy    Outer Cache Policy    Shareable    Executable    Privileged Access    Unprivileged Access
--------  -----------------   ----------  ----------  ----------  ----------------  --------------------  --------------------  -----------  ------------  -------------------  ---------------------
       0  Whole memory map    0x0         0xFFFFFFFF     4194304  Strongly Ordered  None                  None                  Yes          No            No Access            No Access
       1  QSPI AHB            0x0         0x1FFFFFFF      524288  Normal            None                  None                  No           Yes           Read/Write           Read/Write
       2  DTCM                0x20000000  0x2000FFFF          64  Normal            None                  None                  Yes          Yes           Read/Write           Read/Write
       3  HSE Shared RAM      0x22C00000  0x22C03FFF          16  Normal            None                  None                  Yes          Yes           Read/Write           Read/Write
       4  Standby RAM         0x24000000  0x24007FFF          32  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       5  SRAM(cacheable)     0x34400000  0x345FFFFF        2048  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       6  SRAM(cacheable)     0x34600000  0x347FFFFF        2048  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       7  SRAM(non-cacheable) 0x34000000  0x343FFFFF        4096  Normal            None                  None                  Yes          NO            Read/Write           Read/Write
       8  Peripherals         0x40000000  0x5FFFFFFF      524288  Strongly Ordered  None                  None                  Yes          No            Read/Write           Read/Write
       9  LLCE                0x43000000  0x43FFFFFF       16384  Strongly Ordered  None                  None                  Yes          No            Read/Write           Read/Write
       10 PPB                 0xE0000000  0xE00FFFFF        1024  Strongly Ordered  None                  None                  Yes          No            Read/Write           Read/Write
*/
                                                 /* Region 0     Region 1       Region 2     Region 3       Region 4     Region 5      Region 6       Region 7      Region 8     Region 9       Region 10 */
static const uint32 rbar[CPU_MPU_MEMORY_COUNT] = {0x00000000UL, 0x00000000UL, 0x20000000UL, 0x22C00000UL, 0x24000000UL, 0x34400000UL, 0x34600000UL, 0x34000000UL, 0x40000000UL, 0x43000000UL, 0xE0000000UL};
static const uint32 rasr[CPU_MPU_MEMORY_COUNT] = {0x1004003FUL, 0x03080039UL, 0x0308001FUL, 0x130C001BUL, 0x030B001DUL, 0x030B0029UL, 0x030B0029UL, 0x030C002BUL, 0x13040039UL, 0x1304002FUL, 0x13040027UL};
/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

void sys_m7_cache_clean(void);
void sys_m7_cache_init(void);
void Mpu_Configuration(void);

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef __cplusplus
}
#endif

#endif

