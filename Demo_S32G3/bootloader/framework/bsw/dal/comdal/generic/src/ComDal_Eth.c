/**
*   @file           ComDal_Eth.c
*   @version        24.12.0
*
*   @brief          ComDal Abstraction Layer ETH component
*   @details        This file implements the ComDal ETH interface to the TCP/IP stack.
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

/*===================================================================================================================================
                                          INCLUDE FILES
=====================================================================================================================================*/
#include "Std_Types.h"

#include "ComDal_Types.h"
#include "ComDal_Cfg.h"
#include "ComDal_Eth.h"
#include "MemLib.h"
#ifdef IFWK_USE_ETH
#include "EthIf_Cbk.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/api.h"
#include "lwip/arch.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
#include "lwip/autoip.h"
#endif /* LWIP_AUTOIP */

#if LWIP_IPV6
#include "lwip/ethip6.h"
#endif /* LWIP_IPV6 */
#endif /* IFWK_USE_ETH */

/*=================================================================================================================================
*                                         SOURCE FILE VERSION INFORMATION
===================================================================================================================================*/
#define COMDAL_ETH_VENDOR_ID_C                       43

#define COMDAL_ETH_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_ETH_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_ETH_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_ETH_SW_MAJOR_VERSION_C                24
#define COMDAL_ETH_SW_MINOR_VERSION_C                12
#define COMDAL_ETH_SW_PATCH_VERSION_C                0

/*=================================================================================================================================
*                                         FILE VERSION CHECKS
===================================================================================================================================*/
#if (COMDAL_MAX_ETH_INTERFACES > 0U)

/* Check if this source file and the Comdal configuration header file are of the same vendor. */
#if (COMDAL_ETH_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Eth.c and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if this source file and the Comdal configuration header file are of the same Autosar version. */
#if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth.c and ComDal_Cfg.h are different"
#endif

/* Check if this source file and the Comdal configuration header file are of the same Software version. */
#if ((COMDAL_ETH_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_ETH_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth.c and ComDal_Cfg.h are different"
#endif

/* Check if this source file and the Comdal types header file are of the same vendor. */
#if (COMDAL_ETH_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Eth.c and ComDal_Types.h have different vendor ids"
#endif

/* Check if this source file and the Comdal types header file are of the same Autosar version. */
#if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth.c and ComDal_Types.h are different"
#endif

/* Check if this source file and the Comdal types header file are of the same Software version. */
#if ((COMDAL_ETH_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_ETH_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Eth.c and ComDal_Types.h are different"
#endif

/* Check if this source file and the Comdal ETH header file are from the same vendor. */
#if (COMDAL_ETH_VENDOR_ID_C != COMDAL_ETH_VENDOR_ID)
    #error "ComDal_Eth.c and ComDal_Eth.h have different vendor ids"
#endif

/* Check if this source file and the Comdal ETH header file are of the same Autosar version. */
#if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION_C != COMDAL_ETH_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_MINOR_VERSION_C != COMDAL_ETH_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_AR_RELEASE_REVISION_VERSION_C != COMDAL_ETH_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Eth.c and ComDal_Eth.h are different"
#endif

/* Check if this source file and the Comdal ETH header file are of the same Software version. */
#if ((COMDAL_ETH_SW_MAJOR_VERSION_C != COMDAL_ETH_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_SW_MINOR_VERSION_C != COMDAL_ETH_SW_MINOR_VERSION) || \
     (COMDAL_ETH_SW_PATCH_VERSION_C != COMDAL_ETH_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of ComDal_Eth.c and ComDal_Eth.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if this source file and the StandardTypes header file are of the same AutoSar version. */
    #if ((COMDAL_ETH_AR_RELEASE_MAJOR_VERSION_C != STD_AR_RELEASE_MAJOR_VERSION) || \
        (COMDAL_ETH_AR_RELEASE_MINOR_VERSION_C != STD_AR_RELEASE_MINOR_VERSION) || \
        (COMDAL_ETH_AR_RELEASE_REVISION_VERSION_C != STD_AR_RELEASE_REVISION_VERSION) \
        )
        #error "AutoSar Version Numbers of ComDal_Eth.c and StandardTypes.h are different"
    #endif
#endif


/*=================================================================================================================================
*                                         LOCAL VARIABLES
===================================================================================================================================*/
/* Array containing all the underlying netifs used by the TCP/IP stack. */
static VAR(ComDal_Eth_NetifType, AUTOMATIC)                  ComDal_NetworkInterfaces[COMDAL_MAX_ETH_INTERFACES];

/* Global configuration pointer passed to the main function. */
static P2CONST(ComDal_ConfigType, AUTOMATIC, ETH_APPL_CONST) ComDal_pEthConfigData;

