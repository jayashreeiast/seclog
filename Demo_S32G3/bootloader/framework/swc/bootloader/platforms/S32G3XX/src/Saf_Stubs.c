/*================================================================================================*/
/*
*   @file              Saf_Stubs.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @version           24.12.0
*
*   @brief             Bootloader safety related stubs definitions.
*   @details           Contains definitions of the stub functions and variables needed by
*                      the Safety context.
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "Saf_Stubs.h"

#if (STD_ON == BL_USE_SAFETY)

#include "eMcem_Fccu_Types.h"

/**
 * @brief Custom structure to track the installed sCheck handlers.
 */
sCheck_FwdType sCheck_Fwd[SCHECK_CORE_NUM];

/*mSel_Specific*/
uint8 u8NcfInjectedFault[NCF_INJECTED_FAULTS_ARRAY_SIZE];
/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

extern uint8 Sys_GetCoreID(void);

/**
@brief      Callback function.
@details    BISTs complete notifications.
*/
void Bist_NotificationFunction(void)
{
    /* Not implemented */
}

/**
@brief      Wrapper function.
@details    Function used to get core Id.
*/
uint32 sys_getCoreId(void)
{
    /* Not implemented */
    return Sys_GetCoreID();
}

/**
@brief      Alarm Handler function prototypes
@details
*/
extern eMcem_ErrRecoveryType eMcemUserAlarmHandler( eMcem_FaultType nFaultId )
{
    (void)nFaultId;
    return EMCEM_ERR_NOT_RECOVERED;
}

/**
@brief      Wrapper function to access PMIC driver
@details    Function to access PMIC driver in order to run the safety tests

@param[in]  eDestination  Specifies PMIC interface to communicate with
@param[in]  u32RegAddr    Specifies PMIC register to read data from
@param[in]  pu16ValueOut  Output variable to store the register value

@param[out] E_OK if success
            E_NOT_OK if error
*/
Std_ReturnType sBase_Pmic_ReadRegister(const sBase_Pmic_InterfaceDstType eDestination, const uint32 u32RegAddr, uint16 * pu16ValueOut)
{
    /*Not implemented*/
    (void) eDestination;
    (void) u32RegAddr;
    (void) pu16ValueOut;

    Std_ReturnType result = E_OK;
    return result;
}

/**
@brief      Stub for Normal operational mode initialization.
@details    Function for Normal operational mode initialization.

@param[out] E_OK if success
            E_NOT_OK if error
*/
Std_ReturnType mSel_UserInitNormal(void)
{
    /* Following NCF faults are set altogether with pre-configured mSel plugin to invoke transition into Degraded operational mode. */
    /* Set NCF faults to test transition into Degraded mode */
    u8NcfInjectedFault[0] = (uint8)EMCEM_FCCU_M7_1_RCCU_LS_ERROR;                 /* Required for Normal Mode, Threshold=2 */
    u8NcfInjectedFault[1] = (uint8)EMCEM_FCCU_M7_1_SRAM_ECC_UE;                   /* Required for Normal Mode, Threshold=2 */
    u8NcfInjectedFault[2] = (uint8)EMCEM_FCCU_CM7_1_LOCKUP;                       /* Required for Normal Mode, Threshold=2 */
    u8NcfInjectedFault[3] = (uint8)EMCEM_FCCU_FLEXNOC_MISSION_FAULT_0;            /* Required for Normal Mode, Threshold=2 */
    u8NcfInjectedFault[4] = (uint8)EMCEM_FCCU_A53_0_CORE1_CORE3_MERR_PMUEVENT;    /* Required for Normal Mode, Threshold=4 */
    u8NcfInjectedFault[5] = (uint8)EMCEM_FCCU_NOC_ERR_PROBE_FAULT;                /* Required for Normal Mode, Threshold=4 */

    return (Std_ReturnType)E_OK;
}

/**
@brief      Stub for Degraded operational mode initialization function.
@details    Function for Degraded operational mode initialization.

@param[out] E_OK if success
            E_NOT_OK if error
*/
Std_ReturnType mSel_UserInitDegraded_0(void)
{
/*Implementation of Degraded mode initialization function. */

    /* Following NCF faults are set for demo purpose altogether with pre-configured mSel plugin to invoke transition into "Stay in reset" state. */
    /* Set NCF faults to test triggering Software Recovery for case No mode can be selected. After several resets (set by register MC_RGM.FRET -> MC_RGM.DRET) will force "Stay in reset" state.*/
    /* Required for Degraded Mode, Threshold=4 */
    u8NcfInjectedFault[0] = (uint8)EMCEM_FCCU_CM7_0_LOCKUP;
    /* Required for Degraded Mode, Threshold=4 */
    u8NcfInjectedFault[1] = (uint8)EMCEM_FCCU_CMU_SAFETY_CORES_ERROR;
    /* Required for Degraded Mode, Threshold=4 */
    u8NcfInjectedFault[2] = (uint8)EMCEM_FCCU_CMU_PERFORMANCE_CORES_ERROR;

    return (Std_ReturnType)E_OK;
}

