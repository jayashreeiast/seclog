/*
 * Copyright 2025 NXP.
 */
#include "Pfe_Rtos_Task.h"
#if defined(PING_TEST)
#include "Eth_queue.h"
#elif defined(LWIP)
#include "lwip_demo.h"
#endif
#include "Gpt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gic.h"


#ifdef LWIP
   uint32 g_sys_milliseconds;
#endif

extern ISR(PIT_1_ISR);

uint32 ipv4_string_to_int(const char *p_ip_addr)
{
    uint32 val = 0;
    uint8 i;          /* The count of the number of bytes processed. */
    const char *str;    /* A pointer to the next digit to process. */
    char c;             /* The digit being processed. */
    uint16 n;         /* The value of this byte. */

    str = p_ip_addr;
    if (NULL != p_ip_addr)
    {
        for (i = 0; i < 4; i++)
        {
            n = 0;
            while (1)
            {
                c = *str;
                str++;
                if (c >= '0' && c <= '9')
                {
                    n *= 10;
                    n += c - '0';
                }
                else if ((i < 3 && c == '.') || i == 3)
                {
                    break;
                }
                else
                {
                    return 0;
                }
            }
            if (n >= 256)
            {
                return 0;
            }
            val *= 256;
            val += n;
        }
    }
    return val;
}

void ipv6_string_to_int(const char *p_ip_addr, uint32 *ipv6)
{
    uint8 i;          /* The count of the number of bytes processed. */
    const char *str;    /* A pointer to the next digit to process. */
    char c;             /* The digit being processed. */
    uint32 n;         /* The value of this byte. */
    uint8 temp=0;

    str = p_ip_addr;
    if (NULL != p_ip_addr)
    {
        for (i = 0; i < 4; i++)
        {
            n = 0;
            temp=0;
            while (1)
            {
                c = *str;
                str++;
                if (c >= '0' && c <= '9')
                {
                    n *= 16;
                    n += c - '0';
                }
                else if (c >= 'a' && c <= 'f')
                {
                    n *= 16;
                    n += c - 'W';
                }
                else if ( (c == ':') || i==3)
                {
                    temp++;
                    if (temp == 2)
                    {
                        break;
                    }
                }
            }
            ipv6[i] = n;
        }
    }
}

#if defined(PING_TEST)
boolean send_frame(uint8 *data, uint16 ether_type, uint16 data_length, uint8 iface, uint8 *target_mac)
{
    Eth_BufIdxType buf_idx;
    Eth_DataType *buf_ptr;
    uint16 temp_length = data_length;
    BufReq_ReturnType buf_status;
    uint32 ctr;
    bool_t ret = TRUE;

    /* Get buffer */
    buf_status = Eth_43_PFE_ProvideTxBuffer(iface, 0U, &buf_idx, &buf_ptr, &temp_length);
    if (BUFREQ_OK != buf_status)
    {
        ret = FALSE;
    }
    if (temp_length < data_length)
    {
        ret = FALSE;
    }
    /* Fill buffer */
    for (ctr = 0U; ctr < data_length; ctr++)
    {
        buf_ptr[ctr] = (Eth_DataType)data[ctr];
    }

    /* Transmit the data */
    Eth_43_PFE_Transmit(iface, buf_idx, ether_type, TRUE, data_length, target_mac);
    Eth_43_PFE_TxConfirmation(iface);

    return ret;
}

