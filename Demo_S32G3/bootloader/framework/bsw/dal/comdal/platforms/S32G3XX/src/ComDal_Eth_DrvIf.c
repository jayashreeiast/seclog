/**
*   @file           ComDal_Eth_DrvIf.c
*   @version        24.12.0
*
*   @brief          ComDal Ethernet GMAC driver wrapper.
*   @details        Driver wrapper over the GMAC Ethernet driver.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*=================================================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
===================================================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*=================================================================================================================================
                               INCLUDE FILES
===================================================================================================================================*/

#ifdef IFWK_USE_ETH
#include "EthIf_Cbk.h"
#include "Std_Types.h"
#include "Mcu.h"

#include "ComDal_Cfg.h"
#include "ComDal_Types.h"

#include "gmac_driver.h"

/*=================================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
===================================================================================================================================*/
#define COMDAL_ETH_DRVIF_VENDOR_ID_C                       43

#define COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_ETH_DRVIF_SW_MAJOR_VERSION_C                24
#define COMDAL_ETH_DRVIF_SW_MINOR_VERSION_C                12
#define COMDAL_ETH_DRVIF_SW_PATCH_VERSION_C                0

/*=================================================================================================================================
*                              FILE VERSION CHECKS
===================================================================================================================================*/
#if (COMDAL_MAX_ETH_INTERFACES > 0U)
/* Check if this source file and the Comdal configuration header file are of the same vendor. */
#if (COMDAL_ETH_DRVIF_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Eth_DrvIf.c and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if this source file and the Comdal configuration header file are of the same Autosar version. */
#if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth_DrvIf.c and ComDal_Cfg.h are different"
#endif

/* Check if this source file and the Comdal configuration header file are of the same Software version. */
#if ((COMDAL_ETH_DRVIF_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth_DrvIf.c and ComDal_Cfg.h are different"
#endif

/* Check if this source file and the Comdal types header file are of the same vendor. */
#if (COMDAL_ETH_DRVIF_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Eth_DrvIf.c and ComDal_Types.h have different vendor ids"
#endif

/* Check if this source file and the Comdal types header file are of the same Autosar version. */
#if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth_DrvIf.c and ComDal_Types.h are different"
#endif

/* Check if this source file and the Comdal types header file are of the same Software version. */
#if ((COMDAL_ETH_DRVIF_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth_DrvIf.c and ComDal_Types.h are different"
#endif

/* Check if this source file and the Comdal ETH Adapter header file are from the same vendor. */
#if (COMDAL_ETH_DRVIF_VENDOR_ID_C != COMDAL_ETH_DRVIF_VENDOR_ID)
    #error "ComDal_Eth_DrvIf.c and EthIf_Cbk.h have different vendor ids"
#endif

/* Check if this source file and the Comdal ETH Adapter header file are of the same Autosar version. */
#if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION_C != COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION_C != COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION_C != COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth_DrvIf.c and EthIf_Cbk.h are different"
#endif

/* Check if this source file and the Comdal ETH Adapter header file are of the same Software version. */
#if ((COMDAL_ETH_DRVIF_SW_MAJOR_VERSION_C != COMDAL_ETH_DRVIF_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_MINOR_VERSION_C != COMDAL_ETH_DRVIF_SW_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_PATCH_VERSION_C != COMDAL_ETH_DRVIF_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of ComDal_Eth_DrvIf.c and EthIf_Cbk.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if this source file and the StandardTypes header file are of the same AutoSar version. */
    #if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION_C != STD_AR_RELEASE_MAJOR_VERSION) || \
        (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION_C != STD_AR_RELEASE_MINOR_VERSION) || \
        (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION_C != STD_AR_RELEASE_REVISION_VERSION) \
        )
        #error "AutoSar Version Numbers of ComDal_Eth_DrvIf.c and StandardTypes.h are different"
    #endif
#endif

/*=====================================================================================================================
*                                        DEFINES AND MACROS
======================================================================================================================*/
/* Maximum frame length, taking into account a maximum MTU of 1500 bytes. */
#define COMDAL_ETH_DRVIF_MAX_FRAME_LEN                       1518U

/* Maximum number of GMAC interfaces. */
#define COMDAL_ETH_DRVIF_MAX_INSTANCES                       1U 

/* The single PHY interface available is RGMII, as the other PHY cannot be used for Ethernet. */
#define COMDAL_ETH_DRVIF_MODE                                GMAC_RGMII_MODE 

/* The default Media Independent Interface speed. */
#define COMDAL_ETH_DRVIF_SPEED                               GMAC_SPEED_1G

