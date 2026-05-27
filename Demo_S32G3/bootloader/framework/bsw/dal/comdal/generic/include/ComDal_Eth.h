/**
*   @file           ComDal_Eth.h
*   @version        24.12.0
*
*   @brief          ComDal header file for the Ethernet communication.
*   @details        ComDal interface for handling the Ethernet communication.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*===================================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
=====================================================================================================================*/

#ifndef COMDAL_ETH_H
#define COMDAL_ETH_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*===================================================================================================================
                               INCLUDE FILES
=====================================================================================================================*/
#include "Std_Types.h"

#include "ComDal_Cfg.h"
#include "ComDal_Types.h"

#ifdef IFWK_USE_ETH
#include "lwip/opt.h"
#endif /* IFWK_USE_ETH */

/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal_Eth.h
*/
#define COMDAL_ETH_VENDOR_ID                           43
#define COMDAL_ETH_MODULE_ID                           100

#define COMDAL_ETH_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_ETH_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_ETH_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_ETH_SW_MAJOR_VERSION                    24
#define COMDAL_ETH_SW_MINOR_VERSION                    12
#define COMDAL_ETH_SW_PATCH_VERSION                    0

/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/**
* @file           ComDal_Eth.h
*/
#if (COMDAL_ETH_VENDOR_ID != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Eth.h and ComDal_Cfg.h have different vendor ids"
#endif

#if (COMDAL_ETH_VENDOR_ID != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Eth.h and ComDal_Types.h have different vendor ids"
#endif

#if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_MINOR_VERSION != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_REVISION_VERSION != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth.h and ComDal_Cfg.h are different"
#endif

#if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_MINOR_VERSION != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_REVISION_VERSION != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth.h and ComDal_Types.h are different"
#endif

#if ((COMDAL_ETH_SW_MAJOR_VERSION != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_SW_MINOR_VERSION != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_ETH_SW_PATCH_VERSION != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth.h and ComDal_Cfg.h are different"
#endif

#if ((COMDAL_ETH_SW_MAJOR_VERSION != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_SW_MINOR_VERSION != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_ETH_SW_PATCH_VERSION != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth.h and ComDal_Types.h are different"
#endif

/*==================================================================================================================
*                             DEFINES AND MACROS
====================================================================================================================*/
/* Macros for initializing an IPv4/IPv6 variable from raw uint array. */
#ifdef LWIP_IPV4
#define COMDAL_IP4_ADDR_INIT(var, raw_addr) IP4_ADDR(&(var), (raw_addr)[0], (raw_addr)[1], (raw_addr)[2], (raw_addr)[3])
#endif /* LWIP_IPV4 */
#ifdef LWIP_IPV6
#define COMDAL_IP6_ADDR_INIT(var, raw_addr) IP6_ADDR(&(var), (raw_addr)[0], (raw_addr)[1], (raw_addr)[2], (raw_addr)[3])
#endif /* LWIP_IPV6 */
#if ((LWIP_IGMP) && (LWIP_IPV4))
/* The IPv4 multicast prefix according to RFC 1112. */
#define COMDAL_IPV4_MAC_MULTICAST_PREFIX 0x01005Eu
#endif /* LWIP_IGMP && LWIP_IVP4 */

#if ((LWIP_IPV6) && (LWIP_IPV6_MLD))
/* The IPv6 multicast prefix according to RFC 2464. */
#define COMDAL_IPV6_MAC_MULTICAST_PREFIX 0x3333u
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

