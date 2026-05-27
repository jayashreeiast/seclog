/*
*   Copyright 2019-2025 NXP
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms.  By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms.  If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
*
*   This file contains sample code only. It is not part of the production code deliverables.
*/

#include "Llce_RegAccess.h"
#include "gic.h"
#include "Llce_Init.h"


void Can_Llce_Init_Irq(void)
{
    /* Configure interrupt support for RXIN fifo.*/
    vGicRegisterIsrHandler(LLCE0_ICSR14_IRQn, Can_FifoRxInNotEmptyIsr_0_7);
    vGicSetIntPriority(LLCE0_ICSR14_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR14_IRQn);
    vGicRegisterIsrHandler(LLCE0_ICSR15_IRQn, Can_FifoRxInNotEmptyIsr_8_15);
    vGicSetIntPriority(LLCE0_ICSR15_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR15_IRQn);
    /* Configure interrupt support for RXOUT fifo.*/
    vGicRegisterIsrHandler(LLCE0_ICSR16_IRQn, Can_FifoRxOutNotEmptyIsr_0_7);
    vGicSetIntPriority(LLCE0_ICSR16_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR16_IRQn);
    vGicRegisterIsrHandler(LLCE0_ICSR17_IRQn, Can_FifoRxOutNotEmptyIsr_8_15);
    vGicSetIntPriority(LLCE0_ICSR17_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR17_IRQn);
    /* Configure interrupt support for TXACK fifo.*/
    vGicRegisterIsrHandler(LLCE0_ICSR22_IRQn, Can_FifoTxAckNotEmptyIsr_0_7);
    vGicSetIntPriority(LLCE0_ICSR22_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR22_IRQn);
    vGicRegisterIsrHandler(LLCE0_ICSR23_IRQn, Can_FifoTxAckNotEmptyIsr_8_15);
    vGicSetIntPriority(LLCE0_ICSR23_IRQn, 10);
    vGicEnableIntID(LLCE0_ICSR23_IRQn);
}

void Can_Enable_Timestamp(void)
{
	/* Select timestamp source STM_7 and enable timestamp module in TIMESTAMP_CONTROL_REGISTER */
    Reg_Bit_Set32(TST_CTRL_REG_ADDR, TST_SOURCE_SEL_MASK | TST_ENABLE_MASK);

	/*start STM_7*/
	Reg_Bit_Set32(STM7_BASE_ADDR, STM7_ENABLE_MASK);

    /* Set STM Prescaler 133. - This shall offer 1ms ticks */
    Reg_Bit_Set32(STMTS_REG_ADDR, STMTS_SEL_MASK);
}
