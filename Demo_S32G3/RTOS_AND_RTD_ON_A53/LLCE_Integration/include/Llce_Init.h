/*
 * Copyright 2025 NXP.
 */
#ifndef LLCE_INIT_H
#define LLCE_INIT_H

#include "Can_43_Llce.h"

#ifdef ASR_NOT_USED
    #include "PlatformTypes.h"    
#endif

#define CAN_FD_FRAME_LEN 64U
#define CAN_STD_FRAME_LEN 8U
#define TP_TIMEOUT 800000U
#define CONTROLLER1 0
#define CONTROLLER2 1
#define CONTROLLER3 2
#define CONTROLLER4 3
#define BADU_500K_2M 1
#define ITER_NR 200
#define MAX_BUFFER_FRAME_FD 64
#define CAN_LPDU_FD_U32 ((uint32)0x40000000)
#define MAX_NUM_OF_CORES 8U
#define ON  1
#define OFF 0

#define TST_CTRL_REG_ADDR   0x4007C0F0  /* TIMESTAMP_CONTROL_REGISTER */
#define TST_SOURCE_SEL_MASK 0x00000001  /* Timestamp 1 counter output is selected as timestamp module output */
#define TST_ENABLE_MASK     0x00000004  /* Timestamp module enable control */
#define STM7_BASE_ADDR      0x40228000  /* STM_7 base address */
#define STM7_ENABLE_MASK    0x00000001  /* STM_7 enable bit */
#define STMTS_REG_ADDR      0x4400C000  /* STM_TS register address */
#define STMTS_SEL_MASK      0x00008501  /* STM_TS mask to set presclarer to 133 */

/* Sample app specific variable. Possible values: ON, OFF */
extern ISR(Can_FifoTxAckNotEmptyIsr_0_7);
extern ISR(Can_FifoTxAckNotEmptyIsr_8_15);
extern ISR(Can_FifoRxOutNotEmptyIsr_0_7);
extern ISR(Can_FifoRxOutNotEmptyIsr_8_15);
extern ISR(Can_FifoRxInNotEmptyIsr_0_7);
extern ISR(Can_FifoRxInNotEmptyIsr_8_15);

#if CAN_43_LLCE_MAX_PARTITIONS == 1U
    extern volatile uint32 last_RxIndication;
    extern volatile uint32 last_TxConfirmation;
#else
    extern volatile uint32 last_RxIndication[MAX_NUM_OF_CORES];
    extern volatile uint32 last_TxConfirmation[MAX_NUM_OF_CORES];
#endif


void Can_Llce_Init_Irq(void);

void Can_Enable_Timestamp(void);

void Circular_Permutation(volatile uint8 *Payload, uint8 n);

StatusType Check_Status(Can_PduType *CanMessage);

#endif
