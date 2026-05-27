/*
 * Copyright 2025 NXP.
 */

/*==================================================================================================
*                                         INCLUDE FILES
==================================================================================================*/
#include "gic.h"
#include "hw_system.h"
#include "FreeRTOSConfig.h"
#include "Platform_Types.h"
#include "S32G399A_MSCM.h"

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/
#define CPACR_FPEN_Pos              20U
#define CPACR_FPEN_Msk              (0x3UL << CPACR_FPEN_Pos)

#define CA53_0_0 (0UL)
#define CA53_0_1 (1UL)
#define CA53_1_0 (2UL)
#define CA53_1_1 (3UL)
#define CM7_0    (4UL)
#define CM7_1    (5UL)
#define CM7_2    (6UL)
#define CM7_3    (7UL)

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
void write_CPTR_EL3(uint32_t);
void write_CPTR_EL2(uint32_t);
void write_CPACR_EL1(uint32_t);

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
void FPU_Init(void);
void SystemInit(void) __attribute__ ((section (".systeminit")));
void SystemInit(void)
{
    /* enable clock for both FTMs */
    MC_ME_PRTN0_PUPD = 0x1; /* PCUD */
    MC_ME_PRTN0_PCONF = 0x1; /* PCE */
    MC_ME_CTL_KEY = 0x5AF0;
    MC_ME_CTL_KEY = 0xA50F;

    /* Enable FXOSC */
    FXOSC_CTRL |= 0x1;

    vGicInit();
#if (configENABLE_FPU == 1)
    FPU_Init();
#endif
}

void FPU_Init(void)
{
    /* Access is always enabled to SIMD & FP registers in AArch64 */

    /* Disable access trapping to SIMD & FP registers in EL3 and EL2 */
    write_CPTR_EL3(0);
    write_CPTR_EL2(0);

    /* Disable access trapping to SIMD & FP registers in EL1 and EL0 */
    write_CPACR_EL1(CPACR_FPEN_Msk);
    ASM_KEYWORD("isb");
}

uint8 Sys_GetCoreID(void)
{
    return ((uint8)(IP_MSCM->CPXNUM & MSCM_CPXNUM_CPN_MASK));
}