#if LWIP_DHCP
/* Each lwIP network interface will have a corresponding DHCP structure if DHCP is enabled. */
static VAR(ComDal_Eth_DhcpType, AUTOMATIC)                   ComDal_NetworkInterfaces_DhcpInfo[COMDAL_MAX_ETH_INTERFACES];
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
/* Each lwIP network interface will have a corresponding AutoIP structure if AutoIP is enabled. */
static VAR(ComDal_Eth_AutoipType, AUTOMATIC)                 ComDal_NetworkInterfaces_AutoipInfo[COMDAL_MAX_ETH_INTERFACES];
#endif /* LWIP_AUTOIP */

#if LWIP_DHCP
#if LWIP_DHCP_AUTOIP_COOP
#define COMDAL_ETH_SUPPLIED_ADDRESS(n)      (dhcp_supplied_address(n) || autoip_supplied_address(n))
#else
#define COMDAL_ETH_SUPPLIED_ADDRESS(n)      dhcp_supplied_address(n)
#endif /* LWIP_DHCP_AUTOIP_COOP */
#elif LWIP_AUTOIP
#define COMDAL_ETH_SUPPLIED_ADDRESS(n)      autoip_supplied_address(n)
#else
#define COMDAL_ETH_SUPPLIED_ADDRESS(n)      1
#endif /* LWIP_DHCP */
/*=================================================================================================================================
*                                         LOCAL FUNCTION PROTOTYPES
===================================================================================================================================*/
/************************Eth driver porting**********************/
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_WriteFrame
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2VAR(ComDal_Eth_PacketBufferType, AUTOMATIC, ETH_APPL_DATA) pTxBuffer
);

static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_NetifInit
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif
);

#if ((LWIP_IGMP) && (LWIP_IPV4))
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_IgmpGroupFilterCallback
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2CONST(ComDal_Eth_Ipv4AddressType, AUTOMATIC, ETH_APPL_CONST) pIgmpGroup,
    VAR(ComDal_Eth_MacFilterActionType, AUTOMATIC) eAction
);
#endif /* LWIP_IGMP && LWIP_IVP4 */

