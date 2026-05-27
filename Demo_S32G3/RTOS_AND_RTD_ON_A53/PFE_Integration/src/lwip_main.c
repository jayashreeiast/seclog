/*
 *  Copyright 2023-2025 NXP
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

#include <stdio.h>
#include <string.h>
#include "lwip_demo.h"

#undef UINT_MAX
#include "Eth_43_PFE.h"

#define NETIF_NUMBER_TO_CONFIG ((ETH_43_PFE_NUM_CONTROLLER_CFG > PFE_NETIF_NUMBER) \
                                ? PFE_NETIF_NUMBER \
                                : ETH_43_PFE_NUM_CONTROLLER_CFG \
                               )

struct netif network_interfaces[ETH_43_PFE_NUM_CONTROLLER_CFG];
struct netif *g_pfe_netifs[PFE_NETIF_NUMBER];

extern uint32 _end;
extern uint32 _heapStart;
extern uint32 ipv4_string_to_int(const char *p_ip_addr);

/* Array of netif configurations */
netif_custom_t netif_cfg[PFE_NETIF_NUMBER] =
{
    {
        .has_dhcp    = FALSE,
        .has_auto_ip = FALSE,
        .has_IPv6    = FALSE,
        .default_netif = FALSE,
        .ip_addr     = IPV4_LOCAL_IP_ADDR_0,
        .netmask     = LWIP_MASK,
        .gw          = LWIP_DEFAULT_GATEWAY_0,
        .hostname    = "ctrl0",
        .name        = { 'e', '0' },
    },
    {
        .has_dhcp    = FALSE,
        .has_auto_ip = FALSE,
        .has_IPv6    = FALSE,
        .default_netif = FALSE,
        .ip_addr     = IPV4_LOCAL_IP_ADDR_1,
        .netmask     = LWIP_MASK,
        .gw          = LWIP_DEFAULT_GATEWAY_1,
        .hostname    = "ctrl1",
        .name        = { 'e', '1' },
    },
    {
        .has_dhcp    = FALSE,
        .has_auto_ip = FALSE,
        .has_IPv6    = FALSE,
        .default_netif = TRUE, /* Lets make the AUX default */
        .ip_addr     = IPV4_LOCAL_IP_ADDR_2,
        .netmask     = LWIP_MASK,
        .gw          = LWIP_DEFAULT_GATEWAY_2,
        .hostname    = "ctrl2",
        .name        = { 'e', '2' },
    }
};

/**
* @brief          This function handles the received Ethernet frame.
* @details        Function should parse the received frame and pass the gathered
*                 information to the appropriate upper layer module.
* @note           The passed data buffer is no longer valid after the function
*                 is exited.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which received the frame.
* @param[in]      FrameType The received frame Ethertype (from the frame header)
* @param[in]      IsBroadcast The value TRUE indicates that the received frame
*                 was sent to broadcast address (ff-ff-ff-ff-ff-ff)
* @param[in]      PhysAddrPtr Pointer to received frame source MAC address
*                 (6 bytes).
* @param[in]      DataPtr Data buffer containing the received Ethernet frame
*                 payload.
* @param[in]      LenByte Length of the data in the buffer DataPtr.
*
*/

FUNC(void, ETHIF_CODE)Lwip_EthIf_RxIndication(VAR(uint8, AUTOMATIC) CtrlIdx, \
                             VAR(Eth_FrameType, AUTOMATIC) FrameType, \
                             VAR(boolean, AUTOMATIC) IsBroadcast, \
                             P2VAR(uint8, AUTOMATIC, AUTOMATIC) PhysAddrPtr, \
                             P2VAR(Eth_DataType, AUTOMATIC, AUTOMATIC) DataPtr, \
                             VAR(uint16, AUTOMATIC) LenByte)
{
    struct pbuf *p;
    struct netif *netif;

    if (CtrlIdx >= PFE_NETIF_NUMBER || !g_pfe_netifs[CtrlIdx])
        return;
    netif = g_pfe_netifs[CtrlIdx];

    (void)FrameType;
    (void)IsBroadcast;
    (void)PhysAddrPtr;

    LenByte += 14;
    DataPtr -= 14;

    p = pbuf_alloc(PBUF_RAW, LenByte, PBUF_POOL);
    if (!p)
        return;

    p->if_idx = netif->num;
    pbuf_take(p, DataPtr, LenByte);

    if (ERR_OK != netif->input(p, netif))
    {
        (void)pbuf_free(p);
#ifdef NXP_LOG_ENABLED
        NXP_LOG_RAW_ERROR("netif input fail\r\n");
#endif
    }
}

