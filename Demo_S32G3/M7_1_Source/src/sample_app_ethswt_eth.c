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
#include "sample_app_ethswt_eth.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define TST_ARP_FRM_FILL_BYTE             0U
#define TST_ARP_FRM_LENGTH                28U
#define TST_ARP_FRM_TYPE                  0x0806U
#define TST_ARP_FRM_SRC_MAC_OFFSET        8U
#define TST_ARP_FRM_SRC_IP_OFFSET         14U
#define TST_ARP_FRM_DES_IP_OFFSET         24U

#define TST_IPV4_TCP_FRM_LENGTH           52U
#define TST_IPV4_TCP_FRM_TYPE             0x0800U
#define TST_IPV4_TCP_FRM_SRC_IP_OFFSET    12U
#define TST_IPV4_TCP_FRM_DES_IP_OFFSET    16U
#define TST_IPV4_TCP_FRM_SRC_PORT_OFFSET  20U
#define TST_IPV4_TCP_FRM_DES_PORT_OFFSET  22U

#define ETH_43_PFE_START_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

/* ARP packet */
uint8 tst_arp_frm_data[] =
{
    0x00U, 0x01U, /* HW type */
    0x08U, 0x00U, /* Protocol type */
    0x06U, /* HW size */
    0x04U, /* Protocol size */
    0x00U, 0x02U, /* Opcode: ARP reply */
    0xd8U, 0xcbU, 0x8aU, 0xa3U, 0x7dU, 0xcfU, /* Sender MAC */
    0x00U, 0x00U, 0x00U, 0x00U, /* Sender IP */
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, /* Target MAC */
    0x00U, 0x00U, 0x00U, 0x00U, /* Target IP */
};

/* IPv4 TCP packet*/
uint8 tst_ipv4_tcp_frm_data[] =
{
    /* IPv4: */
    0x45U, /* Internet Protocol Version */
    0x00U, 0x00U, /* Type of Service */
    0x34U, /* Total Length */
    0x59U, 0xa1U, /* Identification */
    0x00U, 0x00U, /* Flags + Fragment Offset */
    0x80U, /* Time To Live */
    0x06U, /* Protocol */
    0x69U, 0x09U, /* IPv4 Header Checksum */
    0x0bU, 0x29U, 0x30U, 0x64U, /* Source IP */
    0x0cU, 0x29U, 0x30U, 0x64U, /* Destination IP */
    /* TCP: */
    0x00U, 0x0bU, /* Source Port */
    0x00U, 0x0cU, /* Destination Port */
    0x03U, 0xa0U, 0x88U, 0x30U, /* Sequence Number */
    0x00U, 0x00U, 0x00U, 0x00U, /* Acknowledgement Number */
    0x80U, /* Header Length */
    0x00U, /* Control Flags */
    0x20U, 0x00U, /* Window Size */
    0x4bU, 0x39U, /* TCP Checksum (Checksum Of TCP Header + Payload) */
    0x00U, 0x00U, /* Urgent Pointer */
    0x02U, 0x04U, 0x05U, 0x8cU, 0x01U, 0x03U, 0x03U, 0x08U, 0x01U, 0x01U, 0x04U, 0x02U /* Options and Pading */
};

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

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
/* Used when running master and slave at the same time */
typedef struct
{
    uint32 rx_frm_cnt[NUM_CONTROLLER_USED];
    uint32 tx_frm_cnt[NUM_CONTROLLER_USED];
    uint8  mac_addr[NUM_CONTROLLER_USED][6U];
    bool_t is_ready;
} device_info_t;

typedef struct
{
    device_info_t master;
    device_info_t slave;
    bool_t enable_write_mii;
    bool_t loopback_mode;
} shared_status_t;

#define ETH_43_PFE_START_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

volatile shared_status_t *p_shared_status = (volatile shared_status_t *) 0x345F4000U;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

/* usage scope: Tst_Pfe_SendDummyFrame */
static uint8        Priority = 0U;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

extern volatile uint32 *pfeTxCtr;
extern volatile uint32 *pfeTxErrorCtr;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