#if ((LWIP_IPV6) && (LWIP_IPV6_MLD))
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_MldGroupFilterCallback
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2CONST(ComDal_Eth_Ipv6AddressType, AUTOMATIC, ETH_APPL_CONST) pMldGroup,
    VAR(ComDal_Eth_MacFilterActionType, AUTOMATIC) eAction
);
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */
/************************TCP callbacks**********************/
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerAccept
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) newPcb,
    VAR(err_t, AUTOMATIC) err
);
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerRecv
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb,
    P2VAR(ComDal_Eth_PacketBufferType, AUTOMATIC, ETH_APPL_DATA) p,
    VAR(err_t, AUTOMATIC) err
);
static FUNC(void, AUTOMATIC)  ComDal_Eth_tcpServerError
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    VAR(err_t, AUTOMATIC) err
);
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerPoll
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb
);
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerSent
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb,
    VAR(uint16, AUTOMATIC) len
);
static FUNC(void, AUTOMATIC) ComDal_Eth_InitTcpServer
(
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig
);
static FUNC(void, AUTOMATIC) ComDal_Eth_DeinitTcpServer
(
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig
);
/*==================================================================================================================================
*                                       GLOBAL FUNCTIONS
===================================================================================================================================*/
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
)
{
    VAR(uint8, AUTOMATIC) u8Index = 0U;
    VAR(ComDal_Eth_InterfaceType, AUTOMATIC) CurrentEthInterface;
#if LWIP_IPV4
    VAR(ComDal_Eth_Ipv4AddressType, AUTOMATIC) NetifIpAddr;
    VAR(ComDal_Eth_Ipv4AddressType, AUTOMATIC) NetifMask;
    VAR(ComDal_Eth_Ipv4AddressType, AUTOMATIC) NetifGateway;
#endif
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig;
#if LWIP_IPV6
    VAR(ComDal_Eth_Ipv6AddressType, AUTOMATIC) NetifIpv6Addr;
#endif /* LWIP_IPV6 */

    Eth_SetControllerMode(0U, ETH_MODE_ACTIVE);
    ComDal_Eth_DrfIf_PhyInit(0u);
#if LWIP_IPV4
    ip4_addr_set_any(&NetifIpAddr);
    ip4_addr_set_any(&NetifMask);
    ip4_addr_set_any(&NetifGateway);
#endif
#if LWIP_IPV6
    ip6_addr_set_any(&NetifIpv6Addr);
#endif /* LWIP_IPV6 */


    /* Initialize all lwIP modules before adding the network interfaces. */
    lwip_init();
    ComDal_pEthConfigData =  pConfigData;
    pEthConfig = (ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor;
    for (u8Index = 0U; u8Index < pEthConfig->u8NoOfEthIf; u8Index++)
    {
        CurrentEthInterface = pEthConfig->pEthIf[u8Index];
#if LWIP_IPV6
        if (TRUE == CurrentEthInterface.bIpv6Enabled)
        {
            COMDAL_IP6_ADDR_INIT(NetifIpv6Addr, CurrentEthInterface.IpAddress.ip.u32Ipv6);
#if LWIP_IPV4
            netif_add(&ComDal_NetworkInterfaces[u8Index], NULL, NULL, NULL, NULL, ComDal_Eth_NetifInit, netif_input);
#else
            netif_add(&ComDal_NetworkInterfaces[u8Index], NULL, ComDal_Eth_NetifInit, netif_input);
#endif
        }
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
        /* Assign static IPv4 only if DHCP and AutoIP are both disabled. */
        if ((FALSE == CurrentEthInterface.bDhcpEnabled) && (FALSE == CurrentEthInterface.bAutoIpEnabled))
        {
            COMDAL_IP4_ADDR_INIT(NetifGateway, CurrentEthInterface.aDefaultGateway);
            COMDAL_IP4_ADDR_INIT(NetifIpAddr, CurrentEthInterface.IpAddress.ip.u8Ipv4);
            COMDAL_IP4_ADDR_INIT(NetifMask, CurrentEthInterface.aNetmask);

            netif_add(&ComDal_NetworkInterfaces[u8Index], &NetifIpAddr, &NetifMask, &NetifGateway, NULL, ComDal_Eth_NetifInit, netif_input);
        }
        /* Add the IPv4 interface without static allocation. */
        else
        {
            netif_add(&ComDal_NetworkInterfaces[u8Index], NULL, NULL, NULL, NULL, ComDal_Eth_NetifInit, netif_input);
        }
#endif /* LWIP_IPV4 */
        if (TRUE == CurrentEthInterface.bDefaultInterface)
        {
            netif_set_default(&ComDal_NetworkInterfaces[u8Index]);
        }
#if LWIP_IPV6
        if (TRUE == CurrentEthInterface.bIpv6Enabled)
        {
            netif_create_ip6_linklocal_address(&ComDal_NetworkInterfaces[u8Index], 1);
            /* Manually set the IPv6 address if it is global, otherwise stay link-local. */
            if (ip6_addr_isglobal(&NetifIpv6Addr) ||
                ip6_addr_isuniquelocal(&NetifIpv6Addr))
            {
                /* The first slot is reserved to the link-local address. */
                netif_add_ip6_address(&ComDal_NetworkInterfaces[u8Index], &NetifIpv6Addr, NULL);
            }
        }
#endif /* LWIP_IPV6 */

#if LWIP_DHCP
        /* If DHCP is enabled, then the network interface will have an attached DHCP structure. */
        if (TRUE == CurrentEthInterface.bDhcpEnabled)
        {
            dhcp_set_struct(&ComDal_NetworkInterfaces[u8Index], &ComDal_NetworkInterfaces_DhcpInfo[u8Index]);
        }
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
        /* If AutoIP is enabled, then the network interface will have an attached AutoIP structure. */
        if (TRUE == CurrentEthInterface.bAutoIpEnabled)
        {
            autoip_set_struct(&ComDal_NetworkInterfaces[u8Index], &ComDal_NetworkInterfaces_AutoipInfo[u8Index]);
        }
#endif /* LWIP_AUTOIP */

        netif_set_up(&ComDal_NetworkInterfaces[u8Index]);

#if LWIP_DHCP
        /* Start the DHCP negotiation for the current interface. */
        if (TRUE == CurrentEthInterface.bDhcpEnabled)
        {
            dhcp_start(&ComDal_NetworkInterfaces[u8Index]);
        }
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
        /* Start the AutoIP client for the current interface. */
        if (TRUE == CurrentEthInterface.bAutoIpEnabled)
        {
            autoip_start(&ComDal_NetworkInterfaces[u8Index]);
        }
#endif /* LWIP_AUTOIP */
    }
    ComDal_Eth_InitTcpServer(pEthConfig);
}

/*==================================================================================================================*/
/**
* @brief      Deinitialize all network interfaces
* @details    Call the low-level deinit function for each configured netif.
*
* @return     None
*/
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DeInit(void)
{
    VAR(uint8, AUTOMATIC) u8Index = 0U;

    ComDal_Eth_DeinitTcpServer((ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor);
    for (u8Index = 0U; u8Index < COMDAL_MAX_ETH_INTERFACES; u8Index++)
    {
        netif_set_down(&ComDal_NetworkInterfaces[u8Index]);
        netif_remove(&ComDal_NetworkInterfaces[u8Index]);
    }
    ComDal_pEthConfigData = NULL;
}

/*==================================================================================================================*/
/**
* @brief      Poll each interface for new data and trigger the lwIP timeout handlers
* @details    Each interface will be checked if it has any new RX frames.
*             Additionally, fire the timeout handlers for the TCP/IP stack.
*
* @return     None
*/
/*==================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_Main(void)
{
    /* Trigger the lwIP timeout triggers. */
    sys_check_timeouts();
}

FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Eth_WriteData(
    ComDal_ChannelType ComChannel, char *pWriteBuffer, uint16 u16WriteBufferSize)
{
    P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, COMDAL_APPL_CONST) pTcpSvr;
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) pEthConfig;
    VAR(uint8, AUTOMATIC) u8Idx ;
    VAR(ComDal_ReturnType, AUTOMATIC) eRetVal = COMDAL_OBSOLETE;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8BswDescriptorId;

    u8BswDescriptorId = ComDal_pEthConfigData->pComChannelMap[ComChannel].ComDescriptorIndex;
    pEthConfig = (ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor;
    if(u8BswDescriptorId <= pEthConfig->u8NoOfTcpSvr)
    {
        pTcpSvr = &pEthConfig->pTcpSvr[u8BswDescriptorId];
        if(NULL != pTcpSvr->pcb)
        {
            for(u8Idx = 0U; u8Idx < pTcpSvr->u8NoOfTcpClSupported; u8Idx++)
            {
                if(NULL != pTcpSvr->pTcpClients[u8Idx].pcb)
                {
                    MemLib_MemCpy(pTcpSvr->pTcpClients[u8Idx].pTxBuff, pWriteBuffer, u16WriteBufferSize);
                    pTcpSvr->pTcpClients[u8Idx].u16TxBuffLen = u16WriteBufferSize;
                    eRetVal = COMDAL_NO_ERROR;
                }
            }
        }
    }
    return eRetVal;
}

FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Eth_ReadData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) pReadBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pBufferSize
)
{
    P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, COMDAL_APPL_CONST) pTcpSvr;
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) pEthConfig;
    VAR(uint8, AUTOMATIC) u8Idx ;
    VAR(ComDal_ReturnType, AUTOMATIC) eRetVal = COMDAL_OBSOLETE;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8BswDescriptorId;

    u8BswDescriptorId = ComDal_pEthConfigData->pComChannelMap[ComChannel].ComDescriptorIndex;
    pEthConfig = (ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor;
        if(u8BswDescriptorId <= pEthConfig->u8NoOfTcpSvr)
    {
        pTcpSvr = &pEthConfig->pTcpSvr[u8BswDescriptorId];
        if(NULL != pTcpSvr->pcb)
        {
            for(u8Idx = 0U; u8Idx < pTcpSvr->u8NoOfTcpClSupported; u8Idx++)
            {
                if(0 < pTcpSvr->pTcpClients[u8Idx].u16RxBuffLen)
                {
                    MemLib_MemCpy(pReadBuffer, pTcpSvr->pTcpClients[u8Idx].pRxBuff, pTcpSvr->pTcpClients[u8Idx].u16RxBuffLen);
                    *pBufferSize = pTcpSvr->pTcpClients[u8Idx].u16RxBuffLen;
                    pTcpSvr->pTcpClients[u8Idx].u16RxBuffLen = 0U;
                    eRetVal = COMDAL_NO_ERROR;
                    break;
                }
            }
        }
    }
    return eRetVal;
}

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
)
{
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_VAL;
    
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif;
    pNetif = &ComDal_NetworkInterfaces[u8Instance];
    
    if (COMDAL_ETH_SUPPLIED_ADDRESS(&pNetif))
    {
        Status = ERR_OK;
    }
    return Status;
}

/*=================================================================================================================================
*                                       LOCAL FUNCTIONS
===================================================================================================================================*/
/*=================================================================================================================================*/
/**
* @brief      Look up a network interface after its instance number
* @details    Look up the network interfaces array for a network interface, by the instance number.
*             This function is used to avoid coupling the TCP/IP stack to the driver interface.
*
* @param[in]  u8Instance                   The netif instance number
*
* @return     Pointer to the netif structure, if found. Otherwise, NULL.
*/
/*=================================================================================================================================*/
FUNC(P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA), AUTOMATIC) ComDal_Eth_GetNetifByInstanceNum
(
    VAR(uint8_t, AUTOMATIC) u8Instance
)
{
    VAR(uint8, AUTOMATIC) u8Index = 0U;
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif = NULL_PTR;
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) pEthConfig;
    
    pEthConfig = (ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor;
    for(u8Index = 0U; u8Index < pEthConfig->u8NoOfEthIf; u8Index++)
    {
        if (ComDal_NetworkInterfaces[u8Index].num == u8Instance)
        {
            pNetif = &ComDal_NetworkInterfaces[u8Index];
            break;
        }
    }

    return pNetif;
}

