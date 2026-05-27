/*
 *  (c) Copyright 2006-2016 Freescale Semiconductor, Inc.
 *  Copyright 2017, 2019-2025 NXP
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

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "sample_app_fci.h"

#ifdef PFE_CFG_FCI_ENABLE

#if (defined(FCI_L2BR_TEST) && defined(FCI_RTABLE_TEST))
    #error Only one of the FCI test macros can be defined (FCI_L2BR_TEST or FCI_RTABLE_TEST)
#endif

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#if defined(FCI_L2BR_TEST)
    #define VLAN_USED 1U
#endif /* defined(FCI_L2BR_TEST)*/

#if defined(TARGET_EVB) || defined(TARGET_EVB3)
#define EMACA_ID_STRING "emac0"
#define EMACB_ID_STRING "emac1"
#elif defined(TARGET_RDB2) || defined(TARGET_RDB3)
#define EMACA_ID_STRING "emac1"
#define EMACB_ID_STRING "emac2"
#else
#error "Unsupported target"
#endif

#if defined (PFE_CFG_PFE_MASTER)
    #define HIF_ID_STRING "hif0"
#else
    #define HIF_ID_STRING "hif1"
#endif /* defined (PFE_CFG_PFE_MASTER) */

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/


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
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#if (defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST))
    static void sample_fci_log_if_print_all(FCI_CLIENT *cl);
    static void sample_fci_phy_if_print_all(FCI_CLIENT *cl);
    static void sample_fci_phy_if_enable(FCI_CLIENT *cl, char *name);
    static void sample_fci_phy_if_set_mode(FCI_CLIENT *cl, char *name, fpp_phy_if_op_mode_t mode);
    static int sample_fci_phy_if_get_by_name(FCI_CLIENT *cl, char *name, fpp_phy_if_cmd_t *phy_if);
#endif /* defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST) */

#if defined(FCI_L2BR_TEST)
    static int sample_fci_phy_if_get_id(FCI_CLIENT *cl, char *name);
    static void sample_fci_phy_if_promisc_on(FCI_CLIENT *cl, char *name);
    static int sample_fci_bd_create(FCI_CLIENT *cl, uint16 vlan);
    static int sample_fci_bd_get(FCI_CLIENT *cl, uint16 vlan, fpp_l2_bd_cmd_t *bd);
    static int sample_fci_bd_add_if(FCI_CLIENT *cl, uint16 vlan, char *phy_if, bool_t tag);
    static int sample_fci_bd_set_policy(FCI_CLIENT *cl, uint16 vlan, uint8 uh, uint8 um, uint8 mh, uint8 mm);
#endif /* defined(FCI_L2BR_TEST)*/

#if defined(FCI_RTABLE_TEST)
    static void sample_fci_router_reset(FCI_CLIENT *cl);
    static void sample_fci_router_register_ipv4_routes(FCI_CLIENT *cl);
    static void sample_fci_router_register_bd_ipv4_conntrack(FCI_CLIENT *cl);
    static void sample_fci_setup_ipv4_router_bd(FCI_CLIENT *cl);
    static void sample_fci_router_print_ipv4_routes(FCI_CLIENT *cl);
#endif /* defined(FCI_RTABLE_TEST)*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
#if (defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST))
static fci_cb_retval_t sample_fci_callback(unsigned short fcode, unsigned short len, unsigned short *payload)
{
    (void)fcode;
    (void)len;
    (void)payload;

    return FCI_CB_CONTINUE;
}

/*
 *    @brief        Print all logical interfaces
 *    @param[in]    cl The FCI client instance
 */