void * _sbrk (ptrdiff_t __incr)
{
    static uint32 *heap = NULL;
    uint32 *prev_heap;
    static uint32 *HeapEndPtr = (uint32 *)&_heapStart;
    void * Status;

    if (heap == NULL)
    {
        heap = (uint32 *)&_end;
    }
    prev_heap = heap;

    if (((heap + __incr) <= HeapEndPtr) && (prev_heap != NULL))
    {
        heap += __incr;
        Status = (void *) ((void *)prev_heap);
    }
    else
    {
        Status = (void *) -1;
#ifdef NXP_LOG_ENABLED
        NXP_LOG_RAW_ERROR("Memory allocation failed");
#endif
    }

    return Status;
}

static err_t pfe_netif_output(struct netif *netif, struct pbuf *p)
{
    int ctrl_idx = netif->name[1] - '0';
    Std_ReturnType		tempRet 	= E_NOT_OK;
    Eth_DataType		*bufPtr;
    Eth_BufIdxType		BufferIndex;
    uint16				LengthInBytes;
    BufReq_ReturnType	ProvideTxBufferResult;
    uint16 eth_type;

    LWIP_ASSERT("Output packet buffer empty", p);
#if defined(LWIP_DEBUG) && LWIP_NETIF_TX_SINGLE_PBUF && !(LWIP_IPV4 && IP_FRAG) && (LWIP_IPV6 && LWIP_IPV6_FRAG)
    LWIP_ASSERT("p->next == NULL && p->len == p->tot_len", p->next == NULL && p->len == p->tot_len);
#endif

    LINK_STATS_INC(link.xmit);

    /* Update SNMP stats (only if you use SNMP) */
    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);

    const int unicast = ((((uint8*)p->payload)[0] & 0x01) == 0);

    if (unicast)
    {
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
    else
    {
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    }

    LengthInBytes = p->tot_len - 14;
    ProvideTxBufferResult = Eth_43_PFE_ProvideTxBuffer(ctrl_idx, 0U, &BufferIndex, &bufPtr, &LengthInBytes);
    if (ProvideTxBufferResult != BUFREQ_OK)
        return ERR_MEM;

    /* Write frame data */
    pbuf_copy_partial(p, bufPtr, p->tot_len - 14, 14);
    eth_type = (*((uint8*)p->payload+12) << 8) | *((uint8*)p->payload+13);
    tempRet = Eth_43_PFE_Transmit(ctrl_idx, BufferIndex, eth_type, TRUE, p->tot_len - 14, p->payload);
    if (E_OK != tempRet)
    {
        return ERR_MEM;
    }
    else
    {
        Eth_43_PFE_TxConfirmation(ctrl_idx);
    }

    return ERR_OK;
}

