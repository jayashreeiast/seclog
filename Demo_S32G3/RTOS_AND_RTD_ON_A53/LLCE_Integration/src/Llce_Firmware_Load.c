/*==================================================================================================
*
* Copyright 2018-2025 NXP  
*
* NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms.
* By expressly accepting such terms or by downloading, installing, activating and/or otherwise using
* the software, you are agreeing that you have read, and that you agree to comply with and are bound
* by, such license terms.  If you do not agree to be bound by the applicable license terms, then you
* may not retain, install, activate or otherwise use the software.
==================================================================================================*/

#include "Llce_Firmware_Load.h"
#include "Llce_InterfaceFwMgr.h"
#include "Llce_RegAccess.h"


typedef struct {
    uint8* const pu8MemLocation;
    const uint32 u32RAMSize;
    const uint8* const pu8CodeData;
    const uint32* u32CodeSize;
} Llce_CoreProgramType;


#define LLCE_NUM_CORES              4U
#define LLCE_ENABLE_CORE_ADDR       ((uint32)0x43FF8000U)
#define LLCE_DATA_RAM_START_ADDR    0x43800000
#define LLCE_DATA_RAM_LENGTH        0x50000
#define LLCE_FW_STARTUP_DURATION    1000000

#if defined(S32G3) || defined(S32G3XX)
    #define DTE_RAM_SIZE        0x4000
    #define PPE_RX_RAM_SIZE     0x18000
    #define PPE_TX_RAM_SIZE     0x8000
    #define FRPE_RAM_SIZE       0x40000
#else /* S32G2 */
    #define DTE_RAM_SIZE        0x2000
    #define PPE_RX_RAM_SIZE     0x8000
    #define PPE_TX_RAM_SIZE     0x8000
    #define FRPE_RAM_SIZE       0x10000
#endif

/* This variable contains information about fw boot end */
__attribute__((section(".llce_boot_end"))) volatile Llce_Mgr_StatusType Llce_Mgr_Status;

extern const uint8 dte_bin[];
extern const uint32 dte_bin_len;

extern const uint8 ppe_rx_bin[];
extern const uint32 ppe_rx_bin_len;

extern const uint8 ppe_tx_bin[];
extern const uint32 ppe_tx_bin_len;

extern const uint8 frpe_bin[];
extern const uint32 frpe_bin_len;

#ifdef LLCE_USE_HEADLESS
extern const uint8 LLCE_HeadlessConfig_bin[];
#endif

static const Llce_CoreProgramType Llce_CoreData[LLCE_NUM_CORES] = 
{
    { (uint8*)0x43000000, DTE_RAM_SIZE,  dte_bin,    &dte_bin_len },
    { (uint8*)0x43100000, PPE_RX_RAM_SIZE, ppe_rx_bin, &ppe_rx_bin_len},
    { (uint8*)0x43200000, PPE_TX_RAM_SIZE, ppe_tx_bin, &ppe_tx_bin_len},
    { (uint8*)0x43300000, FRPE_RAM_SIZE, frpe_bin,   &frpe_bin_len }
};