static void sample_fci_log_if_print_all(FCI_CLIENT *cl)
{
    fpp_log_if_cmd_t rep, cmd;
    unsigned short replen;
    int ret;

    /* Get exclusive access to interfaces */
    if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR)))
    {
        NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed: %d\n", ret);
    }
    else
    {
        /* Get all interfaces */
        cmd.action = FPP_ACTION_QUERY;
        ret = fci_query(cl, FPP_CMD_LOG_IF, sizeof(cmd), (unsigned short *)&cmd,
                    &replen, (unsigned short *)&rep);
        while (FPP_ERR_OK == ret)
        {
            NXP_LOG_INFO("%02d %s (%-5s): Flags: 0x%04x, Egress: 0x%08x, MatchRules: 0x%08x\n",
                    (int_t)oal_ntohl(rep.id), rep.name, rep.parent_name,
                    (uint_t)oal_ntohl(rep.flags), (uint_t)oal_ntohl(rep.egress),
                    (uint_t)oal_ntohl(rep.match));
            cmd.action = FPP_ACTION_QUERY_CONT;
            ret = fci_query(cl, FPP_CMD_LOG_IF, sizeof(cmd), (unsigned short *)&cmd,
                        &replen, (unsigned short *)&rep);
        }

        /* Unlock interfaces */
        if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR)))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed: %d\n", ret);
        }
    }
}

/*
 * @brief        Print all physical interfaces
 * @param[in]    The FCI client instance
 */
static void sample_fci_phy_if_print_all(FCI_CLIENT *cl)
{
    fpp_phy_if_cmd_t rep = {0}, cmd = {0};
    unsigned short replen;
    int ret;

    /* Get exclusive access to interfaces */
    if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR)))
    {
        NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed: %d\n", ret);
    }
    else
    {
        /* Get all interfaces */
        cmd.action = FPP_ACTION_QUERY;
        ret = fci_query(cl, FPP_CMD_PHY_IF, sizeof(cmd), (unsigned short *)&cmd,
                    &replen, (unsigned short *)&rep);
        while (FPP_ERR_OK == ret)
        {
            NXP_LOG_INFO("%02d %-5s: Mode: 0x%x, Flags: 0x%04x, Filter: %s\n",
                    (int_t)oal_ntohl(rep.id), rep.name, (uint_t)rep.mode,
                    (uint_t)oal_ntohl(rep.flags),
                    (0 == autolibc_strlen(rep.ftable)) ? "Disabled" : rep.ftable);
            cmd.action = FPP_ACTION_QUERY_CONT;
            ret = fci_query(cl, FPP_CMD_PHY_IF, sizeof(cmd), (unsigned short *)&cmd,
                        &replen, (unsigned short *)&rep);
        }
    }

    /* Unlock interfaces */
    if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR)))
    {
        NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed: %d\n", ret);
    }
}

/*
 * @brief        Get QUERY response by interface name
 * @param[in]    cl The FCI client instance
 * @param[in]    name Physical interface name
 * @param[out]    phy_if Pointer where the response shall be written
 * @return        1 if success, zero otherwise
 */
static int sample_fci_phy_if_get_by_name(FCI_CLIENT *cl, char *name, fpp_phy_if_cmd_t *phy_if)
{
    fpp_phy_if_cmd_t rep = {0}, cmd = {0};
    unsigned short replen;
    int ret, retval = 0;

    /*    Get exclusive access to interfaces */
    if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR)))
    {
        NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed: %d\n", ret);
    }
    else
    {
        /*    Get all interfaces */
        cmd.action = FPP_ACTION_QUERY;
        ret = fci_query(cl, FPP_CMD_PHY_IF, sizeof(cmd), (unsigned short *)&cmd,
                    &replen, (unsigned short *)&rep);
        while (FPP_ERR_OK == ret)
        {
            if (0 == autolibc_strcmp(name, rep.name))
            {
                autolibc_memcpy(phy_if, &rep, sizeof(*phy_if));
                retval = 1;
                break;
            }
            else
            {
                cmd.action = FPP_ACTION_QUERY_CONT;
                ret = fci_query(cl, FPP_CMD_PHY_IF, sizeof(cmd), (unsigned short *)&cmd,
                            &replen, (unsigned short *)&rep);
            }
        }

        /*    Unlock interfaces */
        if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR)))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed: %d\n", ret);
        }
    }

    return retval;
}