extern uint32 ipv4_string_to_int(const char *p_ip_addr);

Std_ReturnType Tst_Pfe_SendDummyFrame (uint8 srcCtrlIndex, uint8 *dstMacAddr, frame_type_t frmType)
{
    Std_ReturnType      tempRet = E_NOT_OK;
    Eth_DataType        *bufPtr;
    Eth_BufIdxType      BufferIndex;
    uint16              LengthInBytes;
    uint16              u16TempLengthInBytes;
    BufReq_ReturnType   ProvideTxBufferResult;
    uint32              srcIpAddr;
    uint32              dstIpAddr;
    char                *pSrcIpAddr = NULL;
    char                *pDstIpAddr = NULL;
    uint8               srcMac[6];
    uint8               u8SrcIpOffset = TST_ARP_FRM_SRC_IP_OFFSET;
    uint8               u8DesIpOffset = TST_ARP_FRM_DES_IP_OFFSET;
    uint16              u16SrcPort;
    uint16              u16DesPort;
    Eth_FrameType       frameType = TST_ARP_FRM_TYPE;

    pfeTxCtr[srcCtrlIndex]++;

    if (frmType == FRAME_TYPE_IPV)
    {
        LengthInBytes = TST_IPV4_TCP_FRM_LENGTH;
    }
    else
    {
        LengthInBytes = TST_ARP_FRM_LENGTH;
    }

    u16TempLengthInBytes = LengthInBytes;

    ProvideTxBufferResult = Eth_43_PFE_ProvideTxBuffer(srcCtrlIndex, Priority, &BufferIndex, &bufPtr, &u16TempLengthInBytes);

    if (BUFREQ_OK == ProvideTxBufferResult)
    {
        if (frmType == FRAME_TYPE_IPV)
        {
            frameType = TST_IPV4_TCP_FRM_TYPE;
            pSrcIpAddr = RTABLE_IPV4_SRC_IP_ADDR;
            pDstIpAddr = RTABLE_IPV4_DES_IP_ADDR;
            u8SrcIpOffset = TST_IPV4_TCP_FRM_SRC_IP_OFFSET;
            u8DesIpOffset = TST_IPV4_TCP_FRM_DES_IP_OFFSET;
        }
        else
        {
            if(srcCtrlIndex == 0U)
            {
                pSrcIpAddr = IPV4_LOCAL_IP_ADDR_0;
                pDstIpAddr = IPV4_LOCAL_IP_ADDR_0;
            }
            else
            {
                pSrcIpAddr = IPV4_LOCAL_IP_ADDR_1;
                pDstIpAddr = IPV4_LOCAL_IP_ADDR_1;
            }
        }

        srcIpAddr = oal_htonl(ipv4_string_to_int(pSrcIpAddr));
        dstIpAddr = oal_htonl(ipv4_string_to_int(pDstIpAddr));

        if(frmType == FRAME_TYPE_IPV)
        {
            /* Update sender and target IP address */
            (void)autolibc_memcpy(&tst_ipv4_tcp_frm_data[u8SrcIpOffset], &srcIpAddr, 4U);
            (void)autolibc_memcpy(&tst_ipv4_tcp_frm_data[u8DesIpOffset], &dstIpAddr, 4U);

            /* Update sender and target port */
            u16SrcPort = oal_htons(RTABLE_IPV4_SRC_PORT);
            u16DesPort = oal_htons(RTABLE_IPV4_DES_PORT);
            (void)autolibc_memcpy(&tst_ipv4_tcp_frm_data[TST_IPV4_TCP_FRM_SRC_PORT_OFFSET], &u16SrcPort, 2U);
            (void)autolibc_memcpy(&tst_ipv4_tcp_frm_data[TST_IPV4_TCP_FRM_DES_PORT_OFFSET], &u16DesPort, 2U);

            /* Write frame data into the TX buffer */
            (void)autolibc_memcpy(bufPtr, tst_ipv4_tcp_frm_data, TST_IPV4_TCP_FRM_LENGTH);
        }
        else
        {
            /* Update sender and target IP address */
            (void)autolibc_memcpy(&tst_arp_frm_data[u8SrcIpOffset], &srcIpAddr, 4U);
            (void)autolibc_memcpy(&tst_arp_frm_data[u8DesIpOffset], &dstIpAddr, 4U);

            /* Update sender MAC address */
            Eth_43_PFE_GetPhysAddr(srcCtrlIndex, &srcMac[0U]);
            (void)autolibc_memcpy(&tst_arp_frm_data[TST_ARP_FRM_SRC_MAC_OFFSET], &srcMac[0U], 6U);

            /* Write frame data into the TX buffer */
            (void)autolibc_memcpy(bufPtr, tst_arp_frm_data, TST_ARP_FRM_LENGTH);
        }

        tempRet = Eth_43_PFE_Transmit(srcCtrlIndex, BufferIndex, frameType, TRUE, LengthInBytes, dstMacAddr);

        if (E_OK != tempRet)
        {
            pfeTxErrorCtr[srcCtrlIndex]++;
        }
    }

    Priority = (Priority + 1) % ETH_43_PFE_MAX_TX_PRIO_NUM;

    return tempRet;
}

