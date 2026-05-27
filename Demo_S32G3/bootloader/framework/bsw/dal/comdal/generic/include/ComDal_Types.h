/**
*   @file        ComDal_Types.h
*   @version     24.12.0
*
*   @brief       AUTOSAR - Integration Framework - Types file for IO Driver Abstraction Layer
*   @details     This file implements the COMDAL Types
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*=========================================================================================================================
*   Project              : AUTOSAR 4.4 GOLDVIP
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/
#ifndef COMDAL_TYPES_H
#define COMDAL_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "ComDal_Drv_Cfg.h"

/*==================================================================================================
                                       SOURCE FILE VERSION INFORMATION
====================================================================================================*/
#define COMDAL_TYPES_VENDOR_ID                           43
#define COMDAL_TYPES_MODULE_ID                           100

#define COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_TYPES_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_TYPES_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_TYPES_SW_MAJOR_VERSION                    24
#define COMDAL_TYPES_SW_MINOR_VERSION                    12
#define COMDAL_TYPES_SW_PATCH_VERSION                    0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if types header file and COMDAL driver configuration header file are of the same vendor */
#if (COMDAL_TYPES_VENDOR_ID != COMDAL_DRV_CFG_VENDOR_ID)
    #error "ComDal_Types.h and ComDal_Drv_Cfg.h have different vendor ids"
#endif

/* Check if types header file and COMDAL driver configuration header file are of the same Autosar version */
#if ((COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION != COMDAL_DRV_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_TYPES_AR_RELEASE_MINOR_VERSION != COMDAL_DRV_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_TYPES_AR_RELEASE_REVISION_VERSION != COMDAL_DRV_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Types.h and ComDal_Drv_Cfg.h are different"
#endif