/*=================================================================================================================================*/
/**
* @details    This function creates the TCP servers and binds them to the specified ports.
*
* @param[in]  pEthConfig                   The netif instance number
*
* @return     none
*/
/*=================================================================================================================================*/
static FUNC(void, AUTOMATIC) ComDal_Eth_InitTcpServer
(
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig
)
{
    VAR(uint8, AUTOMATIC) u8Index = 0U;
    VAR(ComDal_Eth_IpAddressType,AUTOMATIC) ipAddr;
    VAR(err_t, AUTOMATIC)   err;
    for (u8Index = 0U; u8Index < pEthConfig->u8NoOfTcpSvr; u8Index++)
    {
        pEthConfig->pTcpSvr[u8Index].pcb = tcp_new();
        if (NULL != pEthConfig->pTcpSvr[u8Index].pcb) {
            if(COMDAL_IPV4 == pEthConfig->pTcpSvr[u8Index].svrAddr.eType)
            {
#if LWIP_IPV4
                COMDAL_IP4_ADDR_INIT(ipAddr, pEthConfig->pTcpSvr[u8Index].svrAddr.ip.u8Ipv4);
#endif /* LWIP_IPV4 */
            }
            else
            {
#if LWIP_IPV6
                COMDAL_IP6_ADDR_INIT(ipAddr, pEthConfig->pTcpSvr[u8Index].svrAddr.ip.u32Ipv6);
#endif /* LWIP_IPV6 */
            }
            err = tcp_bind(pEthConfig->pTcpSvr[u8Index].pcb,
                            &ipAddr,
                            pEthConfig->pTcpSvr[u8Index].u16TcpPort);

            if(ERR_OK == err)
            {
                pEthConfig->pTcpSvr[u8Index].pcb = 
                                    tcp_listen(pEthConfig->pTcpSvr[u8Index].pcb);
                tcp_arg(pEthConfig->pTcpSvr[u8Index].pcb, 
                    &pEthConfig->pTcpSvr[u8Index]);
                /* initialize LwIP tcp_accept callback function */
                tcp_accept(pEthConfig->pTcpSvr[u8Index].pcb, ComDal_Eth_tcpServerAccept);
            }
        }
        else
        {
            /* Error creating PCB. Out of memory? */
        }
    }
}

/*=================================================================================================================================*/
/**
* @details    This function deletes the TCP servers and unbinds them from the specified ports.
*
* @param[in]  pEthConfig                   The netif instance number
*
* @return     none
*/
/*=================================================================================================================================*/
static FUNC(void, AUTOMATIC) ComDal_Eth_DeinitTcpServer
(
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig
)
{
    VAR(uint8, AUTOMATIC) u8Idx = 0U;
    VAR(uint8, AUTOMATIC) u8IdxCl = 0U;
    P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, ETH_APPL_DATA) pTcpSvr;

    for (u8Idx = 0U; u8Idx < pEthConfig->u8NoOfTcpSvr; u8Idx++)
    {
        pTcpSvr = &pEthConfig->pTcpSvr[u8Idx];
        for(u8IdxCl = 0; u8IdxCl < pTcpSvr->u8NoOfTcpClSupported; u8IdxCl++)
        {
            if(NULL != pTcpSvr->pTcpClients[u8IdxCl].pcb)
            {
                (void)tcp_close(pTcpSvr->pTcpClients[u8IdxCl].pcb);
                pTcpSvr->pTcpClients[u8IdxCl].pcb = NULL;
            }
        }
        if(NULL != pTcpSvr->pcb)
        {
            (void)tcp_close(pTcpSvr->pcb);
            (void)tcp_shutdown(pTcpSvr->pcb, 1U, 1U);
            pTcpSvr->pcb = NULL;
        }
    }
}

/*=================================================================================================================================*/
/**
* @brief      Used as the linkoutput callback for the TCP/IP stack.
* @details    For the given network interface, this function will:
*               - check if the memory allocation of the coalesced packet buffer is successful;
*               - send the pbuf payload and total length to the driver to be transmitted;
*               - will be a blocking call until the request is sent or timed out.
*             If the memory allocation failed, ERR_MEM is returned.
*             If the payload is incorrect, ERR_VAL is returned.
*             If the driver times out, ERR_TIMEOUT is returned.
*             If the driver successfully sent the frame, ERR_OK is returned.
*
* @param[in]  pNetif                       Pointer to the netif used to send the buffer
* @param[in]  pTxBuffer                    Pointer to the TX buffer containing the payload
*
* @return     The error status:            ERR_MEM, ERR_VAL, ERR_TIMEOUT, ERR_OK
*/
/*=================================================================================================================================*/
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_WriteFrame
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2VAR(ComDal_Eth_PacketBufferType, AUTOMATIC, ETH_APPL_DATA) pTxBuffer
)
{
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_OK;
    P2VAR(ComDal_Eth_PacketBufferType, AUTOMATIC, ETH_APPL_DATA) Buffer = pTxBuffer;

    /* Increase the ref count on the initial buffer */
    pbuf_ref(pTxBuffer);

    /* If this is a chained buffer, coalesce all of them into one. */
    if (NULL != pTxBuffer->next)
    {
        Buffer = pbuf_coalesce(pTxBuffer, PBUF_RAW_TX);
        /* Check if the buffer allocation failed. */
        if (Buffer == pTxBuffer)
        {
            Status = ERR_MEM;
        }
    }

    if (ERR_MEM != Status)
    {
        Status = ComDal_Eth_DrvIf_WriteFrame(pNetif->num, Buffer->payload, Buffer->tot_len);
        pbuf_free(Buffer);
    }

    return Status;
}