/* Number of RX rings that can be used by the driver. */
#define COMDAL_ETH_DRVIF_RX_RINGS_COUNT                      2U

/* Number of TX rings that can be used by the driver. */
#define COMDAL_ETH_DRVIF_TX_RINGS_COUNT                      1U

/* Maximum number of RX buffers as part of the driver's RX ring descriptors list. */
#define COMDAL_ETH_DRVIF_RX_BUFFERS_COUNT                    16U

/* Maximum number of TX buffers as part of the driver's TX ring descriptors list. */
#define COMDAL_ETH_DRVIF_TX_BUFFERS_COUNT                    8U

/* The maximum number of retries before timing out when sending a frame. */
#define COMDAL_ETH_DRVIF_TX_RETRY_COUNT                      100000U

/* The buffer descriptors alignment boundary size. The internal DMA requires the transfer descriptors to be aligned.  */
#define COMDAL_ETH_DRVIF_BUFFER_DESCRIPTOR_ALIGNMENT_SIZE    64UL

/*=====================================================================================================================
*                                        LOCAL DRVIF TYPE DECLARATIONS
======================================================================================================================*/
/**
* @brief          Holds the low-level driver configuration for each network interface
* @details        The structure is used to match the each TCP/IP interface with a low-level driver config.
*/
struct ComDal_Eth_DrvIf_Configuration {
    /* The instance number used to match the configuration for the TCP/IP stack interface. */
    VAR(uint8, AUTOMATIC)              IfNum;

    /* The GMAC module configuration structure. */
    CONST(gmac_config_t, AUTOMATIC)                   DriverConfig;

    /* GMAC RX rings configuration. */
    CONST(gmac_buffer_config_t, AUTOMATIC)            aRxBuffersConfig[COMDAL_ETH_DRVIF_RX_RINGS_COUNT];

    /* GMAC TX rings configuration. */
    CONST(gmac_buffer_config_t, AUTOMATIC)            aTxBuffersConfig[COMDAL_ETH_DRVIF_TX_RINGS_COUNT];

    /* The GMAC structure holding the assigned callbacks. */
    P2VAR(gmac_state_t, AUTOMATIC, ETH_APPL_DATA)     pGmacState;
};

/*=====================================================================================================================
*                                        LOCAL VARIABLES
======================================================================================================================*/
/* The RX rings descriptors table for the maximum number of GMAC instances. */
__attribute__((aligned(COMDAL_ETH_DRVIF_BUFFER_DESCRIPTOR_ALIGNMENT_SIZE)))
static VAR(gmac_buffer_descriptor_t, AUTOMATIC)                ComDal_Eth_DrvIf_aRxDescriptors[COMDAL_ETH_DRVIF_MAX_INSTANCES]
                                                               [COMDAL_ETH_DRVIF_RX_RINGS_COUNT]
                                                               [COMDAL_ETH_DRVIF_RX_BUFFERS_COUNT];

/* The TX rings descriptors table for the maximum number of GMAC instances. */
__attribute__((aligned(COMDAL_ETH_DRVIF_BUFFER_DESCRIPTOR_ALIGNMENT_SIZE)))
static VAR(gmac_buffer_descriptor_t, AUTOMATIC)                ComDal_Eth_DrvIf_aTxDescriptors[COMDAL_ETH_DRVIF_MAX_INSTANCES]
                                                               [COMDAL_ETH_DRVIF_TX_RINGS_COUNT]
                                                               [COMDAL_ETH_DRVIF_TX_BUFFERS_COUNT];

/* The preliminary RX rings buffers table for the maximum number of GMAC instances. */
__attribute__((aligned(COMDAL_ETH_DRVIF_BUFFER_DESCRIPTOR_ALIGNMENT_SIZE)))
static VAR(uint8_t, AUTOMATIC)                                 ComDal_Eth_DrvIf_aRxBuffer[COMDAL_ETH_DRVIF_MAX_INSTANCES]
                                                               [COMDAL_ETH_DRVIF_RX_RINGS_COUNT]
                                                               [COMDAL_ETH_DRVIF_RX_BUFFERS_COUNT * COMDAL_ETH_DRVIF_MAX_FRAME_LEN];

/* The GMAC state structures for the maximum number of GMAC instances. */
static VAR(gmac_state_t, AUTOMATIC)                            ComDal_Eth_DrvIf_GmacState[COMDAL_ETH_DRVIF_MAX_INSTANCES];

