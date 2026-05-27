/**
 * @file      FlexNOC_Init.c
 * @version
 * 24.12.0
 *
 * @brief     FlexNoC functionality implementation.
 *
 *            Platform     : CORTEXM
 *            Build Version: S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
 *
 * @copyright (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *                All Rights Reserved.
 *
 *            This file contains sample code only.
 *            It is not part of the production code deliverables.
 */

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================*/
/*                                            INCLUDE FILES                                          */
/*===================================================================================================*/
#include "FlexNOC_Init.h"

/*===================================================================================================*/
/*                                        TYPE DEFINITIONS                                           */
/*===================================================================================================*/
typedef struct
{
    uint32 u32Address;
    uint32 u32Mask;
    uint32 u32Value;
    uint8  u8Shift;
} FlexNoc_ConfigType;

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/* Platform cores ID's as it is defined in MSCM_CPXNUM */
#define CORE_ID_A53_0   0U
#define CORE_ID_A53_1   1U
#define CORE_ID_A53_2   2U
#define CORE_ID_A53_3   3U
#define CORE_ID_M7_0    4U
#define CORE_ID_M7_1    5U
#define CORE_ID_M7_2    6U
#define CORE_ID_M7_3    7U
#define CORE_ID_A53_4   8U
#define CORE_ID_A53_5   9U
#define CORE_ID_A53_6   10U
#define CORE_ID_A53_7   11U