#ifdef NXP_LOG_ENABLED
#if LWIP_NETIF_STATUS_CALLBACK
static void netif_status_callback(struct netif *netif)
{
     NXP_LOG_INFO("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}
#endif

static void lwiperf_status_report(void *arg, enum lwiperf_report_type report_type, const ip_addr_t *local_addr, u16_t local_port, const ip_addr_t *remote_addr, u16_t remote_port, u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
    (void)arg;
    char report[20];

    switch (report_type)
    {
        case LWIPERF_TCP_DONE_SERVER:
        case LWIPERF_TCP_DONE_CLIENT:
            strcpy(report,"Test success.");
            break;
        case LWIPERF_TCP_ABORTED_LOCAL:
            strcpy(report,"Local error.");
            break;
        case LWIPERF_TCP_ABORTED_LOCAL_DATAERROR:
            strcpy(report,"Data check error.");
            break;
        case LWIPERF_TCP_ABORTED_LOCAL_TXERROR:
            strcpy(report,"Transmit error.");
            break;
        case LWIPERF_TCP_ABORTED_REMOTE:
            strcpy(report,"Test aborted.");
            break;
        default:
            strcpy(report,"Unknown error.");
            break;
    }

    NXP_LOG_INFO("Report:%s Local adress:%s Local port:%hu Remote adress:%s Remote port:%d Bytes transferred:%lu Duration in ms:%lu Bandwith in kb/s:%lu\n",report, ip4addr_ntoa(local_addr) ,local_port ,ip4addr_ntoa(remote_addr), remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec);
}
#endif

static err_t pfe_netif_init(struct netif *netif)
{
    uint8 hwaddr[NETIF_MAX_HWADDR_LEN];
    uint8 ctrl_idx = netif->name[1] - '0';

    for (uint8 j = 0U; j < NETIF_MAX_HWADDR_LEN; j++)
    {
        hwaddr[j] = Eth_43_PFE_InternalCfgPtr->pController[ctrl_idx]->EthMACAddress[j];
    }

    if (ctrl_idx >= PFE_NETIF_NUMBER)
        return ERR_ARG;

    if (g_pfe_netifs[ctrl_idx])
        return ERR_ALREADY;
    g_pfe_netifs[ctrl_idx] = netif;

    netif->linkoutput = pfe_netif_output;
#if LWIP_IPV4
    netif->output     = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->mtu        = 1500;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);

    SMEMCPY(netif->hwaddr, hwaddr, ETH_HWADDR_LEN);
    netif->hwaddr_len = ETH_HWADDR_LEN;

    return ERR_OK;
}

int demo_lwip_init(void)
{
    uint32 netmask_num = lwip_htonl(ipv4_string_to_int(LWIP_MASK));
    uint8 i;
    struct netif *netif;
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
#endif /* LWIP_IPV4 */

    lwip_init();

    for (i = 0U; i < NETIF_NUMBER_TO_CONFIG; i++)
    {
        netif = &network_interfaces[i];
#if LWIP_IPV4
        ip4_addr_set_zero(&gw);
        ip4_addr_set_zero(&ipaddr);
        ip4_addr_set_zero(&netmask);
        if ((!netif_cfg[i].has_dhcp) && (!netif_cfg[i].has_auto_ip))
        {
            ip4_addr_set_u32((&gw), lwip_htonl(ipv4_string_to_int(netif_cfg[i].gw)));
            ip4_addr_set_u32((&ipaddr), lwip_htonl(ipv4_string_to_int(netif_cfg[i].ip_addr)));
            ip4_addr_set_u32((&netmask), netmask_num);
        }
#endif /* LWIP_IPV4 */
        for (uint8 j = 0U; j < 2U; j++)
        {
            netif->name[j] = netif_cfg[i].name[j];
        }
#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        if(NULL != netif_cfg[i]->hostname)
        {
            netif->hostname = netif_cfg[i]->hostname;
        }
#endif /* LWIP_NETIF_HOSTNAME */

        netif = netif_add(netif,
#if LWIP_IPV4
        &ipaddr, &netmask, &gw,
#endif
        NULL, pfe_netif_init, netif_input);
        if (!netif)
        {
            continue;
        }

#if LWIP_IPV6_AUTOCONFIG
        if (netif_cfg[i].has_IPv6)
        {
            netif_create_ip6_linklocal_address(netif, 1);
            netif->ip6_autoconfig_enabled = 1;
        }
#endif

#if LWIP_NETIF_STATUS_CALLBACK
        netif_set_status_callback(netif, netif_status_callback);
#endif

        if (netif_cfg[i].default_netif)
            netif_set_default(netif);

        netif_set_up(netif);
#if LWIP_DHCP
        /* Start DHCP and HTTPD */
        if (netif_cfg[i].has_dhcp)
            dhcp_start(netif);
#endif
        netif_set_link_up(&network_interfaces[i]);
    }
    lwiperf_report_fn report_function = NULL;
#ifdef NXP_LOG_ENABLED
    report_function = &lwiperf_status_report;
#endif
    lwiperf_start_tcp_server_default(report_function,NULL);

    httpd_init();

    return 0;
}

void demo_lwip_main(void)
{
    /* Cyclic lwIP timers check */
    sys_check_timeouts();
}