/*=================================================================================================================================*/
/**
* @brief          Passed as a callback to the netif_add call to configure the network interface.
* @details        For the given network interface, this function will:
*                   - set the netif details given through the user config;
*                   - set the output callbacks for both IPv4 and IPv6;
*                   - call the low-level driver initialization function.
*                 If the configuration values are invalid, ERR_VAL is returned.
*                 If the low-level init fails, ERR_IF is returned.
*                 If the interface initialization was successful, ERR_OK is returned.
*
* @param[in,out]  pNetif                    Pointer to the netif structure to initialize
*
* @return         The error status:         ERR_OK, ERR_VAL, ERR_IF
*/
/*=================================================================================================================================*/
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_NetifInit
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif
)
{
    VAR(uint8, AUTOMATIC) InstanceNum = pNetif->num;
    P2VAR(ComDal_Eth_ConfigType, AUTOMATIC, ETH_APPL_DATA) pEthConfig;
    P2CONST(ComDal_Eth_InterfaceType, AUTOMATIC, ETH_APPL_CONST) pEthInterface;
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_OK;
    VAR(uint8, AUTOMATIC) u8Index = 0U;
    pEthConfig = (ComDal_Eth_ConfigType*)ComDal_pEthConfigData->pEthDescriptor;
    pEthInterface = &pEthConfig->pEthIf[InstanceNum];
#if LWIP_IPV6
    VAR(ComDal_Eth_Ipv6AddressType, AUTOMATIC) Ipv6LinkLocalNodes;
    ip6_addr_set_allnodes_linklocal(&Ipv6LinkLocalNodes);
#endif /* LWIP_IPV6 */

    /* Copy over user configured interface details. */
    pNetif->name[0] = pEthInterface->aIfName[0];
    pNetif->name[1] = pEthInterface->aIfName[1];
    pNetif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
    for (u8Index = 0U; u8Index < NETIF_MAX_HWADDR_LEN; u8Index++)
    {
        pNetif->hwaddr[u8Index] = pEthInterface->aHwAddress[u8Index];
    }
    pNetif->mtu = pEthInterface->u16EthMtu;
    /* NETIF_FLAG_ERHARP flag should be used only in conjunction with ARP-related functionality. */
    pNetif->flags = (uint8) (NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHERNET);
#if LWIP_IGMP
    pNetif->flags |= (uint8) NETIF_FLAG_IGMP;
    pNetif->igmp_mac_filter = ComDal_Eth_IgmpGroupFilterCallback;
#endif /* LWIP_IGMP */

#if LWIP_IPV4
#if LWIP_ARP
    /* Use the default output function, which fills in the Ethernet L2 header for the outgoing packet. */
    pNetif->output = etharp_output;
    pNetif->flags |= NETIF_FLAG_ETHARP;
#else /* LWIP_ARP */
    pNetif->output = NULL;
#endif /* LWIP_ARP */
#endif /* LWIP_IPV4 */

    NETIF_SET_CHECKSUM_CTRL(pNetif, NETIF_CHECKSUM_SETTING);

#if LWIP_IPV6
    if (TRUE == pEthInterface->bIpv6Enabled)
    {
        /* Use the default IPv6 output function which maps the multicast IP address to MAC, if needed. */
        pNetif->output_ip6 = ethip6_output;
#if LWIP_IPV6_MLD
        pNetif->flags |= (uint8) NETIF_FLAG_MLD6;
        pNetif->mld_mac_filter = ComDal_Eth_MldGroupFilterCallback;
        /* Multicast packets must be allowed for link-local listening nodes, which are handled by default with IPv6. */
        pNetif->mld_mac_filter(pNetif, &Ipv6LinkLocalNodes, NETIF_ADD_MAC_FILTER);
#endif /* LWIP_IPV6_MLD */
    }
#endif /* LWIP_IPV6 */

    /* Set the linkoutput callback which will pass the final buffer to the driver to be transmitted. */
    pNetif->linkoutput = ComDal_Eth_WriteFrame;
#if LWIP_NETIF_HOSTNAME
    if (NULL != pEthInterface->pHostname)
    {
        pNetif->hostname = pEthInterface->pHostname;
    }
    else
    {
        Status = ERR_VAL;
    }
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_ARP
    /* Enable automatic ARP response with IP to MAC mapping . */
    if ((FALSE == pEthInterface->bDhcpEnabled) && (FALSE == pEthInterface->bAutoIpEnabled))
    {
        ComDal_Eth_DrvIf_SetArpOffloading(pNetif->num, pEthInterface->IpAddress.ip.u8Ipv4, TRUE);
    }
#endif /* LWIP_ARP */

    netif_set_link_up(pNetif);

    return Status;
}