#ifdef LOOPBACK_TEST
uint32 Tst_Pfe_LoopbackTestFramesGood[NUM_CONTROLLER_USED] = {0U};
uint32 Tst_Pfe_LoopbackTestFramesBad[NUM_CONTROLLER_USED] = {0U};

/* Check if the frame matches transmitted dummy frame */
void Tst_Pfe_LoopbackTestCheckRxFrame( uint8 CtrlIdx, \
                                                        Eth_FrameType FrameType, \
                                                        bool_t IsBroadcast, \
                                                        uint8 * PhysAddrPtr, \
                                                        Eth_DataType *  DataPtr, \
                                                        uint16 LenByte
                                                      )
{
    (void)PhysAddrPtr;
    (void)LenByte;
    uint16 Idx;
    bool_t fail = FALSE;

    if(TST_ARP_FRM_TYPE != FrameType)
    {
        fail = TRUE;
    }
    if(FALSE == IsBroadcast)
    {
        fail = TRUE;
    }
    for(Idx = 0U; Idx < TST_ARP_FRM_LENGTH; Idx++)
    {
        if(TST_ARP_FRM_FILL_BYTE != DataPtr[Idx])
        {
            fail = TRUE;
        }
    }
    if(TRUE == fail)
    {
        Tst_Pfe_LoopbackTestFramesBad[CtrlIdx]++;
    }
    else
    {
        Tst_Pfe_LoopbackTestFramesGood[CtrlIdx]++;
    }
}
#endif /* LOOPBACK_TEST */

/*================================================================================================*/
/**
@brief      SampleAppEthInit - Initialize the Eth task
@details    After calling this function the task containing Eth driver should be initialized

@return     Returns the value of success
@retval     E_OK or E_NOT_OK

@pre None
@post None
*/
Std_ReturnType SampleAppEthInit(void)
{
    /* local variables here */
    Std_ReturnType  stdRet          = E_OK;
    uint8           pfeCtrlIndex    = 0U;

    Eth_43_PFE_Init(NULL_PTR);

#if defined (PFE_CFG_PFE_MASTER)
#ifdef LOOPBACK_TEST
    /* The RX clock is not provided in loopback so configure the PHY to generate it */
    Eth_43_PFE_WriteMii(1U, 4U, 0U, 0x0140); /* Call Eth_WriteMii to disable autonegotiation, 1Gb/s */
#endif
#endif

    for (pfeCtrlIndex = 0U; pfeCtrlIndex < NUM_CONTROLLER_USED; pfeCtrlIndex++)
    {
        stdRet |= Eth_43_PFE_SetControllerMode(pfeCtrlIndex, ETH_MODE_ACTIVE);
    }

    return(stdRet);
}

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

/*================================================================================================*/
#ifdef __cplusplus
}
#endif