/*===================================================================================================*/
/*                                       GLOBAL CONSTANTS                                            */
/*===================================================================================================*/
static const FlexNoc_ConfigType FlexNocConfigs_M7[] =
{
/*     u32Address      u32Mask     u32Value     u8Shift      */
    /* dummy element in case no other elements are defined */
    { 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFu }

#ifdef M_A0_EDA0Q_PP0
    ,{ 0x50000708u, 0x7u, M_A0_EDA0Q_PP0, 0x0u}
#endif

#ifdef M_A0_EDA0Q_PP1
    ,{ 0x50000708u, 0x700u, M_A0_EDA0Q_PP1, 0x8u}
#endif

#ifdef M_A0_EDA0Q_MD
    ,{ 0x5000070cu, 0x3u, M_A0_EDA0Q_MD, 0x0u}
#endif

#ifdef M_A0_EDA0Q_BD
    ,{ 0x50000710u, 0xfffu, M_A0_EDA0Q_BD, 0x0u}
#endif

#ifdef M_A0_EDA0Q_ST
    ,{ 0x50000714u, 0x3ffu, M_A0_EDA0Q_ST, 0x0u}
#endif

#ifdef M_A0_EDA1Q_PP0
    ,{ 0x50000788u, 0x7u, M_A0_EDA1Q_PP0, 0x0u}
#endif

#ifdef M_A0_EDA1Q_PP1
    ,{ 0x50000788u, 0x700u, M_A0_EDA1Q_PP1, 0x8u}
#endif

#ifdef M_A0_EDA1Q_MD
    ,{ 0x5000078cu, 0x3u, M_A0_EDA1Q_MD, 0x0u}
#endif

#ifdef M_A0_EDA1Q_BD
    ,{ 0x50000790u, 0xfffu, M_A0_EDA1Q_BD, 0x0u}
#endif

#ifdef M_A0_EDA1Q_ST
    ,{ 0x50000794u, 0x3ffu, M_A0_EDA1Q_ST, 0x0u}
#endif

#ifdef M_A0_ETHM_PP0
    ,{ 0x50000808u, 0x7u, M_A0_ETHM_PP0, 0x0u}
#endif

#ifdef M_A0_ETHM_PP1
    ,{ 0x50000808u, 0x700u, M_A0_ETHM_PP1, 0x8u}
#endif

#ifdef M_A0_ETHM_MD
    ,{ 0x5000080cu, 0x3u, M_A0_ETHM_MD, 0x0u}
#endif

#ifdef M_A0_ETHM_BD
    ,{ 0x50000810u, 0x1fffu, M_A0_ETHM_BD, 0x0u}
#endif

#ifdef M_A0_ETHM_ST
    ,{ 0x50000814u, 0x3ffu, M_A0_ETHM_ST, 0x0u}
#endif

#ifdef M_A0_FRYQ_PP0
    ,{ 0x50000888u, 0x7u, M_A0_FRYQ_PP0, 0x0u}
#endif

#ifdef M_A0_FRYQ_PP1
    ,{ 0x50000888u, 0x700u, M_A0_FRYQ_PP1, 0x8u}
#endif

#ifdef M_A0_FRYQ_MD
    ,{ 0x5000088cu, 0x3u, M_A0_FRYQ_MD, 0x0u}
#endif

#ifdef M_A0_FRYQ_BD
    ,{ 0x50000890u, 0xfffu, M_A0_FRYQ_BD, 0x0u}
#endif

#ifdef M_A0_FRYQ_ST
    ,{ 0x50000894u, 0x3ffu, M_A0_FRYQ_ST, 0x0u}
#endif

#ifdef M_A0_FSNQ_PP0
    ,{ 0x50000388u, 0x7u, M_A0_FSNQ_PP0, 0x0u}
#endif

#ifdef M_A0_FSNQ_PP1
    ,{ 0x50000388u, 0x700u, M_A0_FSNQ_PP1, 0x8u}
#endif

#ifdef M_A0_FSNQ_MD
    ,{ 0x5000038cu, 0x3u, M_A0_FSNQ_MD, 0x0u}
#endif

#ifdef M_A0_FSNQ_BD
    ,{ 0x50000390u, 0xfffu, M_A0_FSNQ_BD, 0x0u}
#endif

#ifdef M_A0_FSNQ_ST
    ,{ 0x50000394u, 0x3ffu, M_A0_FSNQ_ST, 0x0u}
#endif

#ifdef M_A0_HSEQ_PP0
    ,{ 0x50000908u, 0x7u, M_A0_HSEQ_PP0, 0x0u}
#endif

#ifdef M_A0_HSEQ_PP1
    ,{ 0x50000908u, 0x700u, M_A0_HSEQ_PP1, 0x8u}
#endif

#ifdef M_A0_HSEQ_MD
    ,{ 0x5000090cu, 0x3u, M_A0_HSEQ_MD, 0x0u}
#endif

#ifdef M_A0_HSEQ_BD
    ,{ 0x50000910u, 0x1fffu, M_A0_HSEQ_BD, 0x0u}
#endif

#ifdef M_A0_HSEQ_ST
    ,{ 0x50000914u, 0x3ffu, M_A0_HSEQ_ST, 0x0u}
#endif

#ifdef M_A0_PCIQ_PP0
    ,{ 0x50000988u, 0x7u, M_A0_PCIQ_PP0, 0x0u}
#endif

#ifdef M_A0_PCIQ_PP1
    ,{ 0x50000988u, 0x700u, M_A0_PCIQ_PP1, 0x8u}
#endif

#ifdef M_A0_PCIQ_MD
    ,{ 0x5000098cu, 0x3u, M_A0_PCIQ_MD, 0x0u}
#endif

#ifdef M_A0_PCIQ_BD
    ,{ 0x50000990u, 0x1fffu, M_A0_PCIQ_BD, 0x0u}
#endif

#ifdef M_A0_PCIQ_ST
    ,{ 0x50000994u, 0x3ffu, M_A0_PCIQ_ST, 0x0u}
#endif

#ifdef M_A0_MA0Q_PP0
    ,{ 0x50020088u, 0x7u, M_A0_MA0Q_PP0, 0x0u}
#endif

#ifdef M_A0_MA0Q_PP1
    ,{ 0x50020088u, 0x700u, M_A0_MA0Q_PP1, 0x8u}
#endif

#ifdef M_A0_MA0Q_MD
    ,{ 0x5002008cu, 0x3u,M_A0_MA0Q_MD, 0x0u}
#endif

#ifdef M_A0_MA0Q_BD
    ,{ 0x50020090u, 0xfffu,M_A0_MA0Q_BD, 0x0u}
#endif

#ifdef M_A0_MA0Q_ST
    ,{ 0x50020094u, 0x3ffu, M_A0_MA0Q_ST, 0x0u}
#endif

#ifdef M_A0_MA1Q_PP0
    ,{ 0x50021088u, 0x7u, M_A0_MA1Q_PP0, 0x0u}
#endif

#ifdef M_A0_MA1Q_PP1
    ,{ 0x50021088u, 0x700u, M_A0_MA1Q_PP1, 0x8u}
#endif

#ifdef M_A0_MA1Q_MD
    ,{ 0x5002108cu, 0x3u, M_A0_MA1Q_MD, 0x0u}
#endif

#ifdef M_A0_MA1Q_BD
    ,{ 0x50021090u, 0xfffu, M_A0_MA1Q_BD, 0x0u}
#endif

#ifdef M_A0_MA1Q_ST
    ,{ 0x50021094u, 0x3ffu, M_A0_MA1Q_ST, 0x0u}
#endif

#ifdef M_A0_MA2Q_PP0
    ,{ 0x50022088u, 0x7u, M_A0_MA2Q_PP0, 0x0u}
#endif

#ifdef M_A0_MA2Q_PP1
    ,{ 0x50022088u, 0x700u, M_A0_MA2Q_PP1, 0x8u}
#endif

#ifdef M_A0_MA2Q_MD
    ,{ 0x5002208cu, 0x3u, M_A0_MA2Q_MD, 0x0u}
#endif

#ifdef M_A0_MA2Q_BD
    ,{ 0x50022090u, 0xfffu, M_A0_MA2Q_BD, 0x0u}
#endif

#ifdef M_A0_MA2Q_ST
    ,{ 0x50022094u, 0x3ffu, M_A0_MA2Q_ST, 0x0u}
#endif

#ifdef M_A0_MA3Q_PP0
    ,{ 0x50023088u, 0x7u, M_A0_MA3Q_PP0, 0x0u}
#endif

#ifdef M_A0_MA3Q_PP1
    ,{ 0x50023088u, 0x700u, M_A0_MA3Q_PP1, 0x8u}
#endif

#ifdef M_A0_MA3Q_MD
    ,{ 0x5002308cu, 0x3u, M_A0_MA3Q_MD, 0x0u}
#endif

#ifdef M_A0_MA3Q_BD
    ,{ 0x50023090u, 0xfffu, M_A0_MA3Q_BD, 0x0u}
#endif

#ifdef M_A0_MA3Q_ST
    ,{ 0x50023094u, 0x3ffu, M_A0_MA3Q_ST, 0x0u}
#endif

#ifdef PFE_DDRQ_PP0
    ,{ 0x47001508u, 0x7u, PFE_DDRQ_PP0, 0x0u}
#endif

#ifdef PFE_DDRQ_PP1
    ,{ 0x47001508u, 0x700u, PFE_DDRQ_PP1, 0x8u}
#endif

#ifdef PFE_DDRQ_MD
    ,{ 0x4700150cu, 0x3u, PFE_DDRQ_MD, 0x0u}
#endif

#ifdef PFE_DDRQ_BD
    ,{ 0x47001510u, 0xfffu, PFE_DDRQ_BD, 0x0u}
#endif

#ifdef PFE_DDRQ_ST
    ,{ 0x47001514u, 0x3ffu, PFE_DDRQ_ST, 0x0u}
#endif

#ifdef PFE_DDWQ_PP0
    ,{ 0x47000088u, 0x7u, PFE_DDWQ_PP0, 0x0u}
#endif

#ifdef PFE_DDWQ_PP1
    ,{ 0x47000088u, 0x700u, PFE_DDWQ_PP1, 0x8u}
#endif

#ifdef PFE_DDWQ_MD
    ,{ 0x4700008cu, 0x3u, PFE_DDWQ_MD, 0x0u}
#endif

#ifdef PFE_DDWQ_BD
    ,{ 0x47000090u, 0xfffu, PFE_DDWQ_BD, 0x0u}
#endif

#ifdef PFE_DDWQ_ST
    ,{ 0x47000094u, 0x3ffu, PFE_DDWQ_ST, 0x0u}
#endif

#ifdef PFE_HF0Q_PP0
    ,{ 0x47000108u, 0x7u, PFE_HF0Q_PP0, 0x0u}
#endif

#ifdef PFE_HF0Q_PP1
    ,{ 0x47000108u, 0x700u, PFE_HF0Q_PP1, 0x8u}
#endif

#ifdef PFE_HF0Q_MD
    ,{ 0x4700010cu, 0x3u, PFE_HF0Q_MD, 0x0u}
#endif

#ifdef PFE_HF0Q_BD
    ,{ 0x47000110u, 0xfffu, PFE_HF0Q_BD, 0x0u}
#endif

#ifdef PFE_HF0Q_ST
    ,{ 0x47000114u, 0x3ffu, PFE_HF0Q_ST, 0x0u}
#endif

#ifdef PFE_HF1Q_PP0
    ,{ 0x47000188u, 0x7u, PFE_HF1Q_PP0, 0x0u}
#endif

#ifdef PFE_HF1Q_PP1
    ,{ 0x47000188u, 0x700u, PFE_HF1Q_PP1, 0x8u}
#endif

#ifdef PFE_HF1Q_MD
    ,{ 0x4700018cu, 0x3u, PFE_HF1Q_MD, 0x0u}
#endif

#ifdef PFE_HF1Q_BD
    ,{ 0x47000190u, 0xfffu, PFE_HF1Q_BD, 0x0u}
#endif

#ifdef PFE_HF1Q_ST
    ,{ 0x47000194u, 0x3ffu, PFE_HF1Q_ST, 0x0u}
#endif

#ifdef PFE_HF2Q_PP0
    ,{ 0x47000208u, 0x7u, PFE_HF2Q_PP0, 0x0u}
#endif

#ifdef PFE_HF2Q_PP1
    ,{ 0x47000208u, 0x700u, PFE_HF2Q_PP1, 0x8u}
#endif

#ifdef PFE_HF2Q_MD
    ,{ 0x4700020cu, 0x3u, PFE_HF2Q_MD, 0x0u}
#endif

#ifdef PFE_HF2Q_BD
    ,{ 0x47000210u, 0xfffu, PFE_HF2Q_BD, 0x0u}
#endif

#ifdef PFE_HF2Q_ST
    ,{ 0x47000214u, 0x3ffu, PFE_HF2Q_ST, 0x0u}
#endif

#ifdef PFE_HF3Q_PP0
    ,{ 0x47000288u, 0x7u, PFE_HF3Q_PP0, 0x0u}
#endif

#ifdef PFE_HF3Q_PP1
    ,{ 0x47000288u, 0x700u, PFE_HF3Q_PP1, 0x8u}
#endif

#ifdef PFE_HF3Q_MD
    ,{ 0x4700028cu, 0x3u, PFE_HF3Q_MD, 0x0u}
#endif

#ifdef PFE_HF3Q_BD
    ,{ 0x47000290u, 0xfffu, PFE_HF3Q_BD, 0x0u}
#endif

#ifdef PFE_HF3Q_ST
    ,{ 0x47000294u, 0x3ffu, PFE_HF3Q_ST, 0x0u}
#endif

#ifdef PFE_UTLQ_PP0
    ,{ 0x47000308u, 0x7u, PFE_UTLQ_PP0, 0x0u}
#endif

#ifdef PFE_UTLQ_PP1
    ,{ 0x47000308u, 0x700u, PFE_UTLQ_PP1, 0x8u}
#endif

#ifdef PFE_UTLQ_MD
    ,{ 0x4700030cu, 0x3u, PFE_UTLQ_MD, 0x0u}
#endif

#ifdef PFE_UTLQ_BD
    ,{ 0x47000310u, 0xfffu, PFE_UTLQ_BD, 0x0u}
#endif

#ifdef PFE_UTLQ_ST
    ,{ 0x47000314u, 0x3ffu, PFE_UTLQ_ST, 0x0u}
#endif

#ifdef UB_MNQ_PP0
    ,{ 0x47000388u, 0x7u, UB_MNQ_PP0, 0x0u}
#endif

#ifdef UB_MNQ_PP1
    ,{ 0x47000388u, 0x700u, UB_MNQ_PP1, 0x8u}
#endif

#ifdef UB_MNQ_MD
    ,{ 0x4700038cu, 0x3u, UB_MNQ_MD, 0x0u}
#endif

#ifdef M_A0_FELQ_PP0
    ,{ 0x50018408u, 0x7u, M_A0_FELQ_PP0, 0x0u}
#endif

#ifdef M_A0_FELQ_PP1
    ,{ 0x50018408u, 0x700u, M_A0_FELQ_PP1, 0x8u}
#endif

#ifdef M_A0_FELQ_MD
    ,{ 0x5001840cu, 0x3u, M_A0_FELQ_MD, 0x0u}
#endif

#ifdef M_A0_FELQ_BD
    ,{ 0x50018410u, 0xfffu, M_A0_FELQ_BD, 0x0u}
#endif

#ifdef M_A0_FELQ_ST
    ,{ 0x50018414u, 0x3ffu, M_A0_FELQ_ST, 0x0u}
#endif

};