/*
 * @brief        Enable physical interface
 * @param[in]    cl The FCI client instance
 * @param[in]    name Physical interface name
 */
static void sample_fci_phy_if_enable(FCI_CLIENT *cl, char *name)
{
    fpp_phy_if_cmd_t cmd;
    int ret;

    if (sample_fci_phy_if_get_by_name(cl, name, &cmd))
    {
        /*    Get exclusive access to interfaces */
        if (FPP_ERR_OK != fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed\n");
        }
        else
        {
            cmd.action = FPP_ACTION_UPDATE;
            cmd.flags |= oal_htonl(FPP_IF_ENABLED);
            if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_PHY_IF, sizeof(cmd),
                                        (unsigned short *)&cmd)))
            {
                NXP_LOG_ERROR("%s enable failed: %d\n", name, ret);
            }
        }

        /*    Unlock interfaces */
        if (FPP_ERR_OK != fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed\n");
        }
    }
    else
    {
        NXP_LOG_ERROR("%s not found\n", name);
    }
}

/*
 * @brief        Change mode and enable physical interface
 * @param[in]    cl The FCI client instance
 * @param[in]    id Physical interface ID
 * @param[in]    mode New operation mode to be set
 */
static void sample_fci_phy_if_set_mode(FCI_CLIENT *cl, char *name, fpp_phy_if_op_mode_t mode)
{
    fpp_phy_if_cmd_t rep;
    int ret;

    /*    Get interface reply by name */
    if (sample_fci_phy_if_get_by_name(cl, name, &rep))
    {
        /*    Get exclusive access to interfaces */
        if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR)))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed: %d\n", ret);
        }
        else
        {
            /*    Change the mode */
            rep.action = FPP_ACTION_UPDATE;
            rep.mode = mode;
            if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_PHY_IF,
                                        sizeof(rep), (unsigned short *)&rep)))
            {
                NXP_LOG_ERROR("Mode change failed: %d\n", ret);
            }

            /*    Unlock interfaces */
            if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR)))
            {
                NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed: %d\n", ret);
            }
        }
    }
    else
    {
        NXP_LOG_ERROR("%s not found\n", name);
    }
}
#endif /* defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST) */

#if defined(FCI_L2BR_TEST)
/*
 * @brief        Get physical interface ID by name
 * @param[in]    The FCI client instance
 * @param[in]    name Name of physical interface
 * @return        Physical interface ID (in host byte order) or -1 if failed.
 */
