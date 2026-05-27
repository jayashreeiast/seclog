/*
 * Copyright 2025 NXP.
 */

#include "gic.h"
#include "S32G399A_A53_COMMON.h"


/*================================================================================================*/
/** 
* @brief vGicInit
* @details Enable GIC500
*/
/*================================================================================================*/
void vGicInit(void)
{
    vGicDistributorInit();
    vGicRedistributorInit_Cpu2();
    vGicCpuInterfaceInit();
}

/*================================================================================================*/
/** 
* @brief vGicDistributorInit
* @details Enable the GIC Distributor
*/
/*================================================================================================*/
void vGicDistributorInit(void)
{
    uint32_t i;
    
    GICD->CTLR &= ~(GICD_CTLR_ENABLE_G0_MASK | GICD_CTLR_ENABLE_G1S_MASK | GICD_CTLR_ENABLE_G1NS_MASK);

    GICD->CTLR |= (GICD_CTLR_ARE_S_MASK | GICD_CTLR_ARE_NS_MASK);

    /* configure SPIs as Group 1 */
    for (i = GIC_MIN_SPI_ID / 32; i < GIC_MAX_SPI_ID / 32; ++i)
    {
        GICD->IGROUPR[i] = 0xFFFFFFFFUL;
        GICD->IGRPMODR[i] = 0UL;
    }

    /* configure SPIs as level-sensitive */
    for(i = GIC_MIN_SPI_ID / 16; i < GIC_MAX_SPI_ID / 16; ++i)
    {
        GICD->ICFGR[i] = 0UL;
    }
    /* configure SPIs with lowest priority (0x00 Highest -> 0xFF Lowest) */
    for(i = GIC_MIN_SPI_ID / 4; i < GIC_MAX_SPI_ID / 4; ++i)
    {
        GICD->IPRIORITYR[i] = GICD_IPRIORITYR_OFFSETx_SET(0xA0,0) | 
                              GICD_IPRIORITYR_OFFSETx_SET(0xA0,1) | 
                              GICD_IPRIORITYR_OFFSETx_SET(0xA0,2) | 
                              GICD_IPRIORITYR_OFFSETx_SET(0xA0,3);
    }

    /* Disable SPIs */
    for (i = GIC_MIN_SPI_ID / 32; i < GIC_MAX_SPI_ID / 32; ++i)
    {
        GICD->ICENABLER[i] = 0xFFFFFFFFUL;
        GICD->ICACTIVER[i] = 0xFFFFFFFFUL;
    }
    /* configure SPIs to be routed to the currently executing core (PE) */
    for(i = 0; i < GIC_MAX_SPI_ID; ++i)
    {
        GICD->IROUTER[i] = 0UL;
    }
    
    GICD->IROUTER[(MSCM_INT1_IRQn - GIC_MIN_SPI_ID)] = (read_MPIDR_EL1() & GICD_IROUTER_AFFx_MASK) & ~GICD_IROUTER_IRM_MASK;
    GICD->IROUTER[(STM0_IRQn - GIC_MIN_SPI_ID)] = (read_MPIDR_EL1() & GICD_IROUTER_AFFx_MASK) & ~GICD_IROUTER_IRM_MASK;
    GICD->IROUTER[(PIT1_IRQn - GIC_MIN_SPI_ID)] = (read_MPIDR_EL1() & GICD_IROUTER_AFFx_MASK) & ~GICD_IROUTER_IRM_MASK;
    
    for(i = (LLCE0_ICSR14_IRQn - GIC_MIN_SPI_ID); i <= (LLCE0_ICSR17_IRQn - GIC_MIN_SPI_ID) ; ++i)
    {
        GICD->IROUTER[i] = (read_MPIDR_EL1() & GICD_IROUTER_AFFx_MASK) & ~GICD_IROUTER_IRM_MASK;
    }

    for(i = (LLCE0_ICSR22_IRQn - GIC_MIN_SPI_ID); i <= (LLCE0_ICSR23_IRQn - GIC_MIN_SPI_ID) ; ++i)
    {
        GICD->IROUTER[i] = (read_MPIDR_EL1() & GICD_IROUTER_AFFx_MASK) & ~GICD_IROUTER_IRM_MASK;
    }

    GICD->CTLR |= GICD_CTLR_ENABLE_G1S_MASK | GICD_CTLR_ENABLE_G1NS_MASK;

}