#if (LWIP_IGMP) && (LWIP_IPV4)
/*=================================================================================================================================*/
/**
* @brief          Notify via the driver interface to start or stop forwarding multicast packets for an IgmpGroup
* @details        If the action is add, the driver will forward the multicast packets corresponding to the given IgmpGroup.
*                 If the action is remove, the driver will stop forwarding the multicast packets for the given IgmpGroup.
*
* @param[in]      pNetif                    Pointer to the netif structure
* @param[in]      pIgmpGroup                Pointer to the IP address corresponding to the multicast group
* @param[in]      Action                    The action to be done for the multicast group
*
* @return         The error status:         ERR_OK, ERR_VAL
*/
/*=================================================================================================================================*/
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_IgmpGroupFilterCallback
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2CONST(ComDal_Eth_Ipv4AddressType, AUTOMATIC, ETH_APPL_CONST) pIgmpGroup,
    VAR(ComDal_Eth_MacFilterActionType, AUTOMATIC) eAction
)
{
    /* Generate the group multicast MAC address based on the IPv4 address */
    VAR(uint8, AUTOMATIC) aMulticastMac[6] = {
        LL_IP4_MULTICAST_ADDR_0,
        LL_IP4_MULTICAST_ADDR_1,
        LL_IP4_MULTICAST_ADDR_2,
        /* Only 23 bits are available for mapping, take only the first 7 least-significant bits out of the second IP address byte. */
        (0x7F & ip4_addr2(pIgmpGroup)),
        ip4_addr3(pIgmpGroup),
        ip4_addr4(pIgmpGroup)
    };
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_OK;

    if ((NULL_PTR == pNetif) || (NULL_PTR == pIgmpGroup))
    {
        Status = ERR_VAL;
    }

    if (NETIF_ADD_MAC_FILTER == eAction)
    {
        ComDal_Eth_DrvIf_AddToHashFilter(pNetif->num, aMulticastMac);
    }
    else
    {
        ComDal_Eth_DrvIf_RemoveFromHashFilter(pNetif->num, aMulticastMac);
    }

    return Status;
}
#endif /* (LWIP_IGMP) && (LWIP_IPV4) */

#if (LWIP_IPV6) && (LWIP_IPV6_MLD)
/*=================================================================================================================================*/
/**
* @brief          Notify via the driver interface to start or stop forwarding multicast packets for a MldGroup
* @details        If the action is add, the driver will forward the multicast packets corresponding to the given MldGroup (multicast address).
*                 If the action is remove, the driver will stop forwarding the multicast packets for the given MldGroup (multicast address).
*
* @param[in]      pNetif                       Pointer to the netif structure
* @param[in]      pMldGroup                    Pointer to the IP address corresponding to the multicast group
* @param[in]      Action                       The action to be done for the multicast group
*
* @return         The error status:            ERR_OK, ERR_VAL
*/
/*=================================================================================================================================*/
static FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_MldGroupFilterCallback
(
    P2VAR(ComDal_Eth_NetifType, AUTOMATIC, ETH_APPL_DATA) pNetif,
    P2CONST(ComDal_Eth_Ipv6AddressType, AUTOMATIC, ETH_APPL_CONST) pMldGroup,
    VAR(ComDal_Eth_MacFilterActionType, AUTOMATIC) eAction
)
{
    /* Generate the group multicast MAC address based on the IPv6 address. */
    VAR(uint8, AUTOMATIC) aMulticastMac[6] = {
        LL_IP6_MULTICAST_ADDR_0,
        LL_IP6_MULTICAST_ADDR_1,
        (IP6_ADDR_BLOCK7(pMldGroup) >> 8) & 0xFF,
        IP6_ADDR_BLOCK7(pMldGroup) & 0xFF,
        (IP6_ADDR_BLOCK8(pMldGroup) >> 8) & 0xFF,
        IP6_ADDR_BLOCK8(pMldGroup) & 0xFF,
    };
    VAR(ComDal_Eth_TcpipReturnType, AUTOMATIC) Status = ERR_OK;

    if ((NULL_PTR == pNetif) || (NULL_PTR == pMldGroup))
    {
        Status = ERR_VAL;
    }

    if (NETIF_ADD_MAC_FILTER == eAction)
    {
        ComDal_Eth_DrvIf_AddToHashFilter(pNetif->num, aMulticastMac);
    }
    else
    {
        ComDal_Eth_DrvIf_RemoveFromHashFilter(pNetif->num, aMulticastMac);
    }

    return Status;
}
#endif /* (LWIP_IPV6) && (LWIP_IPV6_MLD) */
/**
 * @brief Callback function for incoming connections
 * @param[in] arg Argument passed to the callback function
 * @param[in] newPcb PCB structure for the new connection
 * @param[in] err Error code
 * @return Error code
 */
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerAccept
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) newPcb,
    VAR(err_t, AUTOMATIC) err
)
{
    P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, ETH_APPL_DATA) pTcpSvr;
    VAR(uint8, AUTOMATIC) u8Idx;
    VAR(err_t, AUTOMATIC) eRetVal = ERR_MEM;

    pTcpSvr = (P2VAR(ComDal_Eth_TcpSvrConfType, AUTOMATIC, ETH_APPL_DATA))arg;
    
    if((ERR_OK == err) &&
        (NULL != pTcpSvr) &&
        (NULL != pTcpSvr->pcb))
    {
        for(u8Idx = 0; u8Idx < pTcpSvr->u8NoOfTcpClSupported; u8Idx++)
        {
            if(NULL == pTcpSvr->pTcpClients[u8Idx].pcb)
            {
                pTcpSvr->pTcpClients[u8Idx].pcb = newPcb;
                pTcpSvr->pTcpClients[u8Idx].u16RxBuffLen = 0u;
                pTcpSvr->pTcpClients[u8Idx].u16TxBuffLen = 0u;
                /* Set up the TCP callback functions for the new connection */
                tcp_arg(newPcb, &pTcpSvr->pTcpClients[u8Idx]);
                tcp_recv(newPcb, ComDal_Eth_tcpServerRecv);
                tcp_err(newPcb, ComDal_Eth_tcpServerError);
                tcp_poll(newPcb, ComDal_Eth_tcpServerPoll, 1);
                tcp_sent(newPcb, ComDal_Eth_tcpServerSent);
                eRetVal = ERR_OK;
                break;
            }
        }
    }
    return eRetVal;
}