/* Configuration table holding the per-interface GMAC config. */
static CONST(struct ComDal_Eth_DrvIf_Configuration, AUTOMATIC) ComDal_Eth_DrvIf_aConfigs[COMDAL_ETH_DRVIF_MAX_INSTANCES] = {
    {
        0U,                                                              /* The instance number */
        {
            .interrupts = 0U,                                            /* Per-instance interrupts are disabled */
            .maxFrameLen = COMDAL_ETH_DRVIF_MAX_FRAME_LEN,               /* The maximum frame length (in bytes) */
            .miiMode = COMDAL_ETH_DRVIF_MODE,                            /* MII mode */
            .speed = COMDAL_ETH_DRVIF_SPEED,                             /* MII Speed selection */
            .duplex = GMAC_FULL_DUPLEX,                                  /* Duplex mode selection */
            .macConfig = (uint32_t) GMAC_MAC_CONFIG_SA_REPLACEMENT | 
                         (uint32_t) GMAC_MAC_CONFIG_CHECKSUM_OFFLOAD,    /* MAC transmission configuration */
            .macPktFilterConfig = (uint32_t) GMAC_PKT_FILTER_RECV_ALL,   /* MAC packet filterting mode */
            .rxRingCount = COMDAL_ETH_DRVIF_RX_RINGS_COUNT,              /* The number of RX rings */
            .txRingCount = COMDAL_ETH_DRVIF_TX_RINGS_COUNT,              /* The number of TX rings */
            .callback = NULL                                             /* Callbacks are disabled in polling mode */
        },
        {
            {
                .ringSize = COMDAL_ETH_DRVIF_RX_BUFFERS_COUNT,           /* The number of buffer descriptors for this RX ring */
                .ringDesc = &(ComDal_Eth_DrvIf_aRxDescriptors[0][0][0]), /* RX ring start address for this interface */
                .callback = NULL,                                        /* Callbacks are disabled in polling mode */
                .interrupts = 0U,                                        /* Per-channel interrupts are disabled */
                .rxBuffer = &(ComDal_Eth_DrvIf_aRxBuffer[0][0][0])       /* Start address of the receive buffer (separate from the descriptors) */
            },
            {
                .ringSize = COMDAL_ETH_DRVIF_RX_BUFFERS_COUNT,           /* The number of buffer descriptors for this RX ring */
                .ringDesc = &(ComDal_Eth_DrvIf_aRxDescriptors[0][1][0]), /* RX ring start address for this interface */
                .callback = NULL,                                        /* Callbacks are disabled in polling mode */
                .interrupts = 0U,                                        /* Per-channel interrupts are disabled */
                .rxBuffer = &(ComDal_Eth_DrvIf_aRxBuffer[0][1][0])       /* Start address of the receive buffer (separate from the descriptors) */
            }
        },
        {
            {
                .ringSize = COMDAL_ETH_DRVIF_TX_BUFFERS_COUNT,           /* The number of buffer descriptors for this TX ring */
                .ringDesc = &(ComDal_Eth_DrvIf_aTxDescriptors[0][0][0]), /* TX ring start address for this interface */
                .callback = NULL,                                        /* Callbacks are disabled in polling mode */
                .interrupts = 0U,                                        /* Per-channel interrupts are disabled */
                .rxBuffer = NULL                                         /* No receive buffer for the TX ring */
            }
        },
        &ComDal_Eth_DrvIf_GmacState[0]                                   /* The GMAC state structure */
    }
};

/*=====================================================================================================================
*                                        LOCAL FUNCTION PROTOTYPES
======================================================================================================================*/
static FUNC(P2CONST(struct ComDal_Eth_DrvIf_Configuration, AUTOMATIC, ETH_APPL_DATA), AUTOMATIC)
       ComDal_Eth_DrvIf_GetConfigByIfNum(VAR(uint8, AUTOMATIC));