/*================================================================================================*/
/** 
* @brief vGicRedistributorInit
* @details Enable the GIC Redistributor
*/
/*================================================================================================*/
void vGicRedistributorInit(void)
{
    uint32_t i;

    /* Wake up PE */
    GICR_CPU0->WAKER &= ~GICR_WAKER_PROCESSOR_SLEEP_MASK;
    while(GICR_CPU0->WAKER & GICR_WAKER_CHILDREN_ASLEEP_MASK){};

    /* Disable SGIs/PPIs */
    GICR_SGI_PPI_CPU0->ICENABLER0 = 0xFFFFFFFFUL;

    /* Configure SGIs/PPIs as G1 */
    GICR_SGI_PPI_CPU0->IGROUPR0 = 0xFFFFFFFFUL;
    GICR_SGI_PPI_CPU0->IGRPMODR0 = 0UL;

    /* Confiure SGIs/PPIs with lowest priority (0x00 Highest -> 0xFF lowest) */
    for(i = GIC_MIN_SGI_ID / 4; i < GIC_MIN_SPI_ID / 4; ++i)
    {
        GICR_SGI_PPI_CPU0->IPRIORITYR[i] = GICR_IPRIORITYR_OFFSETx_SET(0xFF, 0) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 1) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 2) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 3);
    }

    /* Configure PPIs as level-sensitive (SGIs are *not* configurable, ICFGR0 is RO) */
    GICR_SGI_PPI_CPU0->ICFGR[1] = 0UL;

    /* SGIs/PPIs will *not* be enabled by default */
}

void vGicRedistributorInit_Cpu1(void)
{
    uint32_t i;
    /* Wake up PE */
    GICR_CPU1->WAKER &= ~GICR_WAKER_PROCESSOR_SLEEP_MASK;
    while(GICR_CPU1->WAKER & GICR_WAKER_CHILDREN_ASLEEP_MASK){};

    /* Disable SGIs/PPIs */
    GICR_SGI_PPI_CPU1->ICENABLER0 = 0xFFFFFFFFUL;

    /* Configure SGIs/PPIs as G1 */
    GICR_SGI_PPI_CPU1->IGROUPR0 = 0xFFFFFFFFUL;
    GICR_SGI_PPI_CPU1->IGRPMODR0 = 0UL;

    /* Confiure SGIs/PPIs with lowest priority (0x00 Highest -> 0xFF lowest) */
    for(i = GIC_MIN_SGI_ID / 4; i < GIC_MIN_SPI_ID / 4; ++i)
    {
        GICR_SGI_PPI_CPU1->IPRIORITYR[i] = GICR_IPRIORITYR_OFFSETx_SET(0xFF, 0) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 1) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 2) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 3);
    }

    /* Configure PPIs as level-sensitive (SGIs are *not* configurable, ICFGR0 is RO) */
    GICR_SGI_PPI_CPU1->ICFGR[1] = 0UL;

    /* SGIs/PPIs will *not* be enabled by default */
}

void vGicRedistributorInit_Cpu2(void)
{
    uint32_t i;
    /* Wake up PE */
    GICR_CPU2->WAKER &= ~GICR_WAKER_PROCESSOR_SLEEP_MASK;
    while(GICR_CPU2->WAKER & GICR_WAKER_CHILDREN_ASLEEP_MASK){};

    /* Disable SGIs/PPIs */
    GICR_SGI_PPI_CPU2->ICENABLER0 = 0xFFFFFFFFUL;

    /* Configure SGIs/PPIs as G1 */
    GICR_SGI_PPI_CPU2->IGROUPR0 = 0xFFFFFFFFUL;
    GICR_SGI_PPI_CPU2->IGRPMODR0 = 0UL;

    /* Confiure SGIs/PPIs with lowest priority (0x00 Highest -> 0xFF lowest) */
    for(i = GIC_MIN_SGI_ID / 4; i < GIC_MIN_SPI_ID / 4; ++i)
    {
        GICR_SGI_PPI_CPU2->IPRIORITYR[i] = GICR_IPRIORITYR_OFFSETx_SET(0xFF, 0) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 1) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 2) | 
                                           GICR_IPRIORITYR_OFFSETx_SET(0xFF, 3);
    }

    /* Configure PPIs as level-sensitive (SGIs are *not* configurable, ICFGR0 is RO) */
    GICR_SGI_PPI_CPU2->ICFGR[1] = 0UL;

    /* SGIs/PPIs will *not* be enabled by default */
}