void rx_process(uint8 ctrl_idx, uint8 *phys_addr_ptr, Eth_DataType *p_rx_data, uint16 len_byte)
{
    queue_elm_info_t new_data;
    Eth_FrameType ether_type;
    uint8 Protocol = 0;
    uint8 Type = 0;
    arp_packet_t *p_arp_request;
    arp_packet_t *p_arp_reply;
    ipv4_header_t *p_packet;
    ipv6_header_t *p_packetv6;
    ipv4_header_t *p_ip_header;
    ipv6_header_t *p_ip_headerv6;
    icmp_echo_message_t *p_icmp_request_header;
    icmp_echo_message_t *p_icmp_reply_header;
    icmp_message_v6_t *p_icmp_headerv6; 
    uint8 src_mac[6];
    static arp_packet_t arp_buffer;
    static uint8 ipv4_header_buffer[1500];
    static uint8 ipv6_header_buffer[1500];
    char *p_ip_addr = NULL;
    char *p_ip_addr_v6 = NULL;
    uint32 ip_local_addr;
    uint32 ip_local_addrv6[4]={0};
    uint32 ip_local_addrv6_temp=0;
    uint32 payload_length;
    uint8 ret=10;

    /* Prepare the MAC address of this device */
    Eth_43_PFE_GetPhysAddr(ctrl_idx, &src_mac[0]);

    /* Local IP address for each controller */
    switch(ctrl_idx){
        case 0U: 
            p_ip_addr = IPV4_LOCAL_IP_ADDR_0;
            p_ip_addr_v6 = IPV6_LOCAL_IP_ADDR_0;
            break;
        case 1U: 
            p_ip_addr = IPV4_LOCAL_IP_ADDR_1;
            p_ip_addr_v6 = IPV6_LOCAL_IP_ADDR_1;
            break;
        case 2U: 
            p_ip_addr = IPV4_LOCAL_IP_ADDR_2;
            p_ip_addr_v6 = IPV6_LOCAL_IP_ADDR_2;
            break;
        default: p_ip_addr = NULL; break;
    }

    ip_local_addr = ipv4_string_to_int(p_ip_addr);
    ipv6_string_to_int(p_ip_addr_v6,ip_local_addrv6);
    ip_local_addrv6_temp=oal_htonl(ip_local_addrv6[3] | 0xff000000);
    for (int i=0;i<4;i++)
    {
        ip_local_addrv6[i]=oal_htonl(ip_local_addrv6[i]);
    }
    if (NULL != p_rx_data)
    {
        ether_type = be16_to_cpu(*(Eth_FrameType *)(p_rx_data - 2U));
        /* Check if received frame is ARP */
        if (ether_type == ETHER_TYPE_ARP)
        {
            p_arp_request = (arp_packet_t *)p_rx_data;
            /* Check if dest ip address matches local ip address */
            if (ip_local_addr == oal_htonl(p_arp_request->tpa))
            {
                /*  Initialize the arp reply buffer */
                p_arp_reply = &arp_buffer;
                (void)autolibc_memset(p_arp_reply, 0, sizeof(arp_packet_t));
                /* Format ARP request */
                p_arp_reply->hrd = oal_htons(ARP_HARDWARE_TYPE_ETH);
                p_arp_reply->pro = oal_htons(ARP_PROTOCOL_TYPE_IPV4);
                p_arp_reply->hln = ARP_HARDWARE_ADDR_LEN;
                p_arp_reply->pln = ARP_PROTOCOL_ADDR_LEN;
                p_arp_reply->op = oal_htons(ARP_OPCODE_ARP_REPLY);
                p_arp_reply->spa = p_arp_request->tpa;
                p_arp_reply->tpa = p_arp_request->spa;
                (void)autolibc_memcpy(p_arp_reply->sha, src_mac, 6U);
                (void)autolibc_memcpy(p_arp_reply->tha, p_arp_request->sha, 6U);
                /* Update new data for sending ethernet frame */
                new_data.ctrl_idx = ctrl_idx;
                new_data.phys_addr_ptr = phys_addr_ptr;
                new_data.data_ptr = (Eth_DataType *)p_arp_reply;
                new_data.ether_type = ether_type;
                new_data.len_byte = len_byte;
                if (!queue_is_full())
                {
                    queue_push(new_data);
                }
            }
        }
        /* Check if received frame is IPv4 */
        if (ether_type == ETHER_TYPE_IPV4)
        {
            Protocol = (*(uint8 *)(p_rx_data + 9U));
            /* Check if protocol of received frame is ICMP */
            if(Protocol == PROTOCOL_TYPE_ICMP)
            {
                p_ip_header = (ipv4_header_t *)p_rx_data;
                /* Check if dest ip address matches local ip address */
                if (ip_local_addr == oal_htonl(p_ip_header->dest_addr))
                {
                    /*  Initialize the ipv4 header buffer */
                    p_packet = (ipv4_header_t *)&ipv4_header_buffer;
                    (void)autolibc_memset(p_packet, 0, sizeof(ipv4_header_t));
                    /* Format IPv4 header */
                    p_packet->version = IPV4_VERSION;
                    p_packet->header_length = IPV4_HEADER_LENGTH;
                    p_packet->type_of_service = IPV4_TYPE_OF_SERVICE;
                    p_packet->total_length = oal_htons(len_byte);
                    p_packet->identification = p_ip_header->identification;
                    p_packet->fragment_offset = p_ip_header->fragment_offset;
                    p_packet->time_to_live = p_ip_header->time_to_live;
                    p_packet->protocol = p_ip_header->protocol;
                    p_packet->header_checksum = 0;
                    p_packet->src_addr = p_ip_header->dest_addr;
                    p_packet->dest_addr = p_ip_header->src_addr;

                    /* Get ICMP packet */
                    p_icmp_request_header = (icmp_echo_message_t *)( &p_rx_data[sizeof(ipv4_header_t)]);
                    /*  Initialize the ICMP reply header buffer */
                    p_icmp_reply_header = (icmp_echo_message_t *)&ipv4_header_buffer[sizeof(ipv4_header_t)];
                    (void)autolibc_memset(p_icmp_reply_header, 0, sizeof(icmp_echo_message_t));
                    /* Format Echo Reply header */
                    p_icmp_reply_header->type = ICMP_TYPE_ECHO_REPLY;
                    p_icmp_reply_header->code = 0;
                    p_icmp_reply_header->checksum = 0;
                    p_icmp_reply_header->identifier = p_icmp_request_header->identifier;
                    p_icmp_reply_header->sequence_number = p_icmp_request_header->sequence_number;
                    /* The length of the ICMP payload */
                    payload_length = len_byte - (sizeof(ipv4_header_t) + sizeof(icmp_echo_message_t));
                    (void)autolibc_memcpy(p_icmp_reply_header->payload, p_icmp_request_header->payload, payload_length);
                    /* Update new data for sending ethernet frame */
                    new_data.ctrl_idx = ctrl_idx;
                    new_data.phys_addr_ptr = phys_addr_ptr;
                    new_data.data_ptr = (Eth_DataType *)p_packet;
                    new_data.ether_type = ether_type;
                    new_data.len_byte = len_byte;
                    if (!queue_is_full())
                    {
                        queue_push(new_data);
                    }
                }
            }
        }
        /* Check if received frame is IPv6 */
        if (ether_type == ETHER_TYPE_IPV6)
        {           
            Protocol = (*(uint8 *)(p_rx_data + 6U));
            /* Check if protocol of received frame is ICMP */
            if(Protocol == PROTOCOL_TYPE_ICMPV6)
            {
                Type = (*(uint8 *)(p_rx_data + 40U));
                if (Type == NEIGHBOR_SOLICITATION_TYPE)
                {
                    p_ip_headerv6 = (ipv6_header_t *)(p_rx_data);
                    /* Check if dest ip address matches local ip address */
                    
                    if (ip_local_addrv6_temp==p_ip_headerv6->dest_addr[3])
                    {
                        /*  Initialize the ipv6 header buffer */
                        p_packetv6 = (ipv6_header_t *)&ipv6_header_buffer;
                        (void)autolibc_memset(p_packetv6, 0, sizeof(ipv6_header_t));
                        /* Format IPv6 header */
                        p_packetv6->version = IPV6_VERSION;
                        p_packetv6->traffic_class1 = 0x0;
                        p_packetv6->traffic_class2 = 0x0;
                        p_packetv6->flow_label = 0x0;
                        p_packetv6->payload_length = p_ip_headerv6->payload_length;
                        p_packetv6->next_header = p_ip_headerv6->next_header;
                        p_packetv6->hop_limit = p_ip_headerv6->hop_limit;
                        p_packetv6->src_addr[0] = ip_local_addrv6[0];
                        p_packetv6->src_addr[1] = ip_local_addrv6[1];
                        p_packetv6->src_addr[2] = ip_local_addrv6[2];
                        p_packetv6->src_addr[3] = ip_local_addrv6[3];
                        p_packetv6->dest_addr[0] = p_ip_headerv6->src_addr[0];
                        p_packetv6->dest_addr[1] = p_ip_headerv6->src_addr[1];
                        p_packetv6->dest_addr[2] = p_ip_headerv6->src_addr[2];
                        p_packetv6->dest_addr[3] = p_ip_headerv6->src_addr[3];
                        
                        /*  Initialize the ICMP reply header buffer */
                        p_icmp_headerv6 = (icmp_message_v6_t *)&ipv6_header_buffer[sizeof(ipv6_header_t)];
                        (void)autolibc_memset(p_icmp_headerv6, 0, sizeof(icmp_message_v6_t));
                        /* Format Echo Reply header */
                        p_icmp_headerv6->type = 0x88;
                        p_icmp_headerv6->code = 0;
                        p_icmp_headerv6->checksum = 0;
                        p_icmp_headerv6->Reserved = 0x000000e0;
                        p_icmp_headerv6->target_address[0] = ip_local_addrv6[0];
                        p_icmp_headerv6->target_address[1] = ip_local_addrv6[1];
                        p_icmp_headerv6->target_address[2] = ip_local_addrv6[2];
                        p_icmp_headerv6->target_address[3] = ip_local_addrv6[3];
                        p_icmp_headerv6->src_link_address=0x2;
                        p_icmp_headerv6->length=0x1;
                        (void)autolibc_memcpy(p_icmp_headerv6->mac, ETH_43_PFE_CFG_MACADDRESS(ctrl_idx), sizeof(p_icmp_headerv6->mac));
                        /* Update new data for sending ethernet frame */
                        new_data.ctrl_idx = ctrl_idx;
                        new_data.phys_addr_ptr = phys_addr_ptr;
                        new_data.data_ptr = (Eth_DataType *)p_packetv6;
                        new_data.ether_type = ether_type;
                        new_data.len_byte = len_byte;
                        if (!queue_is_full())
                        {
                            queue_push(new_data);
                        }
                    }
                }
                if (Type == ICMPV6_TYPE_ECHO_REQUEST)
                {
                    p_ip_headerv6 = (ipv6_header_t *)(p_rx_data);
                    /* Check if dest ip address matches local ip address */
                    ret=autolibc_memcmp(ip_local_addrv6,p_ip_headerv6->dest_addr,sizeof(ip_local_addrv6));
                    if (ret == 0)
                    {
                        /*  Initialize the ipv6 header buffer */
                        p_packetv6 = (ipv6_header_t *)&ipv6_header_buffer;
                        (void)autolibc_memset(p_packetv6, 0, sizeof(ipv6_header_t));
                        /* Format IPv6 header */
                        p_packetv6->version = IPV6_VERSION;
                        p_packetv6->traffic_class1 = 0x0;
                        p_packetv6->traffic_class2 = 0x0;
                        p_packetv6->flow_label = 0x0;
                        p_packetv6->payload_length = p_ip_headerv6->payload_length;
                        p_packetv6->next_header = p_ip_headerv6->next_header;
                        p_packetv6->hop_limit = p_ip_headerv6->hop_limit;
                        p_packetv6->src_addr[0] = ip_local_addrv6[0];
                        p_packetv6->src_addr[1] = ip_local_addrv6[1];
                        p_packetv6->src_addr[2] = ip_local_addrv6[2];
                        p_packetv6->src_addr[3] = ip_local_addrv6[3];
                        p_packetv6->dest_addr[0] = p_ip_headerv6->src_addr[0];
                        p_packetv6->dest_addr[1] = p_ip_headerv6->src_addr[1];
                        p_packetv6->dest_addr[2] = p_ip_headerv6->src_addr[2];
                        p_packetv6->dest_addr[3] = p_ip_headerv6->src_addr[3];

                        /* Get ICMP packet */
                        p_icmp_request_header = (icmp_echo_message_t *)( &p_rx_data[sizeof(ipv6_header_t)]);
                        /*  Initialize the ICMP reply header buffer */
                        p_icmp_reply_header = (icmp_echo_message_t *)&ipv6_header_buffer[sizeof(ipv6_header_t)];
                        (void)autolibc_memset(p_icmp_reply_header, 0, sizeof(icmp_echo_message_t));
                        /* Format Echo Reply header */
                        p_icmp_reply_header->type = ICMPV6_TYPE_ECHO_REPLY;
                        p_icmp_reply_header->code = 0;
                        p_icmp_reply_header->checksum = 0;
                        p_icmp_reply_header->identifier = p_icmp_request_header->identifier;
                        p_icmp_reply_header->sequence_number = p_icmp_request_header->sequence_number;
                        /* The length of the ICMP payload */
                        payload_length = len_byte - (sizeof(ipv6_header_t) + sizeof(icmp_echo_message_t));
                        (void)autolibc_memcpy(p_icmp_reply_header->payload, p_icmp_request_header->payload, payload_length);
                        /* Update new data for sending ethernet frame */
                        new_data.ctrl_idx = ctrl_idx;
                        new_data.phys_addr_ptr = phys_addr_ptr;
                        new_data.data_ptr = (Eth_DataType *)p_packetv6;
                        new_data.ether_type = ether_type;
                        new_data.len_byte = len_byte;
                        if (!queue_is_full())
                        {
                            queue_push(new_data);
                        }
                    }
                }
            }
        }
    }
}