/*================================================================================================*/
/**
* @brief       Used for initializing the GMAC RX clock source after the pins were initialised
* @details     Call the Mcu_InitClock with the GMAC RX clock config settings
*
* @return      None
*/
/*================================================================================================*/
static FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_EnableGmacRxClock(void)
{
    Mcu_InitClock(McuClockSettingConfig_EnableGmacRxClock);
}
/*====================================================================================================================
*                                        GLOBAL FUNCTIONS
=====================================================================================================================*/
/*=================================================================================================================================*/
/**
* @brief      Calls the driver initialization function
* @details    This function passes the driver configuration parameters, the TX/RX buffer configs 
*             and the hardware address to the driver init function.
*             If there is no configuration available, ERR_VAL is returned.
*             If the driver was successfully initialized, ERR_OK is returned.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pHwAddress                  Pointer to the hardware address of the interface
* 
* @return     The error status: ERR_VAL, ERR_OK 
*/
/*=================================================================================================================================*/
FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_DrvIf_Init(VAR(uint8, AUTOMATIC) IfNum,
                                                                  P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pHwAddr)
{
    P2CONST(struct ComDal_Eth_DrvIf_Configuration, AUTOMATIC, ETH_APPL_DATA) pGmacConfig = NULL;
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status                                    = ERR_OK;

    pGmacConfig = ComDal_Eth_DrvIf_GetConfigByIfNum(IfNum);

    if (NULL == pGmacConfig)
    {
       Status = ERR_VAL; 
    }

    /* Workaround for enabling the GMAC RX clock after the port init was made. */
    ComDal_Eth_DrvIf_EnableGmacRxClock();

    /* Initialize the GMAC driver. */
    GMAC_DRV_Init(IfNum, pGmacConfig->pGmacState, &pGmacConfig->DriverConfig,
                  pGmacConfig->aRxBuffersConfig, pGmacConfig->aTxBuffersConfig,
                  pHwAddr);

    /* Enable multicast traffic filtering for the current interface. */
    GMAC_DRV_SetMulticastHashFilter(IfNum, TRUE);

    return Status;
}

/*=================================================================================================================================*/
/**
* @brief      Calls the driver deinit function for a given interface
* @details    This function calls the deinit function, passing the instance number only.
* 
* @param[in]  IfNum                       The interface number
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_DeInit(VAR(uint8, AUTOMATIC) IfNum)
{
    GMAC_DRV_Deinit(IfNum);
}

/*=================================================================================================================================*/
/**
* @brief      Try to read an Ethernet frame and pass the received data up the stack
* @details    If there is any new frame in the driver's RX rings, then call the read callback.
*             If the queue is empty, then do nothing.
* 
* @param[in]  IfNum                       The interface number
* @param[out] pfReadFrameCallback         The read callback
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_ReadFrame(VAR(uint8, AUTOMATIC) IfNum,
                                                   ComDal_Eth_ReadFrameCallbackType pfReadFrameCallback)
{
    VAR(gmac_buffer_t, AUTOMATIC) BufferDescriptor;
    VAR(status_t, AUTOMATIC) eStatus;
    VAR(uint8_t, AUTOMATIC) u8Index = 0;

    /* Iterate through each RX ring to see if there is any new frame to be read. */
    for (u8Index = 0; u8Index < COMDAL_ETH_DRVIF_RX_RINGS_COUNT; u8Index++)
    {
        /* Calling the ReadFrame function with NULL for the info argument shows we don't treat the payload details here. */
        eStatus = GMAC_DRV_ReadFrame(IfNum, u8Index, &BufferDescriptor, NULL);

        if (STATUS_SUCCESS == eStatus)
        {
            pfReadFrameCallback(IfNum, BufferDescriptor.data, BufferDescriptor.length);

            /* Free up the receive buffer in the current RX ring after it got processed. */
            GMAC_DRV_ProvideRxBuff(IfNum, u8Index, &BufferDescriptor);
        }
    }
}

/*=================================================================================================================================*/
/**
* @brief      Try to send an Ethernet frame having the given payload
* @details    Fill the TX buffer descriptor and then transmit the frame.
*             If the driver times out when sending the frame, ERR_TIMEOUT is returned.
*             If the the payload is illegal, ERR_VAL is returned.
*             If the frame was sent successfully, ERR_OK is returned.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pPayload                    Pointer to the frame payload
* @param[in]  PayloadLen                  Payload length
* 
* @return     The error status: ERR_TIMEOUT, ERR_VAL, ERR_OK
*/
/*=================================================================================================================================*/
FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_DrvIf_WriteFrame(VAR(uint8, AUTOMATIC) IfNum,
                                                                        P2VAR(void, AUTOMATIC, ETH_APPL_DATA) pPayload,
                                                                        VAR(uint16, AUTOMATIC) PayloadLen)
{
    VAR(gmac_buffer_t, AUTOMATIC) BufferDescriptor;
    VAR(gmac_tx_options_t, AUTOMATIC) TxOptions;
    VAR(status_t, AUTOMATIC) eDriverStatus;
    VAR(uint8_t, AUTOMATIC) u8Index = 0;
    VAR(uint32_t, AUTOMATIC) RetryCount = 0;
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_OK;
    
    if (NULL == pPayload || 0 == PayloadLen)
    {
        Status = ERR_VAL;
    }

    BufferDescriptor.data = (uint8_t *) pPayload;
    BufferDescriptor.length = PayloadLen;

    TxOptions.noInt = true;
    /* Enable FCS and protocol-specific checksum insertion by the driver. */
    TxOptions.crcPadIns = GMAC_CRC_AND_PAD_INSERTION;
    TxOptions.checksumIns = GMAC_CHECKSUM_INSERTION_PROTO_PSEUDOH;

    /* Cycle through the TX rings until we find a free TX descriptor in the list. */
    for (u8Index = 0; STATUS_ENET_TX_QUEUE_FULL == (eDriverStatus = GMAC_DRV_SendFrame(IfNum, u8Index, &BufferDescriptor, &TxOptions));)
    {
        if (COMDAL_ETH_DRVIF_TX_RINGS_COUNT == u8Index++) 
        {
            u8Index = 0;
        }
    }

    /* Once the TX buffer was filled, we need to wait until the frame was transmitted. */
    if (STATUS_SUCCESS == eDriverStatus)
    {
       while (STATUS_BUSY == GMAC_DRV_GetTransmitStatus(IfNum, u8Index, &BufferDescriptor, NULL))
       {
           if (COMDAL_ETH_DRVIF_TX_RETRY_COUNT <= ++RetryCount)
           {
               Status = ERR_TIMEOUT;
               break;
           }
       }
    }
    
    return Status;
}