/*================================================================================================*/
/** 
* @brief vGicCpuInterfaceInit
* @details Enable the GIC CPU Interface
*/
/*================================================================================================*/
void vGicCpuInterfaceInit(void)
{
    /* 
     * Enable lower exception level access to ICC_SRE_ELn
     * Disable IRQ/FIQ bypass (i.e. disable legacy mode)
     * Enable the system register interface for the ICC_*_ELn registers
    */
    write_ICC_SRE_EL3(ICC_SRE_ELx_ENABLE_MASK |
                      ICC_SRE_ELx_DIB_MASK    | 
                      ICC_SRE_ELx_DFB_MASK    | 
                      ICC_SRE_ELx_SRE_MASK);
    ASM_KEYWORD("isb");

    write_ICC_SRE_EL2(ICC_SRE_ELx_ENABLE_MASK |
                      ICC_SRE_ELx_DIB_MASK    | 
                      ICC_SRE_ELx_DFB_MASK    | 
                      ICC_SRE_ELx_SRE_MASK);
    ASM_KEYWORD("isb");

    write_ICC_SRE_EL1(ICC_SRE_ELx_DIB_MASK    | 
                      ICC_SRE_ELx_DFB_MASK    | 
                      ICC_SRE_ELx_SRE_MASK);
    ASM_KEYWORD("isb");

    /* Switch to the other security mode to access the bank of ICC_SRE_EL1 */
    write_SCR_EL3(read_SCR_EL3() ^ SCR_EL3_NS_MASK);
    ASM_KEYWORD("isb");

    write_ICC_SRE_EL1(ICC_SRE_ELx_DIB_MASK    | 
                      ICC_SRE_ELx_DFB_MASK    | 
                      ICC_SRE_ELx_SRE_MASK);
    ASM_KEYWORD("isb");

    /* Switch back to the initial security mode */
    write_SCR_EL3(read_SCR_EL3() ^ SCR_EL3_NS_MASK);
    ASM_KEYWORD("isb");

    /* Set idle priority to lowest: 0xFF */
    write_ICC_PMR_EL1(0xFFUL);

    /* Disable subpriority field */
    write_ICC_BPR0_EL1(0);
    write_ICC_BPR1_EL1(0);

    /* Switch to the other security mode to access the bank of ICC_BPR1_EL1 */
    write_SCR_EL3(read_SCR_EL3() ^ SCR_EL3_NS_MASK);
    ASM_KEYWORD("isb");

    write_ICC_BPR1_EL1(0);

    /* Switch back to the initial security mode */
    write_SCR_EL3(read_SCR_EL3() ^ SCR_EL3_NS_MASK);
    write_SCR_EL3(read_SCR_EL3() & (~SCR_EL3_SMD_MASK));
    ASM_KEYWORD("isb");

    /*
     * Configure G1(S/NS) interrupts to be observable from EL3 instead of returning special INTID values
     * Configure EOIR writes to also provide interrupt deactivation (accesses to DIR are unpredictable)
     * Configure BPRs as not common (i.e. BPR0 affects only G0 interrupts and BPR1 affects only G1NS and G1S interrupts)
     */
    write_ICC_CTLR_EL3(read_ICC_CTLR_EL3() & ~(ICC_CTLR_EL3_RM_MASK            |
                                               ICC_CTLR_EL3_EOIMODE_EL1NS_MASK |
                                               ICC_CTLR_EL3_EOIMODE_EL1S_MASK  |
                                               ICC_CTLR_EL3_EOIMODE_EL3_MASK   |
                                               ICC_CTLR_EL3_CBPR_EL1NS_MASK    |
                                               ICC_CTLR_EL3_CBPR_EL1S_MASK));

    /* Enable the signalling of all interrupt groups */
    write_ICC_IGRPEN0_EL1(ICC_IGRPEN0_EL1_ENABLE_MASK);
    write_ICC_IGRPEN1_EL3(ICC_IGRPEN1_EL3_ENABLE_G1S_MASK |
                          ICC_IGRPEN1_EL3_ENABLE_G1NS_MASK);

    ASM_KEYWORD("isb");
    ASM_KEYWORD("dsb sy");
}