/* Check if header file and COMDAL driver configuration header file are of the same software version */
#if ((COMDAL_TYPES_SW_MAJOR_VERSION != COMDAL_DRV_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_TYPES_SW_MINOR_VERSION != COMDAL_DRV_CFG_SW_MINOR_VERSION) || \
     (COMDAL_TYPES_SW_PATCH_VERSION != COMDAL_DRV_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Types.h and ComDal_Drv_Cfg.h are different"
#endif

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

typedef enum
{
    COMDAL_CAN_CHANNEL  = 0U,    /**< @brief COMDAL channel is of type CAN. */
    COMDAL_LIN_CHANNEL,          /**< @brief COMDAL channel is of type LIN.*/
    COMDAL_UART_CHANNEL,         /**< @brief COMDAL channel is of type UART. */
    COMDAL_NFC_CHANNEL,          /**< @brief COMDAL channel is of type NFC. */
    COMDAL_IPC_CHANNEL,          /**< @brief COMDAL channel is of type IPC. */
    COMDAL_LLCE_CHANNEL,         /**< @brief COMDAL channel is of type LLCE. */
    COMDAL_ETH_CHANNEL,          /**< @brief COMDAL channel is of type ETH. */
    COMDAL_CHANNEL_INVALID       /**< @brief COMDAL channel is not valid. */
} ComDal_BswChannelType;

typedef enum
{
    COMDAL_RXCHANNEL  = 0U,      /**< @brief RX channel used for receiving data */
    COMDAL_TXCHANNEL             /**< @brief TX channel used for transmiting data */
} ComDal_AccessType;

typedef enum
{
    COMDAL_NO_ERROR  = 0U,      /**< @brief channel had detected NO ERROR. */
    COMDAL_OBSOLETE,            /**< @brief channel contains OBSOLETE data (RX data was not update since last read).*/
    COMDAL_INVALID,             /**< @brief channel state is INVALID (COM channel was not properly initialized).*/
    COMDAL_BUSY,                /**< @brief channel is BUSY.*/
    COMDAL_OVERFLOW             /**< @brief channel overflow error.*/
} ComDal_ReturnType;

typedef enum
{
    COMDAL_STATE_IDLE  = 0U,    /**< @brief channel is not in use and the transfer can start */
    COMDAL_STATE_RX_ONGOING,    /**< @brief channel is used and there is a receive in progress */
    COMDAL_STATE_TX_ONGOING,    /**< @brief channel is used and there is a transmit in progress */
    COMDAL_STATE_ERROR,         /**< @brief channel encountered an error during transfer */
} ComDal_State;

typedef enum
{
    COMDAL_IPV4      = 0U,    /**< @brief IP version 4 format */
    COMDAL_IPV6      = 6u,    /**< @brief IP version 6 format */
    COMDAL_IPV4_IPV6 = 46,    /**< @brief channel is used and there is a transmit in progress */
} ComDal_IpTypes;

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
* @brief          ComDal Basic Software channel Id type
* @details        Type used for Bsw ComDal channels;
*/
typedef uint16                                                ComDal_BswDescriptorIdType;

/**
* @brief          ComDal UART Buffer type
* @details        Type used to define UART data buffers;
*/
typedef char                                                 ComDal_UartBufferType;

/**
* @brief          ComDal Basic Software channel Id type
* @details
*/
typedef uint16                                               ComDal_ChannelType;

/**
* @brief          ComDal notification
* @details        ComDal notification function for upper layers
*/
typedef void (*ComDal_CalloutType)(int);

/**
* @brief          ComDal Ring Buffer
* @details        ComDal Ring Buffer
*/
typedef struct
{
    /**< @brief Buffer to store data */
    P2VAR(char, AUTOMATIC,  COMDAL_APPL_CONST)                  pBuffer;

    /**< @brief Maximum size */
    VAR(uint16, AUTOMATIC)                                      u16Size;

    /**< @brief Number of elements in buffer */
    VAR(uint16, AUTOMATIC)                                      u16ElementsCount;

    /**< @brief Points to the last element added */
    VAR(uint16, AUTOMATIC)                                      u16HeadIndex;

    /**< @brief Points to the current element */
    VAR(uint16, AUTOMATIC)                                      u16TailIndex;
} ComDal_RingBuffer;

#ifdef IFWK_USE_CAN
/**
* @brief          Structure to configure CAN channel descriptors
* @details        Data structure containing the set of configuration parameters required
*                 for setting up CAN channels.
*/
typedef struct
{
  /**< @brief Bsw channel id*/
  VAR(uint8, AUTOMATIC)                                      ComChannelId;

  /**< @brief Buffer to store CAN Data for TX or RX data depending on channed direction*/
  P2VAR(char, AUTOMATIC, FW_APPL_DATA)                       pDataBuffer;

  /**< @brief Can Hw oject Id*/
  VAR(Can_HwHandleType, AUTOMATIC)                           CanHwObject;

  /**< @brief SW Handler Id*/
  VAR(Can_IdType, AUTOMATIC)                                 PduId;

   /**< @brief Pdu Id*/
  VAR(PduIdType, AUTOMATIC)                                  SwPduHandle;

  /**< @brief Can TX Timeout*/
  VAR(uint16, AUTOMATIC)                                     CanTxTimeout;

  /**< @brief Can Data Len*/
  VAR(uint8, AUTOMATIC)                                      CanDataLen;

  /**< @brief Can Commnunication handling type for Tx */
  VAR(uint8, AUTOMATIC)                                      CanControllerId;

  /**< @brief Can channel using interrupts or polling */
  VAR(boolean, AUTOMATIC)                                    bPollingEnabled;

  /**< @brief Can notification function */
  CONST(ComDal_CalloutType, AUTOMATIC)                       ComDal_Notification;

} ComDal_CanDescriptorType;
#endif /* IFWK_USE_CAN */

#ifdef IFWK_USE_LLCE
/**
* @brief          Structure to configure LLCE channel descriptors
* @details        Data structure containing the set of configuration parameters required
*                 for setting up LLCE channels.
*/
typedef struct
{
  /**< @brief Bsw channel id*/
  VAR(uint8, AUTOMATIC)                                      ComChannelId;

  /**< @brief Buffer to store LLCE Data for TX or RX data depending on channed direction*/
  P2VAR(char, AUTOMATIC, FW_APPL_DATA)                       pDataBuffer;

  /**< @brief LLCE Hw oject Id*/
  VAR(Can_HwHandleType, AUTOMATIC)                           CanHwObject;

  /**< @brief SW Handler Id*/
  VAR(Can_IdType, AUTOMATIC)                                 PduId;

   /**< @brief Pdu Id*/
  VAR(PduIdType, AUTOMATIC)                                  SwPduHandle;

  /**< @brief LLCE TX Timeout*/
  VAR(uint16, AUTOMATIC)                                     CanTxTimeout;

  /**< @brief LLCE Data Len*/
  VAR(uint8, AUTOMATIC)                                      CanDataLen;

  /**< @brief LLCE Commnunication handling type for Tx */
  VAR(uint8, AUTOMATIC)                                      CanControllerId;

  /**< @brief LLCE channel using interrupts or polling */
  VAR(boolean, AUTOMATIC)                                    bPollingEnabled;

  /**< @brief LLCE notification function */
  CONST(ComDal_CalloutType, AUTOMATIC)                       ComDal_Notification;

} ComDal_LlceDescriptorType;
#endif /* IFWK_USE_CAN */

#ifdef IFWK_USE_ETH
#if defined(LWIP_IPV4) || defined(LWIP_IPV6)
/**
* @brief          Alias for the lwIP netif structure type.
* @details        Used for namespacing and type consistency.
*/
typedef struct netif ComDal_Eth_NetifType;

/**
* @brief          Alias for the lwIP dhcp structure type.
* @details        Used for namespacing and type consistency.
*/
typedef struct dhcp ComDal_Eth_DhcpType;

/**
* @brief          Alias for the lwIP autoip structure type.
* @details        Used for namespacing and type consistency.
*/
typedef struct autoip ComDal_Eth_AutoipType;

/**
* @brief          Alias for the lwIP packet buffer structure type.
* @details        Used for namespacing and type consistency.
*/
typedef struct pbuf ComDal_Eth_PacketBufferType;

/**
* @brief          Alias for the lwIP TCP control block structure type (PCB).
* @details        Used for namespacing and type consistency.
*/
typedef struct tcp_pcb ComDal_Eth_TcpControlBlockType;

/**
* @brief          Alias for the lwIP packet buffer layer enum.
* @details        Used for namespacing and type consistency.
*/
typedef pbuf_layer ComDal_Eth_PacketBufferLayerType;

/**
* @brief          Alias for the lwIP IP generic address type.
* @details        Used for namespacing and type consistency.
*/
typedef ip_addr_t ComDal_Eth_IpAddressType;
#if LWIP_IPV4
/**
* @brief          Alias for the lwIP IPv4 address type.
* @details        Used for namespacing and type consistency.
*/
typedef ip4_addr_t ComDal_Eth_Ipv4AddressType;
#endif
#if LWIP_IPV6
/**
* @brief          Alias for the lwIP IPv6 address type.
* @details        Used for namespacing and type consistency.
*/
typedef ip6_addr_t ComDal_Eth_Ipv6AddressType;
#endif /* LWIP_IPV6 */

/**
* @brief          Alias for the lwIP error type.
* @details        Used for namespacing and type consistency.
*/
typedef err_t ComDal_Eth_TcpipReturnType;

/**
* @brief          Alias for the lwIP MAC filter action enum.
* @details        Used for namespacing and type consistency.
*/
typedef enum netif_mac_filter_action ComDal_Eth_MacFilterActionType;
#endif /* defined(LWIP_IPV4) || defined(LWIP_IPV6) */

/**
 * @brief Structure for Ethernet IP addresses
 *
 * @param eType Type of IP address (IPv4 or IPv6)
 * @param ip Union containing the IP address
 *   - u8Ipv4 IPv4 address (4-byte array)
 *   - u32Ipv6 IPv6 address (4-word array)
 */
typedef struct 
{
    ComDal_IpTypes eType;
    union {
        /**< @brief IPv4 Address */
        VAR(uint8, AUTOMATIC)                                      u8Ipv4[4];
        /**< @brief IPv6 Address */
        VAR(uint32, AUTOMATIC)                                     u32Ipv6[4];
    }ip;
}ComDal_Eth_IpAddr_t;
/**
* @brief Structure for Ethernet client data
* @param pcb Pointer to the Ethernet TCP control block
* @param pBuff Pointer to the buffer containing the data
* @param u16BuffSize Size of the data buffer
* @param u16BuffLen Length of the data in the buffer
* @param u8InUse Flag indicating if the client is in use
*/
typedef struct {
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, COMDAL_APPL_CONST)  pcb;
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_CONST) pRxBuff;
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_CONST) pTxBuff;
    VAR(uint16, AUTOMATIC) u16BuffSize;
    VAR(uint16, AUTOMATIC) u16RxBuffLen;
    VAR(uint16, AUTOMATIC) u16TxBuffLen;
}ComDal_Eth_ClientType;
/**
 * @brief Structure for Ethernet TCP server configuration
 *
 * @param svrAddr Server listening address
 * @param pTcpClients Pointer to an array with all TCP clients structures
 * @param u8NoOfTcpClSupported Number of TCP clients connections supported by the server
 * @param u16TcpPort Listening port for the TCP server
 */