/**
@brief      Stub for low level NVM read function.
@details    Function for low level NVM read function.

@param[out] E_OK if success
            E_NOT_OK if error
*/
#if (STD_ON == MSEL_NVM_STORAGE)
Std_ReturnType mSel_NvmRead(uint8 *data, const uint32 size, const mSel_NvmDataItemType Selection)
{
    /* Not implemented */

    (void) data;
    (void) size;
    (void) Selection;

    return E_OK;
}

/**
@brief      Stub for low level NVM write function.
@details    Function for low level NVM write function.

@param[out] E_OK if success
            E_NOT_OK if error
*/
Std_ReturnType mSel_NvmWrite(const uint8 *data, const uint32 size, const mSel_NvmDataItemType Selection)
{
    /* Not implemented */

    (void) data;
    (void) size;
    (void) Selection;

    return E_OK;
}
#endif

/**
@brief      This function installs sCheck specific ISR.
@details    The function shall install sCheck interrupt service routine (ISR) to the user's ISR
            in order to handle interrupts generated by sCheck tests. The sCheck ISR shall be installed to
            the system's ISR of the current Exception Level (EL). Not implemented yet.

@param[in]  InterruptID   Specifies GIC INTID that the sCheck ISR shall be associated with.
@param[in]  InterruptHandler  Pointer to the sCheck ISR.

*/
void sCheck_InstallTestIsr( uint32 InterruptID, sCheck_InterruptHandlerType InterruptHandler)
{
    /* Not implemented */

    (void) InterruptID;
    (void) InterruptHandler;
}

/**
@brief      This function uninstalls sCheck specific ISR.
@details    function shall disable forwarding of the specified interrupt to the previously installed
            sCheck ISR at the current Exception Level (EL)..

@param[in]  InterruptID   Specifies GIC INTID that the sCheck ISR shall be associated with.

*/
void sCheck_UninstallTestIsr( uint32 InterruptID )
{
    /* Not implemented */

    (void) InterruptID;
}

/**
@brief      This function installs sCheck specific exception handler.
@details    The function shall install sCheck exception handler to the user's exception service routine
            in order to handle exceptions generated by sCheck tests. The handler shall be installed to
            the exception service routine of the current Exception Level (EL).

@param[in]  ExceptionType     Specifies system exception that the handlers shall be associated with.
@param[in]  ExceptionHandler  Pointer to the sCheck exception handler.

*/
void sCheck_InstallTestEsr( sCheck_ExceptionType ExceptionType, sCheck_ExceptionHandlerType ExceptionHandler )
{
    /* Not implemented */

    (void) ExceptionType;
    (void) ExceptionHandler;
}

/**
@brief      This function uninstalls sCheck specific exception handler.
@details    The function shall uninstall sCheck exception handler to the user's exception service routine
            in order to handle exceptions generated by sCheck tests.

@param[in]  ExceptionType     Specifies system exception that the handlers shall be associated with.
*/
void sCheck_UninstallTestEsr( sCheck_ExceptionType ExceptionType )
{
    /* Not implemented */

    (void) ExceptionType;
}

/**
@brief      This function triggers an SquareCheck event on the specified core
@details    This function triggers an SquareCheck event on the specified core.

@param[in]  dstCoreId   Destination Core ID
*/
Std_ReturnType sCheck_SendEvent(sCheck_CoreIdType dstCoreId)
{
    /* Not implemented */

    (void) dstCoreId;
}

/**
 * @brief   Initialization of the array with NCF indexes injected by example demo code.
 */
void NcfInjectionInit(void)
{
    uint8 u8i;

    /* Clear Injected Fault buffer */
    for(u8i=0; u8i<NCF_INJECTED_FAULTS_ARRAY_SIZE; u8i++)
    {
        u8NcfInjectedFault[u8i] = 0xFF; /* NCF Array initialization to invalid NCF values */
    }
}

/**
@brief      Execute NCF injection based on previously set NCF array.
@details    Please be aware if some NCF channel, being injected to, has configured reset reaction,
            it is executed immediately so rest of faults won't be injected.

@param[out] E_OK if success
            E_NOT_OK if error
*/
Std_ReturnType NcfInjectionExecution(void)
{
    uint8 u8i;
    Std_ReturnType ret = E_OK;

    for(u8i = 0; u8i < NCF_INJECTED_FAULTS_ARRAY_SIZE; u8i++)
    {
        /* Inject only valid NCF values */
        if( 0xFF != u8NcfInjectedFault[u8i] )
        {
            ret |= eMcem_InjectFault(u8NcfInjectedFault[u8i]);
        }
    }
    return ret;
}

#endif /* STD_ON == BL_USE_SAFETY */

#ifdef __cplusplus
}
#endif

/** @} */