static int sample_fci_phy_if_get_id(FCI_CLIENT *cl, char *name)
{
    fpp_phy_if_cmd_t rep = {0};
    int ret;

    /*    Get reply data by name */
    if (sample_fci_phy_if_get_by_name(cl, name, &rep))
    {
        ret = oal_ntohl(rep.id);
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/*
 * @brief        Turn on promiscuous mode on physical interface
 * @param[in]    cl The FCI client instance
 * @param[in]    name Physical interface name
 */
static void sample_fci_phy_if_promisc_on(FCI_CLIENT *cl, char *name)
{
    fpp_phy_if_cmd_t cmd;
    int ret;

    if (sample_fci_phy_if_get_by_name(cl, name, &cmd))
    {
        /*    Get exclusive access to interfaces */
        if (FPP_ERR_OK != fci_write(cl, FPP_CMD_IF_LOCK_SESSION, 0, NULL_PTR))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_LOCK_SESSION failed\n");
        }
        else
        {
            cmd.action = FPP_ACTION_UPDATE;
            cmd.flags |= oal_htonl(FPP_IF_PROMISC);
            if (FPP_ERR_OK != (ret = fci_write(cl, FPP_CMD_PHY_IF, sizeof(cmd),
                                        (unsigned short *)&cmd)))
            {
                NXP_LOG_ERROR("%s promisc mode failed: %d\n", name, ret);
            }
        }

        /*    Unlock interfaces */
        if (FPP_ERR_OK != fci_write(cl, FPP_CMD_IF_UNLOCK_SESSION, 0, NULL_PTR))
        {
            NXP_LOG_ERROR("FPP_CMD_IF_UNLOCK_SESSION failed\n");
        }
    }
    else
    {
        NXP_LOG_ERROR("%s not found\n", name);
    }
}

/*
 * @brief        Create bridge domain
 * @param[in]    cl The FCI client instance
 * @param[in]    vlan The VLAN ID identifying the domain
 * @return        1 if success, zero otherwise
 */
static int sample_fci_bd_create(FCI_CLIENT *cl, uint16 vlan)
{
    fpp_l2_bd_cmd_t cmd;
    int ret;

    cmd.action = FPP_ACTION_REGISTER;
    cmd.vlan = oal_htons(vlan);

    /*    Default policy is to discard all */
    cmd.ucast_hit = 3;
    cmd.ucast_miss = 3;
    cmd.mcast_hit = 3;
    cmd.mcast_miss = 3;

    ret = fci_write(cl, FPP_CMD_L2_BD, sizeof(cmd), (unsigned short *)&cmd);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_L2_BD failed: %d\n", ret);
        return 0;
    }

    return 1;
}

/*
 * @brief        Get bridge domain
 * @param[in]    cl The FCI client instance
 * @param[in]    vlan The VLAN ID identifying the domain
 * @param[out]    bd Pointer where response shall be written
 * @return        1 if success, zero otherwise
 */
static int sample_fci_bd_get(FCI_CLIENT *cl, uint16 vlan, fpp_l2_bd_cmd_t *bd)
{
    fpp_l2_bd_cmd_t cmd;
    unsigned short replen = sizeof(*bd);
    int ret;

    cmd.action = FPP_ACTION_QUERY;

    ret = fci_query(cl, FPP_CMD_L2_BD, sizeof(cmd), (unsigned short *)&cmd,
            &replen, (unsigned short *)bd);
    while (FPP_ERR_OK == ret)
    {
        if (vlan == oal_ntohs(bd->vlan))
        {
            return 1;
        }
        else
        {
            cmd.action = FPP_ACTION_QUERY_CONT;
            ret = fci_query(cl, FPP_CMD_L2_BD, sizeof(cmd), (unsigned short *)&cmd,
                        &replen, (unsigned short *)bd);
        }
    }

    return 0;
}

/*
 * @brief        Add interface to a bridge domain
 * @param[in]    cl The FCI client instance
 * @param[in]    vlan The VLAN ID identifying the domain
 * @param[in]    phy_if Name of physical interface to be added
 * @param[in]    tag If TRUE then in case of VLAN bridge the traffic leaving
 *                     the interface "phy_if" will be VLAN tagged. If FALSE
 *                     then in case of VLAN bridge the traffic leaving the interface
 *                     "phy_if" will be untagged (VLAN tag will be stripped).
 * @return        1 if success, zero otherwise
 */
static int sample_fci_bd_add_if(FCI_CLIENT *cl, uint16 vlan, char *phy_if, bool_t tag)
{
    int if_id = sample_fci_phy_if_get_id(cl, phy_if);
    int ret;
    fpp_l2_bd_cmd_t bd;

    if (if_id < 0)
    {
        NXP_LOG_ERROR("Could not get interface ID\n");
        return 0;
    }

    if (0 == sample_fci_bd_get(cl, vlan, &bd))
    {
        NXP_LOG_ERROR("Can't get bridge domain\n");
        return 0;
    }

    bd.action = FPP_ACTION_UPDATE;
    bd.if_list |= oal_htonl(1 << if_id);

    if (tag)
    {
        bd.untag_if_list &= ~oal_htonl(1 << if_id);
    }
    else
    {
        bd.untag_if_list |= oal_htonl(1 << if_id);
    }

    ret = fci_write(cl, FPP_CMD_L2_BD, sizeof(bd), (unsigned short *)&bd);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_L2_BD failed: %d\n", ret);
        return 0;
    }

    return 1;
}