/*===================================================================================================================
*                             GLOBAL FUNCTION DECLARATIONS
====================================================================================================================*/
#if (COMDAL_MAX_ETH_INTERFACES > 0U)
/*==================================================================================================================*/
/**
* @brief      Initialize all configured network interfaces
* @details    - initialize the stack;
*             - set the netif init callback;
*             - set the interface details (i.e. IP, Netmask, Gateway);
*             - set the default interface (i.e. interface used when no specific route is found);
*             - start DHCP/AutoIP clients;
*             - if IPv6 is used, generate the link-local address from the hardware address (by appending the EUI-64 ID to the FE80:: prefix);
*             - bring the interface up to be used by the TCP/IP stack.
* 
* @param[in]  pConfigData     Pointer to the global configuration structure
* 
* @return     None
*/
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_Init
(
    P2CONST(ComDal_ConfigType, AUTOMATIC, ETH_APPL_CONST) pConfigData
);

/*==================================================================================================================*/
/**
* @brief      Deinitialize all network interfaces
* @details    Call the low-level deinit function for each configured netif.
* 
* @return     None
*/
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DeInit(void);

/*==================================================================================================================*/
/**
* @brief      Poll each interface for new data and trigger the lwIP timeout handlers
* @details    Each interface will be checked if it has any new RX frames.
*             Additionally, fire the timeout handlers for the TCP/IP stack.
* 
* @return     None
*/
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_Main(void);

/*==================================================================================================================*/
/**
* @brief        Check if the network instance has been assigned an IP address.   
* @details      The function should be called from ComManager at a certain interval in order not to starve the system.
*               If the network instance does not have an IP address, ERR_VAL is returned.
*               If the device is assigned an IP address, ERR_OK is returned.
*
* @param[in]    u8Instance                   The network instance
*
* @return       The error status:            ERR_VAL, ERR_OK
*/
/*==================================================================================================================*/
FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_CheckIpAddress
(
    VAR(uint8_t, AUTOMATIC) u8Instance
);

/*==================================================================================================================*/
/**
 * @brief Write data to an Ethernet channel.
 *
 * @param[in] ComChannel The Ethernet channel to write to.
 * @param[in] pWriteBuffer Pointer to the buffer containing the data to write.
 * @param[in] u16WriteBufferSize The size of the write buffer, in bytes.
 *
 * @return COMDAL_OK if the data was written successfully, COMDAL_E_NOT_OK otherwise.
 */
/*==================================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Eth_WriteData
(
    VAR(ComDal_ChannelType, AUTOMATIC)  ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) pWriteBuffer,
    VAR(uint16, AUTOMATIC) u16WriteBufferSize
);

/*==================================================================================================================*/
/**
 * @brief Read data from an Ethernet channel.
 *
 * @param[in] ComChannel The Ethernet channel to read from.
 * @param[out] pReadBuffer Pointer to the buffer where the read data will be stored.
 * @param[in,out] pBufferSize Pointer to the size of the read buffer. On input, this value should contain the maximum size of the buffer. On output, it will contain the actual number of bytes read.
 *
 * @return COMDAL_OK if data was read successfully, COMDAL_E_NOT_OK otherwise.
 */
/*==================================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Eth_ReadData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) pReadBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pBufferSize
);

/*==================================================================================================================*/
/**
 * @brief Get the network interface for the given instance.
 *
 * @param[in] u8Instance The Ethernet driver index.
 *
 * @return ComDal_Eth_NetifType if there is a network interface for the given instance, NULL otherwise.
 */
/*==================================================================================================================*/
FUNC(P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA), AUTOMATIC) ComDal_Eth_GetNetifByInstanceNum
(
    VAR(uint8_t, AUTOMATIC) u8Instance
);

/*==================================================================================================================*/
/**
 * @brief Set arp offloading for the given network interface.
 *
 * @param[in] IfNum The network interface index.
 * @param[in] pTargetAddr The target address.
 * @param[in] bEnable The enable flag.
 * @return None
 */
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_SetArpOffloading
(
    VAR(uint8, AUTOMATIC) IfNum,
    P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pTargetAddr,
    VAR(boolean, AUTOMATIC) bEnable
);
#endif /* (COMDAL_MAX_ETH_INTERFACES > 0U) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMDAL_ETH_H */
/** @} */