void rx_callback(uint8 ctrl_idx, Eth_FrameType frame_type, boolean is_broadcast,
                    uint8 *phys_addr_ptr, Eth_DataType *p_rx_data, uint16 len_byte)
{
    (void)is_broadcast;
    (void)phys_addr_ptr;
    (void)frame_type;

#if (PFE_CFG_VERBOSITY_LEVEL >= 9)
    NXP_LOG_INFO("Received on PFE%hhu, type 0x%hx, length %hu, data %02hhx %02hhx %02hhx %02hhx %02hhx...", (uint8)ctrl_idx, (uint16)frame_type, len_byte, p_rx_data[0U], p_rx_data[1U], p_rx_data[2U], p_rx_data[3U], p_rx_data[4U]);
#endif
    rx_process(ctrl_idx, phys_addr_ptr, p_rx_data, len_byte);
}
#endif

void EthSwt_Tst_PitNotification(void)
{
#ifdef LWIP
    g_sys_milliseconds++;
#endif
}

void Pit_Init_Irq(void)
{
    /* Configure interrupt support for PIT1.*/
    vGicRegisterIsrHandler(PIT1_IRQn, PIT_1_ISR);
    vGicSetIntPriority(PIT1_IRQn, 10);
    vGicEnableIntID(PIT1_IRQn);
}