typedef struct {
    VAR(ComDal_Eth_IpAddr_t, AUTOMATIC)                                  svrAddr;
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, COMDAL_APPL_CONST)  pcb;
    P2VAR(ComDal_Eth_ClientType, AUTOMATIC, COMDAL_APPL_CONST)           pTcpClients;
    VAR(uint8, AUTOMATIC)                                                u8NoOfTcpClSupported;
    VAR(uint16, AUTOMATIC)                                               u16TcpPort;
}ComDal_Eth_TcpSvrConfType;

/**
* @brief          Structure used to configure the Ethernet interfaces.
* @details        Data structure containing the set of configuration parameters required
*                 for setting up the Ethernet interfaces with either static or dynamic allocation.
*/
typedef struct
{
    /**< @brief Ethernet MTU needed for the low-level driver initialization */
    VAR(uint16, AUTOMATIC)                                     u16EthMtu;

    /**< @brief Ethernet interface number */
    VAR(uint8, AUTOMATIC)                                      u8IfNumber;

    /**< @brief Ethernet interface name (e.g. "en"), excluding the null-terminator */
    VAR(char, AUTOMATIC)                                       aIfName[2];

    /**< @brief MAC Hardware address */
    VAR(uint8, AUTOMATIC)                                      aHwAddress[NETIF_MAX_HWADDR_LEN];

    /**< @brief Flag indicating whether IPv6 is enabled */
    VAR(boolean, AUTOMATIC)                                    bIpv6Enabled;

    /**< @brief Flag indicating whether DHCP is enabled */
    VAR(boolean, AUTOMATIC)                                    bDhcpEnabled;

    /**< @brief Flag indicating whether Auto IP is enabled */
    VAR(boolean, AUTOMATIC)                                    bAutoIpEnabled;

    /**< @brief Flag indicating whether this will be the default network interface */
    VAR(boolean, AUTOMATIC)                                    bDefaultInterface;

    /**< @brief The configured IP address, either IPv4 or IPv6 */
    VAR(ComDal_Eth_IpAddr_t, AUTOMATIC)                        IpAddress;

    /**< @brief The netmask */
    VAR(uint8, AUTOMATIC)                                      aNetmask[4];

    /**< @brief The default network gateway address */
    VAR(uint8, AUTOMATIC)                                      aDefaultGateway[4];

    /**< @brief The configured hostname used for communicating with the DNS */
    P2CONST(char, AUTOMATIC, ETH_APPL_CONST)                   pHostname;
}ComDal_Eth_InterfaceType;