/**
 * @brief Callback function for received data
 * @param[in] arg Argument passed to the callback function
 * @param[in] tpcb PCB structure for the connection
 * @param[in] p PBUF structure for the received data
 * @param[in] err Error code
 * @return Error code
 */
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerRecv
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb,
    P2VAR(ComDal_Eth_PacketBufferType, AUTOMATIC, ETH_APPL_DATA) p,
    VAR(err_t, AUTOMATIC) err
)
{
    P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA) pCl;

    pCl = (P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA))arg;

    if((ERR_OK == err) &&
        (NULL != pCl)&&
        (NULL != p))
    {
        MemLib_MemCpy(pCl->pRxBuff, p->payload, p->len);
        pCl->u16RxBuffLen = p->len;
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
    }
    else
    {
        if(NULL != p)
        {
            pbuf_free(p);
        }
        (void)tcp_close(tpcb);
        if(NULL != pCl)
        {
            pCl->pcb = NULL;
        }
    }
    return ERR_OK;
}

/**
 * @brief Callback function for errors
 * @param[in] arg Argument passed to the callback function
 * @param[in] err Error code
 */
static FUNC(void, AUTOMATIC)  ComDal_Eth_tcpServerError
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    VAR(err_t, AUTOMATIC) err
)
{
    P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA) pCl;
    pCl = (P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA))arg;
    if((NULL != pCl) &&
        ((ERR_CLSD == err) ||
        (ERR_RST == err) ||
        (ERR_ABRT == err)))
    {
        tcp_close(pCl->pcb);
        pCl->pcb = NULL;
        pCl->u16RxBuffLen = 0u;
        pCl->u16TxBuffLen = 0u;
    }
}

/**
 * @brief Callback function for connection polling
 * @param[in] arg Argument passed to the callback function
 * @param[in] tpcb PCB structure for the connection
 * @return Error code
 */
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerPoll
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb
)
{
    P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA) pCl;
    VAR(err_t, AUTOMATIC) eRetVal = ERR_OK;

    pCl = (P2VAR(ComDal_Eth_ClientType, AUTOMATIC, ETH_APPL_DATA))arg;
    if(NULL != pCl)
    {
        if(0u < pCl->u16TxBuffLen )
        {
            eRetVal = tcp_write(tpcb, pCl->pTxBuff, pCl->u16TxBuffLen, TCP_WRITE_FLAG_COPY);
            if(ERR_OK == eRetVal)
            {
                pCl->u16TxBuffLen = 0u;
            }
        }
    }
    return eRetVal;
}

/**
 * @brief Callback function for sent data
 * @param[in] arg Argument passed to the callback function
 * @param[in] tpcb PCB structure for the connection
 * @param[in] len Length of the data sent
 * @return Error code
 */
static FUNC(err_t, AUTOMATIC) ComDal_Eth_tcpServerSent
(
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) arg,
    P2VAR(ComDal_Eth_TcpControlBlockType, AUTOMATIC, ETH_APPL_DATA) tpcb,
    VAR(uint16, AUTOMATIC) len
)
{
    (void)arg;
    (void)tpcb;
    (void)len;
    /* Do something with the sent data here */

    return ERR_OK;
}
/*=================================================================================================================================*/
#endif /* (COMDAL_MAX_ETH_INTERFACES > 0U) */

#ifdef __cplusplus
}
#endif
/** @} */