static const FlexNoc_ConfigType FlexNocConfigs_A53[] =
{
/*     u32Address      u32Mask     u32Value     u8Shift      */
    /* dummy element in case no other elements are defined */
    { 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFu }

#ifdef CA0_Q_PP0
    ,{ 0x50500108u, 0x3u, CA0_Q_PP0, 0x0u}
#endif

#ifdef CA0_Q_PP1
    ,{ 0x50500108u, 0x300u, CA0_Q_PP1, 0x8u}
#endif

#ifdef CA1_Q_PP0
    ,{ 0x50500208u, 0x3u, CA1_Q_PP0, 0x0u}
#endif

#ifdef CA1_Q_PP1
    ,{ 0x50500208u, 0x300u, CA1_Q_PP1, 0x8u}
#endif

#ifdef M_A0_C0_NCMQ_PP0
    ,{ 0x50000408u, 0x7u, M_A0_C0_NCMQ_PP0, 0x0u}
#endif

#ifdef M_A0_C0_NCMQ_PP1
    ,{ 0x50000408u, 0x700u, M_A0_C0_NCMQ_PP1, 0x8u}
#endif

#ifdef M_A0_C0_NCMQ_MD
    ,{ 0x5000040cu, 0x3u, M_A0_C0_NCMQ_MD, 0x0u}
#endif

#ifdef M_A0_C0_NCMQ_BD
    ,{ 0x50000410u, 0x1fffu, M_A0_C0_NCMQ_BD, 0x0u}
#endif

#ifdef M_A0_C0_NCMQ_ST
    ,{ 0x50000414u, 0x3ffu, M_A0_C0_NCMQ_ST, 0x0u}
#endif

#ifdef M_A0_C1_NCMQ_PP0
    ,{ 0x50000488u, 0x7u, M_A0_C1_NCMQ_PP0, 0x0u}
#endif

#ifdef M_A0_C1_NCMQ_PP1
    ,{ 0x50000488u, 0x700u, M_A0_C1_NCMQ_PP1, 0x8u}
#endif

#ifdef M_A0_C1_NCMQ_MD
    ,{ 0x5000040cu, 0x3u, M_A0_C1_NCMQ_MD, 0x0u}
#endif

#ifdef M_A0_C1_NCMQ_BD
    ,{ 0x50000490u, 0x1fffu, M_A0_C1_NCMQ_BD, 0x0u}
#endif

#ifdef M_A0_C1_NCMQ_ST
    ,{ 0x50000494u, 0x3ffu, M_A0_C1_NCMQ_ST, 0x0u}
#endif

#ifdef M_A0_CTOMQ_PP0
    ,{ 0x50000508u, 0x7u, M_A0_CTOMQ_PP0, 0x0u}
#endif

#ifdef M_A0_CTOMQ_PP1
    ,{ 0x50000508u, 0x700u, M_A0_CTOMQ_PP1, 0x8u}
#endif

#ifdef M_A0_CTOMQ_MD
    ,{ 0x5000050cu, 0x3u, M_A0_CTOMQ_MD, 0x0u}
#endif

#ifdef M_A0_CTOMQ_BD
    ,{ 0x50000510u, 0x1fffu, M_A0_CTOMQ_BD, 0x0u}
#endif

#ifdef M_A0_CTOMQ_ST
    ,{ 0x50000514u, 0x3ffu, M_A0_CTOMQ_ST, 0x0u}
#endif

#ifdef M_A0_CTOMQ_ES
    ,{ 0x50000518u, 0x1u, M_A0_CTOMQ_ES, 0x0u}
#endif

#ifdef M_A0_CG5Q_PP0
    ,{ 0x50018488u, 0x7u, M_A0_CG5Q_PP0, 0x0u}
#endif

#ifdef M_A0_CG5Q_PP1
    ,{ 0x50018488u, 0x700u, M_A0_CG5Q_PP1, 0x8u}
#endif

#ifdef M_A0_CG5Q_MD
    ,{ 0x5001848cu, 0x3u, M_A0_CG5Q_MD, 0x0u}
#endif

#ifdef M_A0_CG5Q_BD
    ,{ 0x50018490u, 0xfffu, M_A0_CG5Q_BD, 0x0u}
#endif

#ifdef M_A0_CG5Q_ST
    ,{ 0x50018494u, 0x3ffu, M_A0_CG5Q_ST, 0x0u}
#endif
};