/**
 * @brief Structure for Ethernet configuration
 *
 * @param pEthIf Pointer to an array of Ethernet interface structures
 * @param u8NoOfEthIf Number of Ethernet interfaces
 * @param pTcpSvr Pointer to an array of TCP server configurations
 * @param u8NoOfTcpSvr Number of TCP servers
 */
typedef struct {
    P2VAR(ComDal_Eth_InterfaceType, AUTOMATIC, COMDAL_APPL_CONST)  pEthIf;
    VAR(uint8, AUTOMATIC) u8NoOfEthIf;
    P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, COMDAL_APPL_CONST) pTcpSvr;
    VAR(uint8, AUTOMATIC) u8NoOfTcpSvr;
}ComDal_Eth_ConfigType;

/**
* @brief          Alias for the ReadFrame callback type.
* @details        This typedef is used to create an alias for the ReadFrame callback type.
*/
typedef ComDal_Eth_TcpipReturnType (*ComDal_Eth_ReadFrameCallbackType)(uint8_t, uint8_t *, uint16_t);
#endif /* IFWK_USE_ETH */

#ifdef IFWK_USE_UART
/**
* @brief          Structure to configure UART channel descriptors
* @details        Data structure containing the set of configuration parameters required
*                 for setting up UART channels.
*/
typedef struct
{
    /**< @brief Uart Hw oject Id*/
    VAR(uint8, AUTOMATIC)                                  UartChannelId;

    /**< @brief Buffer to store UART Data for TX or RX data depending on channed direction */
    P2VAR(ComDal_RingBuffer, AUTOMATIC,  FW_APPL_DATA)     pRingBuffer;

    /**< @brief Uart notification function */
    CONST(ComDal_CalloutType, AUTOMATIC)                   ComDal_Notification;
} ComDal_UartDescriptorType;
#endif /* IFWK_USE_UART */

/**
* @brief          Structure used for COMDAL channel mapping.
* @details
*/
typedef struct
{
    /**< @brief Bsw channel type*/
    CONST(ComDal_BswChannelType, AUTOMATIC)                ComChannelType;

    /**< @brief Bsw channel direction (TX or RX) */
    VAR(ComDal_AccessType, AUTOMATIC)                      AccessType;

    /**< @brief Bsw channel id*/
    CONST(ComDal_BswDescriptorIdType, AUTOMATIC)           ComDescriptorIndex;
} ComDal_BswChannelMapType;