/*=================================================================================================================================*/
/**
* @brief      Start forwarding the packets for a multicast group
* @details    The multicast MAC group is added to the hash filter table.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pHwAddr                     Pointer to the hardware address to be added
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_AddToHashFilter(VAR(uint8, AUTOMATIC) IfNum,
                                                       P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pHwAddr)
{
    GMAC_DRV_AddDstAddrToHashFilter(IfNum, pHwAddr);
}

/*=================================================================================================================================*/
/**
* @brief      Stop forwarding the packets for a multicast group
* @details    The multicast MAC group is removed from the hash filter table.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pHwAddr                     Pointer to the hardware address to be removed
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_RemoveFromHashFilter(VAR(uint8, AUTOMATIC) IfNum,
                                                            P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pHwAddr)
{
    GMAC_DRV_RemoveDstAddrFromHashFilter(IfNum, pHwAddr);
}

/*=================================================================================================================================*/
/**
* @brief      Enable ARP requests recognition and automatic ARP replying
* @details    The IP address is registered in the ARP table to send ARP replies automatically.
*             The MAC will recognize an ARP packet and schedule the response for it, also passing it up the stack.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pTargetAddr                 Pointer to the IP address for which to match the MAC address
* @param[in]  bEnable                     Enable flag
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_SetArpOffloading(VAR(uint8, AUTOMATIC) IfNum,
                                                        P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pTargetAddr,
                                                        VAR(boolean, AUTOMATIC) bEnable)
{
    GMAC_DRV_SetArpOffloading(IfNum, pTargetAddr, bEnable);
}

/*====================================================================================================================*/

/*=====================================================================================================================
*                                       LOCAL FUNCTIONS
======================================================================================================================*/
/*====================================================================================================================*/
/**
* @brief      Look up a GMAC config after the interface number
* @details    Use the interface number to find the corresponding GMAC configuration in the config array.
* 
* @param[in]  IfNum                       The interface number
* 
* @return     Pointer to the GMAC configuration structure, if found. Otherwise, NULL.
*/
/*===================================================================================================================*/
static FUNC(P2CONST(struct ComDal_Eth_DrvIf_Configuration, AUTOMATIC, ETH_APPL_DATA), AUTOMATIC) 
       ComDal_Eth_DrvIf_GetConfigByIfNum(VAR(uint8, AUTOMATIC) IfNum)
{
    VAR(uint8_t, AUTOMATIC) u8Index                                                     = 0;
    P2CONST(struct ComDal_Eth_DrvIf_Configuration, AUTOMATIC, ETH_APPL_DATA) pGmacConfig = NULL;
    
    for (u8Index = 0; u8Index < COMDAL_ETH_DRVIF_MAX_INSTANCES; u8Index++)
    {
        if (ComDal_Eth_DrvIf_aConfigs[u8Index].IfNum == IfNum)
        {
            pGmacConfig = &ComDal_Eth_DrvIf_aConfigs[u8Index];
            break;
        }
    }

    return pGmacConfig;
}
/*====================================================================================================================*/

#endif /* (COMDAL_MAX_ETH_INTERFACES > 0U) */

#endif /* IFWK_USE_ETH */

#ifdef __cplusplus
}
#endif /* __cplusplus */
/** @} */