/*
 * @brief        Set bridge domain policy
 * @details        It is possible to configure action for various conditions
 *                 related to the bridge domain. Available actions are:
 *                 0 - Forward, 1 - Flood, 2 - Punt, 3 - Discard.
 * @param[in]    cl The FCI client instance
 * @param[in]    vlan The VLAN ID identifying the domain
 * @param[in]    uh Unicast-Hit action. MAC table hit (unicast address).
 * @param[in]    um Unicase-Miss action. MAC table miss (unicast address).
 * @param[in]    mh Multicast-Hit action. MAC table hit (multicast address).
 * @param[in]    mm Multicast-Miss action. MAC table miss (multicast address).
 * @return        1 if success, zero otherwise
 */
static int sample_fci_bd_set_policy(FCI_CLIENT *cl, uint16 vlan, uint8 uh, uint8 um, uint8 mh, uint8 mm)
{
    int ret;
    fpp_l2_bd_cmd_t bd;

    if (0 == sample_fci_bd_get(cl, vlan, &bd))
    {
        NXP_LOG_ERROR("Can't get bridge domain\n");
        return 0;
    }

    bd.action = FPP_ACTION_UPDATE;
    bd.ucast_hit = uh;
    bd.ucast_miss = um;
    bd.mcast_hit = mh;
    bd.mcast_miss = mm;

    ret = fci_write(cl, FPP_CMD_L2_BD, sizeof(bd), (unsigned short *)&bd);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_L2_BD failed: %d\n", ret);
        return 0;
    }

    return 1;
}
#endif /* defined(FCI_L2BR_TEST)*/

#if defined(FCI_RTABLE_TEST)
/*
 * @brief        Reset IPv4 and IPv6 router
 * @param[in]    cl The FCI client instance
 */
static void sample_fci_router_reset(FCI_CLIENT *cl)
{
    int ret;

    ret = fci_write(cl, FPP_CMD_IPV4_RESET, 0, NULL_PTR);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_IPV4_RESET failed: %d\n", ret);
    }

    ret = fci_write(cl, FPP_CMD_IPV6_RESET, 0, NULL_PTR);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_IPV6_RESET failed: %d\n", ret);
    }
}

/*
 * @brief       Register two IPv4 routes
 * @details     Function registers two IPv4 routes: one targeting emac0 and the second
 *              one emac1 (emac1 and emac2 for RDB targets). Traffic matching respective
 *              route will be forwarded via physical interface given by
 *              'fpp_rt_cmd_t.output_device' while its source MAC address will be replaced
 *              by MAC address of the output interface and destination MAC address will be
 *              replaced by 'fpp_rt_cmd_t.dst_mac'.
 * @param[in]   cl The FCI client instance
 */
static void sample_fci_router_register_ipv4_routes(FCI_CLIENT *cl)
{
    int ret;

    fpp_rt_cmd_t r1 =
    {
        /*    Register new route */
        .action = FPP_ACTION_REGISTER,
        /*    Destination MAC address to be used for packets matching the route */
        .dst_mac = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee},
        /*    Egress physical interface */
        .output_device = HIF_ID_STRING,
        /*    Unique route identifier */
        .id = oal_htonl(123),
        /*    Use IPv4 addressing */
        .flags = oal_htonl(1)
    };

    fpp_rt_cmd_t r2 =
    {
        .action = FPP_ACTION_REGISTER,
        .dst_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        .output_device = EMACA_ID_STRING,
        .id = oal_htonl(456),
        .flags = oal_htonl(1),
    };

    /*    Register route "r1" */
    ret = fci_write(cl, FPP_CMD_IP_ROUTE, sizeof(r1), (void *)&r1);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_IP_ROUTE[FPP_ACTION_REGISTER] failed: %d\n", ret);
        return;
    }

    /*    Register route "r2" */
    ret = fci_write(cl, FPP_CMD_IP_ROUTE, sizeof(r2), (void *)&r2);
    if (FPP_ERR_OK != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_IP_ROUTE[FPP_ACTION_REGISTER] failed: %d\n", ret);
        return;
    }
}