/*================================================================================================*/
/** 
* @brief vGicEnableIntID
* @details Enables the interrupt source number ID
*/
/*================================================================================================*/
void vGicEnableIntID(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;
    
    if(id < GIC_MIN_SPI_ID)
    {
        GICR_SGI_PPI_CPU0->ISENABLER0 = 1U << x;
    }
    else
    {
        GICD->ISENABLER[n] = 1U << x;
    }
}

/*================================================================================================*/
/** 
* @brief vGicDisableIntID
* @details Disables the interrupt source number ID
*/
/*================================================================================================*/
void vGicDisableIntID(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;

    if (id < GIC_MIN_SPI_ID)
    {
        GICR_SGI_PPI_CPU0->ICENABLER0 = 1U << x;
    }
    else
    {
        GICD->ICENABLER[n] = 1U << x;
    }
}

/*================================================================================================*/
/** 
* @brief vGicSetIntPriority
* @details Sets the priority of the specified ID
*/
/*================================================================================================*/
void vGicSetIntPriority(uint32_t id, uint8_t priority)
{
    uint32_t n = id / 4U;
    uint32_t x = id % 4U;
    priority = priority << (8U - GIC_PRIO_BITS_COUNT);

    if (id < GIC_MIN_SPI_ID)
    {
        GICR_SGI_PPI_CPU0->IPRIORITYR[n] = (GICR_SGI_PPI_CPU0->IPRIORITYR[n] & ~GICR_IPRIORITYR_OFFSETx_SET(0xFFU, x))
                                                                 | GICR_IPRIORITYR_OFFSETx_SET(priority, x);
    }
    else
    {
        GICD->IPRIORITYR[n] = (GICD->IPRIORITYR[n] & ~GICD_IPRIORITYR_OFFSETx_SET(0xFFU, x))
                                                   | GICD_IPRIORITYR_OFFSETx_SET(priority, x);
    }
}

/*================================================================================================*/
/** 
* @brief uint8_t ucGicGetIntPriority
* @details Returns the priority of the specified ID
*/
/*================================================================================================*/
uint8_t ucGicGetIntPriority(uint32_t id)
{
    uint32_t n = id / 4U;
    uint32_t x = id % 4U;
    uint8_t priority;

    if (id < GIC_MIN_SPI_ID)
    {
        priority = GICR_IPRIORITYR_OFFSETx_GET(GICR_SGI_PPI_CPU0->IPRIORITYR[n], x);
    }
    else
    {
        priority = GICD_IPRIORITYR_OFFSETx_GET(GICD->IPRIORITYR[n], x);
    }

    return priority >> (8U - GIC_PRIO_BITS_COUNT);
}

/*================================================================================================*/
/** 
* @brief vGicSetIntPending
* @details Sets the pending bit of the specified ID  
*/
/*================================================================================================*/
void vGicSetIntPending(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;

    if (id < GIC_MIN_SPI_ID)
    {
        GICR_SGI_PPI_CPU0->ISPENDR0 = 1U << x;
    }
    else
    {
        GICD->ISPENDR[n] = 1U << x;
    }
}

/*================================================================================================*/
/** 
* @brief vGicClearIntPending
* @details Clears the pending bit of the specified ID  
*/
/*================================================================================================*/
void vGicClearIntPending(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;

    if (id < GIC_MIN_SPI_ID)
    {
        GICR_SGI_PPI_CPU0->ICPENDR0 = 1U << x;
    }
    else
    {
        GICD->ICPENDR[n] = 1U << x;
    }
}

/*================================================================================================*/
/** 
* @brief ucGicGetIntPending
* @details Returns the value of the status bit of the specifed ID  
*/
/*================================================================================================*/
uint8_t ucGicGetIntPending(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;
    uint8_t pending;

    if (id < GIC_MIN_SPI_ID)
    {
        pending = (GICR_SGI_PPI_CPU0->ISPENDR0 & (1U << x)) >> x;
    }
    else
    {
        pending = (GICD->ICPENDR[n] & (1U << x)) >> x;
    }

    return pending;
}

