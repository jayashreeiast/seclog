/**
 * @file      FlexNOC_Init.h
 * @version   24.12.0
 *
 * @brief     FlexNoC functionality specific defines.
 * @details   This file adds the defines needed for the FlexNoC registers initialization.
 *            In the first part (until the structure defines) there are values
 *            to be set into the registers, and those can be changed if needed.
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


 #ifndef __FLEX_NOC_INIT_H
 #define __FLEX_NOC_INIT_H

 #ifdef __cplusplus
 extern "C"{
 #endif

/*===================================================================================================*/
/*                                            INCLUDE FILES                                          */
/*===================================================================================================*/
#include "S32G399A_M7_COMMON.h"

/*===================================================================================================*/
/*                                               MACROS                                              */
/*===================================================================================================*/

 #define CA0_Q_PP0                                  2u
 #define CA0_Q_PP1                                  0u

 #define CA1_Q_PP0                                  2u
 #define CA1_Q_PP1                                  0u

 #define M_A0_C0_NCMQ_PP0                           4u
 #define M_A0_C0_NCMQ_PP1                           5u
 /* #define M_A0_C0_NCMQ_MD                         0u */
 #define M_A0_C0_NCMQ_BD                            197u
 /* #define M_A0_C0_NCMQ_ST                         64u */

 #define M_A0_C1_NCMQ_PP0                           3u
 #define M_A0_C1_NCMQ_PP1                           5u
 /* #define M_A0_C1_NCMQ_MD                         0u */
 #define M_A0_C1_NCMQ_BD                            197u
 /* #define M_A0_C1_NCMQ_ST                         64u */

 #define M_A0_CTOMQ_PP0                             1u
 #define M_A0_CTOMQ_PP1                             5u
 /* #define M_A0_CTOMQ_MD                           0u */
 #define M_A0_CTOMQ_BD                              984u
 /* #define M_A0_CTOMQ_ST                           64u */
 #define M_A0_CTOMQ_ES                              1u

 #define M_A0_EDA0Q_PP0                              6u
 #define M_A0_EDA0Q_PP1                              6u
 /* #define M_A0_EDA0Q_MD                           0u */
 /* #define M_A0_EDA0Q_BD                           100u */
 /* #define M_A0_EDA0Q_ST                           64u */

 #define M_A0_EDA1Q_PP0                             6u
 #define M_A0_EDA1Q_PP1                             6u
 /* #define M_A0_EDA1Q_MD                           0u */
 /* #define M_A0_EDA1Q_BD                           100u */
 /* #define M_A0_EDA1Q_ST                           64u */

 #define M_A0_ETHM_PP0                              3u
 #define M_A0_ETHM_PP1                              5u
 /* #define M_A0_ETHM_MD                            0u */
 /* #define M_A0_ETHM_BD                            125u */
 /* #define M_A0_ETHM_ST                            64u */

 #define M_A0_FRYQ_PP0                              7u
 #define M_A0_FRYQ_PP1                              7u
 /* #define M_A0_FRYQ_MD                            0u */
 #define M_A0_FRYQ_BD                               38u
 /* #define M_A0_FRYQ_ST                            64u */

 #define M_A0_FELQ_PP0                              7u
 #define M_A0_FELQ_PP1                              7u
 /* #define M_A0_FELQ_MD                            0u */
 /* #define M_A0_FELQ_BD                            100u */
 /* #define M_A0_FELQ_ST                            64u */

 #define M_A0_FSNQ_PP0                              5u
 #define M_A0_FSNQ_PP1                              5u
 /* #define M_A0_FSNQ_MD                            0u */
 /* #define M_A0_FSNQ_BD                            100u */
 /* #define M_A0_FSNQ_ST                            64u */

 #define M_A0_CG5Q_PP0                              5u
 #define M_A0_CG5Q_PP1                              5u
 /* #define M_A0_CG5Q_MD                            0u */
 #define M_A0_CG5Q_BD                               20u
 /* #define M_A0_CG5Q_ST                            64u */

 #define M_A0_HSEQ_PP0                              6u
 #define M_A0_HSEQ_PP1                              6u
 /* #define M_A0_HSEQ_MD                            0u */
 /* #define M_A0_HSEQ_BD                            20u */
 /* #define M_A0_HSEQ_ST                            64u */

 #define M_A0_PCIQ_PP0                              1u
 #define M_A0_PCIQ_PP1                              5u
 /* #define M_A0_PCIQ_MD                            3u */
 #define M_A0_PCIQ_BD                               640u
 /* #define M_A0_PCIQ_ST                            64u */

 #define M_A0_MA0Q_PP0                              4u
 #define M_A0_MA0Q_PP1                              5u
 /* #define M_A0_MA0Q_MD                            0u */
 #define M_A0_MA0Q_BD                               64u
 /* #define M_A0_MA0Q_ST                            64u */

 #define M_A0_MA1Q_PP0                              4u
 #define M_A0_MA1Q_PP1                              5u
 /* #define M_A0_MA1Q_MD                            0u */
 #define M_A0_MA1Q_BD                               64u
 /* #define M_A0_MA1Q_ST                            64u */

 #define M_A0_MA2Q_PP0                              4u
 #define M_A0_MA2Q_PP1                              5u
 /* #define M_A0_MA2Q_MD                            0u */
 #define M_A0_MA2Q_BD                               64u
 /* #define M_A0_MA2Q_ST                            64u */

 #define M_A0_MA3Q_PP0                              4u
 #define M_A0_MA3Q_PP1                              5u
 /* #define M_A0_MA3Q_MD                            0u */
 #define M_A0_MA3Q_BD                               64u
 /* #define M_A0_MA3Q_ST                            64u */

 #define PFE_DDRQ_PP0                               4u
 #define PFE_DDRQ_PP1                               5u
 /* #define PFE_DDRQ_MD                             0u */
 #define PFE_DDRQ_BD                                256u
 /* #define PFE_DDRQ_ST                             64u */

 #define PFE_DDWQ_PP0                               4u
 #define PFE_DDWQ_PP1                               5u
 /* #define PFE_DDWQ_MD                             0u */
 #define PFE_DDWQ_BD                                256u
 /* #define PFE_DDWQ_ST                             64u */

 #define PFE_HF0Q_PP0                               1u
 #define PFE_HF0Q_PP1                               5u
 /* #define PFE_HF0Q_MD                             0u */
 #define PFE_HF0Q_BD                                128u
 /* #define PFE_HF0Q_ST                             64u */

 #define PFE_HF1Q_PP0                               1u
 #define PFE_HF1Q_PP1                               5u
 /* #define PFE_HF1Q_MD                             0u */
 #define PFE_HF1Q_BD                                128u
 /* #define PFE_HF1Q_ST                             64u */

 #define PFE_HF2Q_PP0                               1u
 #define PFE_HF2Q_PP1                               5u
 /* #define PFE_HF2Q_MD                             0u */
 #define PFE_HF2Q_BD                                21u
 /* #define PFE_HF2Q_ST                             64u */

 #define PFE_HF3Q_PP0                               1u
 #define PFE_HF3Q_PP1                               5u
 /* #define PFE_HF3Q_MD                             0u */
 #define PFE_HF3Q_BD                                21u
 /* #define PFE_HF3Q_ST                             64u */

 #define PFE_UTLQ_PP0                               1u
 #define PFE_UTLQ_PP1                               5u
 #define PFE_UTLQ_MD                                0u
 #define PFE_UTLQ_BD                                42u
 #define PFE_UTLQ_ST                                64u

 #define UB_MNQ_PP0                                 5u
 #define UB_MNQ_PP1                                 5u
 #define UB_MNQ_MD                                  0u

/*===================================================================================================*/
/*                                       FUNCTION PROTOTYPES                                         */
/*===================================================================================================*/

/*
 * @brief      Initialise FlexNoc registers specific to M7 or A53 core
*/
void FlexNOC_InitNoc(uint8 core);

#ifdef __cplusplus
}
#endif

#endif