/*
 * @brief        Register bi-directional UPD connection to be fast-forwarded
 * @details        Add 2 routing table entries with a single conntrack. Traffic
 *                matching respective conntrack will be forwarded via physical
 *                interface given by matching route defined for both, the
 *                original as well as reply direction.
 *
 *                In case of no hit, packet will be sent to default logical
 *                interface (to host). Host can configure slow-path routing using
 *                standard OS-provided mechanisms to route rest of traffic (e.g.
 *                ICMP).
 * @param[in]    cl The FCI client instance
 */
static void sample_fci_router_register_bd_ipv4_conntrack(FCI_CLIENT *cl)
{
    int ret;

    fpp_ct_cmd_t ct1 =
    {
        /*    New connection */
        .action = FPP_ACTION_REGISTER,
        /*    Source IP address: 11.41.48.100 */
        .saddr = oal_htonl(0x0b293064),
        /*    Destination IP address: 12.41.48.100 */
        .daddr = oal_htonl(0x0c293064),
        /*    Source L4 port */
        .sport = oal_htons(11U),
        /*    Destination L4 port */
        .dport = oal_htons(12U),
        /*    Source IP address in reply direction. Equal to 'daddr' to
            disable NAT */
        .saddr_reply = oal_htonl(0x0c293064),
        /*    Destination IP address in reply direction. Same as 'saddr' to
            disable NAT. */
        .daddr_reply = oal_htonl(0x0b293064),
        /*    Source L4 port in reply direction. Equal to 'dport' to disable
            replacement. */
        .sport_reply = oal_htons(12U),
        /*    Destination L4 port in reply direction. Equal to 'sport' to disable
            replacement.*/
        .dport_reply = oal_htons(11U),
        /*    Protocol ID: TCP */
        .protocol = oal_htons(6),
        /*    Flags: None. Create bi-directional connection. */
        .flags = oal_htons(0),
        /*    Associated route (123=HIF_ID_STRING, 456=EMACB_ID_STRING). This routes will be used to
            forward packets matching this tracked connection either in original
            or opposite, reply direction. Routes must exist. To create a route
            see the FPP_CMD_IP_ROUTE. */
        .route_id = oal_htonl(123),
        .route_id_reply = oal_htonl(456)
    };

    /*    Register connection "ct1" */
    ret = fci_write(cl, FPP_CMD_IPV4_CONNTRACK, sizeof(ct1), (void *)&ct1);
    if (0 != ret)
    {
        NXP_LOG_ERROR("FPP_CMD_IPV4_CONNTRACK[FPP_ACTION_REGISTER] failed: %d\n", ret);
        return;
    }
}

/*
 * @brief       Configure IPv4 router using bi-directional conntrack
 * @details     Create routes and conntrack to fast-forward traffic between
 *              EMAC0 and EMAC1 (EMAC1 and EMAC2 for RDB targets).
 *              Put both EMACs to Router mode and enable them.
 * @param[in]   cl The FCI client instance
 */
static void sample_fci_setup_ipv4_router_bd(FCI_CLIENT *cl)
{
    /*    Reset the router */
    sample_fci_router_reset(cl);

    /*    Create routes */
    sample_fci_router_register_ipv4_routes(cl);

    /*    Create bi-directional conntrack */
    sample_fci_router_register_bd_ipv4_conntrack(cl);

    /*    Set interface mode */
    sample_fci_phy_if_set_mode(cl, HIF_ID_STRING, FPP_IF_OP_ROUTER);

    /*    Enable interface */
    sample_fci_phy_if_enable(cl, HIF_ID_STRING);

    /* Catch messages from the FCI endpoint */
    fci_catch(cl);

    /*
        Now traffic received via EMAC0 or EMAC1 and matching conntrack in both
        directions will be routed via interfaces defined by routes.
    */
}