void Pfe_Rtos_PrePare(void)
{
    Pit_Init_Irq();
    /* Initialize GPT and start PIT1 timer. */
    Gpt_Init(NULL_PTR);
    Gpt_EnableNotification(GPT_SLAVE_CHANNEL);
    Gpt_StartTimer(GPT_SLAVE_CHANNEL, GPT_TIMER_VALUE);
    /*Init PFE Slave driver*/
    Eth_43_PFE_PreInit(NULL_PTR);
    Eth_43_PFE_Init(NULL_PTR);
    uint8 pfeCtrlIndex = 0U;
    Std_ReturnType stdRet = E_OK;
    for (pfeCtrlIndex = 0U; pfeCtrlIndex < NUM_CONTROLLER_USED; pfeCtrlIndex++)
    {
        stdRet |= Eth_43_PFE_SetControllerMode(pfeCtrlIndex, ETH_MODE_ACTIVE);
    }
    #if defined (PING_TEST)
    queue_init();
    #endif
    #if defined (LWIP)
    demo_lwip_init();
    #endif
}

void Pfe_Slave_Ping_Task(void )
{
    Eth_RxStatusType PFEStatus;
    while(1)
    {
        Eth_43_PFE_Receive(2, 0U, &PFEStatus);
        #if defined (PING_TEST)
        queue_elm_info_t queue_data;
        if (!queue_is_empty())
        {
            queue_data = queue_pop();
            send_frame(queue_data.data_ptr, queue_data.ether_type, queue_data.len_byte, queue_data.ctrl_idx, queue_data.phys_addr_ptr);
        }
        #endif
    }
}