/**
* @brief          Service routine used to load the firmware from host memory to LLCE internal memory and enable LLCE cores in order to start running.
* @details        This routine is called by:
*                  - Can_Llce_Init
*
* @param[in]      void
*
* @return
* @retval        E_OK  Timeout was not reached. LLCE internal cores started correctly
* @retval        E_NOT_OK  Timeout was reached. LLCE internal cores not started correctly
*/
Std_ReturnType Llce_Firmware_Load(void)
{   
    uint8 i;
    uint32 crtOffset;
    uint32 byteIndex;
    uint64 *u64MemPointer;
    uint64 *u64MemPointer2;
    const uint64 *u64DataPointer;
	
    /* Put all cores in reset*/
    Reg_Write32(LLCE_ENABLE_CORE_ADDR, 0x0);

    for (i = 0U; i < LLCE_NUM_CORES; i++) 
    {
        /* Move code to core's code section*/
        /* Performance optimisation: write 8 bytes at a time */

        u64MemPointer  = (uint64*)Llce_CoreData[i].pu8MemLocation;
        u64DataPointer = (uint64*)Llce_CoreData[i].pu8CodeData;

        /* Offset is in words (32 bit) */
        for (crtOffset = 0U; crtOffset <  (*(Llce_CoreData[i].u32CodeSize)) / 8; crtOffset++)
        {
            u64MemPointer[crtOffset] = u64DataPointer[crtOffset];
        }
        
        /* Write 2 words at a time before copying bytes */
        u64MemPointer[crtOffset] = (uint64)0;

        /* Back to 1 byte mode */
        for (byteIndex = crtOffset * 8U; byteIndex <  (*(Llce_CoreData[i].u32CodeSize)); byteIndex++)
        {
            Llce_CoreData[i].pu8MemLocation[byteIndex] = Llce_CoreData[i].pu8CodeData[byteIndex];
        }

        crtOffset++;

        /* Fill the rest of the core's RAM in order to enable ECC RAM */
        for ( ; crtOffset < (Llce_CoreData[i].u32RAMSize) / 8 ; crtOffset++)
        {
            u64MemPointer[crtOffset] = (uint64)0;
        }
    }

    /* Pointer to the first half of LLCE Shared RAM */
    u64MemPointer  = (uint64*)LLCE_DATA_RAM_START_ADDR;
    /* Pointer to the second half of LLCE Shared RAM */
    u64MemPointer2  = (uint64*)(LLCE_DATA_RAM_START_ADDR + (LLCE_DATA_RAM_LENGTH/2));

    /* Write all the Shared Memory in order to enable ECC RAM: 8 bytes at a time */
    for (crtOffset = 0U; crtOffset <  LLCE_DATA_RAM_LENGTH / 16; crtOffset++)
    {
        u64MemPointer[crtOffset] = (uint64)0;
        u64MemPointer2[crtOffset] = (uint64)0;
    }

#ifdef LLCE_USE_HEADLESS
    /* Copy magic value from headless config */
    *((uint32*)LLCE_DATA_RAM_START_ADDR) = *((uint32*)LLCE_HeadlessConfig_bin);
    /* Set headless config address */
    *(((uint32*)LLCE_DATA_RAM_START_ADDR) + 1U) = (uint32)&LLCE_HeadlessConfig_bin;
#endif    

    /* Clear memory area where Llce notifies about boot ending */
    Llce_Mgr_Status.eTxBootStatus = LLCE_FW_NOTRUN;
    Llce_Mgr_Status.eRxBootStatus = LLCE_FW_NOTRUN;
    Llce_Mgr_Status.eDteBootStatus = LLCE_FW_NOTRUN;
    Llce_Mgr_Status.eFrpeBootStatus = LLCE_FW_NOTRUN;

    Reg_Write32(LLCE_ENABLE_CORE_ADDR, 0xF);      /*start all cores */

    return Llce_Firmware_Load_GetBootStatus();
}

boolean Llce_Firmware_Load_GetBootStatus(void)
{
    Std_ReturnType eReturnValue = (uint8)E_NOT_OK;
    uint32 u32Timeout = LLCE_FW_STARTUP_DURATION;
    Llce_Fw_ReturnType eTxBootStatus   = LLCE_FW_NOTRUN;
    Llce_Fw_ReturnType eRxBootStatus   = LLCE_FW_NOTRUN;
    Llce_Fw_ReturnType eDteBootStatus  = LLCE_FW_NOTRUN;
    Llce_Fw_ReturnType eFrpeBootStatus = LLCE_FW_NOTRUN; 

    /*Wait that the LLCE firmware cores to start running and boot.*/
    while (( (LLCE_FW_SUCCESS != eTxBootStatus) || (LLCE_FW_SUCCESS != eRxBootStatus) || \
			(LLCE_FW_SUCCESS != eDteBootStatus) || (LLCE_FW_SUCCESS != eFrpeBootStatus) ) && (u32Timeout > 0))
    {
        /* Volatile variables should be read separately */
        eTxBootStatus   = Llce_Mgr_Status.eTxBootStatus;
        eRxBootStatus   = Llce_Mgr_Status.eRxBootStatus;
        eDteBootStatus  = Llce_Mgr_Status.eDteBootStatus;
        eFrpeBootStatus = Llce_Mgr_Status.eFrpeBootStatus;
		u32Timeout--;
    }
    
    if (u32Timeout != 0)	
    {
            eReturnValue = E_OK;
    }

    return  eReturnValue;
}