/*
 * @brief        Print all IPv4 routes
 * @param[in]    cl The FCI client instance
 */
static void sample_fci_router_print_ipv4_routes(FCI_CLIENT *cl)
{
    int ret;
    fpp_rt_cmd_t cmd;
    fpp_rt_cmd_t rep;
    unsigned short rep_len;

    cmd.action = FPP_ACTION_QUERY;
    ret = fci_query(cl, FPP_CMD_IP_ROUTE, sizeof(cmd), (void *)&cmd, &rep_len, (void *)&rep);
    while (FPP_ERR_OK == ret)
    {
        if (1 == oal_ntohl(rep.flags))
        {
            NXP_LOG_INFO("%03d: %s (%02x:%02x:%02x:%02x:%02x:%02x), flags: 0x%x\n",
                        (int_t)oal_ntohl(rep.id), rep.output_device, rep.dst_mac[0],
                        rep.dst_mac[1], rep.dst_mac[2], rep.dst_mac[3], rep.dst_mac[4],
                        rep.dst_mac[5], (uint_t)oal_ntohl(rep.flags));
        }
        cmd.action = FPP_ACTION_QUERY_CONT;
        ret = fci_query(cl, FPP_CMD_IP_ROUTE, sizeof(cmd), (void *)&cmd, &rep_len, (void *)&rep);
    }
}
#endif /* defined(FCI_RTABLE_TEST)*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/
#if (defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST))
Std_ReturnType SampleAppFciTestTask(void)
{
    /* local variables here */
    Std_ReturnType stdRet = E_OK;
    FCI_CLIENT *cl;
#if defined(FCI_L2BR_TEST)
    fpp_l2_bd_cmd_t bd;
#endif /* defined(FCI_L2BR_TEST)*/

    cl = fci_open(FCILIB_FF_TYPE, FCI_GROUP_NONE);
    if (NULL_PTR == cl)
    {
        NXP_LOG_ERROR("fci_open() failed\n");
        return E_NOT_OK;
    }

    if (EOK != fci_register_cb(cl, &sample_fci_callback))
    {
        NXP_LOG_ERROR("fci_register_cb() failed\n");
        return E_NOT_OK;
    }

    NXP_LOG_INFO("Physical interfaces info\n");
    sample_fci_phy_if_print_all(cl);
    NXP_LOG_INFO("========================================\n");

    NXP_LOG_INFO("Logical interfaces info\n");
    sample_fci_log_if_print_all(cl);
    NXP_LOG_INFO("========================================\n");

#if defined(FCI_L2BR_TEST)
    /* Create bridge domain */
    if (VLAN_USED > 1U) /* For VLAN 0 and 1 it exists already */
    {
        if (!sample_fci_bd_create(cl, VLAN_USED))
        {
            NXP_LOG_ERROR("sample_fci_bd_create failed\n");
            return E_NOT_OK;
        }
    }

    /* Add interfaces (all standard HIFs and the two configured EMACs) to a bridge domain */
    if (!sample_fci_bd_add_if(cl, VLAN_USED, EMACA_ID_STRING, FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }
    if (!sample_fci_bd_add_if(cl, VLAN_USED, EMACB_ID_STRING, FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }
    if (!sample_fci_bd_add_if(cl, VLAN_USED, "hif0", FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }
    if (!sample_fci_bd_add_if(cl, VLAN_USED, "hif1", FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }
    if (!sample_fci_bd_add_if(cl, VLAN_USED, "hif2", FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }
    if (!sample_fci_bd_add_if(cl, VLAN_USED, "hif3", FALSE))
    {
        NXP_LOG_ERROR("sample_fci_bd_add_if failed\n");
        return E_NOT_OK;
    }

    /* Set bridge domain policy */
    if (!sample_fci_bd_set_policy(cl, VLAN_USED, 0, 1, 0, 1))
    {
        NXP_LOG_ERROR("sample_fci_bd_set_policy failed\n");
        return E_NOT_OK;
    }

    /* Set interfaces mode to bridge */
    sample_fci_phy_if_set_mode(cl, EMACA_ID_STRING, FPP_IF_OP_VLAN_BRIDGE);
    sample_fci_phy_if_set_mode(cl, EMACB_ID_STRING, FPP_IF_OP_VLAN_BRIDGE);
    sample_fci_phy_if_set_mode(cl, "hif0", FPP_IF_OP_VLAN_BRIDGE);
    sample_fci_phy_if_set_mode(cl, "hif1", FPP_IF_OP_VLAN_BRIDGE);
    sample_fci_phy_if_set_mode(cl, "hif2", FPP_IF_OP_VLAN_BRIDGE);
    sample_fci_phy_if_set_mode(cl, "hif3", FPP_IF_OP_VLAN_BRIDGE);

    /*    Enable interfaces */
    sample_fci_phy_if_enable(cl, EMACA_ID_STRING);
    sample_fci_phy_if_enable(cl, EMACB_ID_STRING);
    sample_fci_phy_if_enable(cl, "hif0");
    sample_fci_phy_if_enable(cl, "hif1");
    sample_fci_phy_if_enable(cl, "hif2");
    sample_fci_phy_if_enable(cl, "hif3");

    /* Turn on promisc mode on interfaces */
    sample_fci_phy_if_promisc_on(cl, EMACA_ID_STRING);
    sample_fci_phy_if_promisc_on(cl, EMACB_ID_STRING);
    sample_fci_phy_if_promisc_on(cl, "hif0");
    sample_fci_phy_if_promisc_on(cl, "hif1");
    sample_fci_phy_if_promisc_on(cl, "hif2");
    sample_fci_phy_if_promisc_on(cl, "hif3");

    /* Get bridge domain */
    if (!sample_fci_bd_get(cl, VLAN_USED, &bd))
    {
        NXP_LOG_ERROR("sample_fci_bd_get failed\n");
        return E_NOT_OK;
    }

    /* Print info from the bridge domain */
    NXP_LOG_INFO("Bridge domain info\n");
    NXP_LOG_INFO("Action: %d, VLAN ID: %d\n",
                (int_t)oal_ntohs(bd.action), (int_t)oal_ntohs(bd.vlan));
    NXP_LOG_INFO("ucast_hit: %d, ucast_miss: %d, mcast_hit: %d, mcast_miss: %d",
                (int_t)bd.ucast_hit, (int_t)bd.ucast_miss,
                (int_t)bd.mcast_hit, (int_t)bd.mcast_miss);
    NXP_LOG_INFO("tagged: 0x%08x, untagged: 0x%08x\n",
                (uint_t)oal_ntohl(bd.if_list),
                (uint_t)oal_ntohl(bd.untag_if_list));
    NXP_LOG_INFO("========================================\n");

    NXP_LOG_INFO("Physical interfaces info\n");
    sample_fci_phy_if_print_all(cl);
    NXP_LOG_INFO("========================================\n");
#endif

#if defined(FCI_RTABLE_TEST)
    /* Configure IPv4 router using bi-directional conntrack */
    sample_fci_setup_ipv4_router_bd(cl);

    NXP_LOG_INFO("IPv4 routes info\n");
    sample_fci_router_print_ipv4_routes(cl);
    NXP_LOG_INFO("========================================\n");
#endif

    return stdRet;
}
#endif /* defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST) */

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#endif /* PFE_CFG_FCI_ENABLE */

#ifdef __cplusplus
}
#endif