/*================================================================================================*/
/** 
* @brief vGicSetIntGroup
* @details Set the group of the specified interrupt
*/
/*================================================================================================*/
void vGicSetIntGroup(uint32_t id, GIC_IntGroupType group)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;

    switch (group)
    {
        case GIC_INTG_G0:
        {
            if (id < GIC_MIN_SPI_ID)
            {
                GICR_SGI_PPI_CPU0->IGROUPR0  &= ~(1U << x);
                GICR_SGI_PPI_CPU0->IGRPMODR0 &= ~(1U << x);
            }
            else
            {
                GICD->IGROUPR[n]  &= ~(1U << x);
                GICD->IGRPMODR[n] &= ~(1U << x);
            }
            break;
        }

        case GIC_INTG_G1NS:
        {
            if (id < GIC_MIN_SPI_ID)
            {
                GICR_SGI_PPI_CPU0->IGROUPR0  |=   1U << x;
                GICR_SGI_PPI_CPU0->IGRPMODR0 &= ~(1U << x);
            }
            else
            {
                GICD->IGROUPR[n]  |=   1U << x;
                GICD->IGRPMODR[n] &= ~(1U << x);
            }
            break;
        }

        case GIC_INTG_G1S:
        {
            if (id < GIC_MIN_SPI_ID)
            {
                GICR_SGI_PPI_CPU0->IGROUPR0  &= ~(1U << x);
                GICR_SGI_PPI_CPU0->IGRPMODR0 |=   1U << x;
            }
            else
            {
                GICD->IGROUPR[n]  &= ~(1U << x);
                GICD->IGRPMODR[n] |=   1U << x;
            }
            break;
        }
    }
}

/*================================================================================================*/
/** 
* @brief GicGetIntGroup
* @details Get the group of the specified interrupt
*/
/*================================================================================================*/
GIC_IntGroupType GicGetIntGroup(uint32_t id)
{
    uint32_t n = id / 32U;
    uint32_t x = id % 32U;
    uint32_t grp;
    uint32_t grpmod;
    GIC_IntGroupType grpType;

    if (id < GIC_MIN_SPI_ID)
    {
        grp    = GICR_SGI_PPI_CPU0->IGROUPR0  & (1U << x);
        grpmod = GICR_SGI_PPI_CPU0->IGRPMODR0 & (1U << x);
    }
    else
    {
        grp    = GICD->IGROUPR[n]  & (1U << x);
        grpmod = GICD->IGRPMODR[n] & (1U << x);
    }

    if (!grp && !grpmod)     {grpType = GIC_INTG_G0;}
    else if (grp && !grpmod) {grpType = GIC_INTG_G1NS;}
    else if (!grp && grpmod) {grpType = GIC_INTG_G1S;}
    else                     {grpType = GIC_INTG_G1NS;}   /* Reserved, but treated as G1NS */

    return grpType;
}

/*================================================================================================*/
/** 
* @brief vGicSetPriorityMask
* @details Sets the Priority mask register for the core run on. The reset value masks ALL interrupts!   
*/
/*================================================================================================*/
void vGicSetPriorityMask(uint8_t priority)
{
    priority = priority << (8U - GIC_PRIO_BITS_COUNT); 
    write_ICC_PMR_EL1(ICC_PMR_EL1_PRIORITY(priority));
}

/*================================================================================================*/
/** 
* @brief vGicRegisterIsrHandler
* @details Register the Isr Handler to the VTABLE
*/
/*================================================================================================*/
void vGicRegisterIsrHandler(uint16_t irq_id, void (*isr_handler)(void))
{
    /* The 'irq_id' parameter is in x0 and the 'isr_handler' parameter is in x1 according to AAPCS */

    /* x2 = &VTABLE */
    ASM_KEYWORD("ldr x2,=VTABLE");
    /* x2 = &VTABLE[irq_id] */
    ASM_KEYWORD("add x2,x2,x0,lsl #3");
    /* VTABLE[irq_id] = isr_handler */
    ASM_KEYWORD("str x1,[x2]");

    (void)irq_id;
    (void)isr_handler;
}