#ifdef IFWK_USE_NFC
/**
* @brief          Structure to configure digital channels
* @details        Data structure containing the set of configuration parameters required
*                 for setting up digital input-output channels.
*/
typedef struct
{
    /**< @brief Bsw channel id*/
    VAR(uint8, AUTOMATIC)                                  ComChannelId;

    /**< @brief Buffer to store Nfc received Data*/
    P2VAR(char, AUTOMATIC,  FW_APPL_DATA)                  pDataBuffer;

    /**< @brief Nfc Data Len*/
    VAR(uint8, AUTOMATIC)                                  DataLen;
} ComDal_NfcDescriptorType;
#endif /* IFWK_USE_NFC */

#ifdef IFWK_USE_IPC
/**
* @brief          Structure to configure Inter-Processor Communication
* @details        Data structure containing the set of configuration parameters required
*                 for setting up Inter-Processor Communication channels.
*/
typedef struct
{
    /**< @brief Bsw channel id*/
    VAR(uint8, AUTOMATIC)                                  ComChannelId;

    /**< @brief Buffer to store Ipc received Data*/
    P2VAR(char, AUTOMATIC,  FW_APPL_DATA)                  pDataBuffer;

    /**< @brief Ipc Data Len*/
    VAR(uint8, AUTOMATIC)                                  DataLen;
} ComDal_IpcDescriptorType;
#endif /* IFWK_USE_IPC */

/**
* @brief          Structure containing configuration data for the initialization of COMDAL .
* @details        Data structure containing the set of configuration parameters required
*                 for initializing the COMDAL.
*/
typedef struct
{
    /**< @brief Total number of CAN channels */
    CONST(uint8, AUTOMATIC)                                           CanChannelCount;
    /**< @brief Total number of LIN channels */
    CONST(uint8, AUTOMATIC)                                           LinChannelCount;
    /**< @brief Total number of Ethernet interfaces */
    CONST(uint8, AUTOMATIC)                                           u8EthInterfaceCount;
    /**< @brief Total number of UART channels */
    CONST(uint8, AUTOMATIC)                                           UartChannelCount;
    /**< @brief Total number of NFC channels */
    CONST(uint8, AUTOMATIC)                                           NfcChannelCount;
    /**< @brief Total number of Inter-Processor Communication channels */
    CONST(uint8, AUTOMATIC)                                           IpcChannelCount;
    /**< @brief Total number of LLCE Communication channels */
    CONST(uint8, AUTOMATIC)                                           LlceChannelCount;

#ifdef IFWK_USE_CAN
    /**< @brief pointer to CAN channel descriptor */
    P2CONST(ComDal_CanDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST)  pCanChanDescriptor;
#endif /* IFWK_USE_CAN */

#ifdef IFWK_USE_LIN
    /**< @brief pointer to LIN channel descriptor */
    P2CONST(ComDal_LinDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST)  pLinChanDescriptor;
#endif /* IFWK_USE_LIN */

#ifdef IFWK_USE_ETH
    /**< @brief pointer to ETHERNET interface */
    P2CONST(ComDal_Eth_ConfigType, AUTOMATIC,  COMDAL_APPL_CONST)   pEthDescriptor;
#endif /* IFWK_USE_ETH */

#ifdef IFWK_USE_UART
    /**< @brief pointer to UART channel descriptor */
    P2CONST(ComDal_UartDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST) pUartChanDescriptor;
#endif /* IFWK_USE_UART */

#ifdef IFWK_USE_NFC
    /**< @brief pointer to NFC channel descriptor */
    P2CONST(ComDal_NfcDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST) pNfcChanDescriptor;
#endif /* IFWK_USE_NFC */

#ifdef IFWK_USE_IPC
    /**< @brief pointer to Inter-Processor Communication channel descriptor */
    P2CONST(ComDal_IpcDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST) pIpcChanDescriptor;
#endif /* IFWK_USE_IPC */

#ifdef IFWK_USE_LLCE
    /**< @brief pointer to LLCE channel descriptor */
    P2CONST(ComDal_LlceDescriptorType, AUTOMATIC,  COMDAL_APPL_CONST)  pLlceChanDescriptor;
#endif /* IFWK_USE_LLCE */

    /**< @brief pointer to COM channel map */
    P2CONST(ComDal_BswChannelMapType,  AUTOMATIC, ADC_APPL_CONST) pComChannelMap;

} ComDal_ConfigType;

/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif  /* COMDAL_TYPES_H */
/** @} */