const uint16 M7_CONFIGS_NUMBER = sizeof(FlexNocConfigs_M7)/sizeof(FlexNoc_ConfigType);
const uint16 A53_CONFIGS_NUMBER = sizeof(FlexNocConfigs_A53)/sizeof(FlexNoc_ConfigType);

/*===================================================================================================*/
/*                                        FUNCTION DEFINITIONS                                       */
/*===================================================================================================*/
void FlexNOC_InitNoc(uint8 core)
{
    uint16 u16Configs;
    FlexNoc_ConfigType const *pCfg;
    FlexNoc_ConfigType const *pConfigArray;
    uint32 u32RegValueOld;
    uint32 u32RegValue;
    uint16 u16Idx = 0u;

    if(core == CORE_ID_M7_0)
    {
        u16Configs = M7_CONFIGS_NUMBER;
        pConfigArray = FlexNocConfigs_M7;
    }

    if(core == CORE_ID_A53_0)
    {
        u16Configs = A53_CONFIGS_NUMBER;
        pConfigArray = FlexNocConfigs_A53;
    }

    for (u16Idx = 1; u16Idx < u16Configs; u16Idx++)
    {
        pCfg = &pConfigArray[u16Idx];

        u32RegValueOld = REG_READ32(pCfg->u32Address) & pCfg->u32Mask;
        u32RegValue = (pCfg->u32Value << pCfg->u8Shift) & pCfg->u32Mask;

        if (u32RegValueOld != u32RegValue)
        {
            u32RegValue |= (REG_READ32(pCfg->u32Address) & ~pCfg->u32Mask);
            REG_WRITE32(pCfg->u32Address, u32RegValue);
        }
    }
}

#ifdef __cplusplus
}
#endif