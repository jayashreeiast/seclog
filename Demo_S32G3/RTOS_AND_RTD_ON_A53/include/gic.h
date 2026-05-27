/*
 * Copyright 2025 NXP.
 */
#ifndef GIC_H
#define GIC_H

#include <stdint.h>
#include "access_macros.h"

typedef enum {
    GIC_INTG_G0,
    GIC_INTG_G1NS,
    GIC_INTG_G1S

} GIC_IntGroupType;

#define GIC_MIN_SGI_ID                  (0U)
#define GIC_MIN_PPI_ID                  (16U)
#define GIC_MIN_SPI_ID                  (32U)
#define GIC_MAX_SPI_ID                  (960U)
#define GIC_CPU_PER_CLUSTER_COUNT       (1U)
#define GIC_PRIO_BITS_COUNT             (4U)
#define GIC_CPU_COUNT                   (4U)

#define GIC500_BASE_ADDR                (0x50800000U)
#define GICD_OFFSET                     (0x0U)
#define GICD_BASE_ADDR                  (GIC500_BASE_ADDR + GICD_OFFSET)

#define GICR_CPU0_OFFSET                (0x00100000U)
#define GICR_CPU0_BASE_ADDR             (GIC500_BASE_ADDR + GICR_CPU0_OFFSET)
#define GICR_CPU0_SGI_PPI_OFFSET        (0x00110000U)
#define GICR_CPU0_SGI_PPI_BASE_ADDR     (GIC500_BASE_ADDR + GICR_CPU0_SGI_PPI_OFFSET)

#define GICR_CPU1_OFFSET                (0x00120000ul)
#define GICR_CPU1_BASE_ADDR             (GIC500_BASE_ADDR + GICR_CPU1_OFFSET)
#define GICR_CPU1_SGI_PPI_OFFSET        (0x00130000ul)
#define GICR_CPU1_SGI_PPI_BASE_ADDR     (GIC500_BASE_ADDR + GICR_CPU1_SGI_PPI_OFFSET)

#define GICR_CPU2_OFFSET                (0x00180000ul)
#define GICR_CPU2_BASE_ADDR             (GIC500_BASE_ADDR + GICR_CPU2_OFFSET)
#define GICR_CPU2_SGI_PPI_OFFSET        (0x00190000ul)
#define GICR_CPU2_SGI_PPI_BASE_ADDR     (GIC500_BASE_ADDR + GICR_CPU2_SGI_PPI_OFFSET)

#define GICR_CPU3_OFFSET                (0x001A0000ul)
#define GICR_CPU3_BASE_ADDR             (GIC500_BASE_ADDR + GICR_CPU3_OFFSET)
#define GICR_CPU3_SGI_PPI_OFFSET        (0x001B0000ul)
#define GICR_CPU3_SGI_PPI_BASE_ADDR     (GIC500_BASE_ADDR + GICR_CPU3_SGI_PPI_OFFSET)

#define GICD                            ((GICD_Type *)GICD_BASE_ADDR)
#define GICR_CPU0                       ((GICR_Type *)GICR_CPU0_BASE_ADDR)
#define GICR_SGI_PPI_CPU0               ((GICR_SGI_PPI_Type *)GICR_CPU0_SGI_PPI_BASE_ADDR)


#define GICR_CPU1                       ((GICR_Type *)GICR_CPU1_BASE_ADDR)
#define GICR_SGI_PPI_CPU1               ((GICR_SGI_PPI_Type *)GICR_CPU1_SGI_PPI_BASE_ADDR)

#define GICR_CPU2                       ((GICR_Type *)GICR_CPU2_BASE_ADDR)
#define GICR_SGI_PPI_CPU2               ((GICR_SGI_PPI_Type *)GICR_CPU2_SGI_PPI_BASE_ADDR)

#define GICR_CPU3                       ((GICR_Type *)GICR_CPU3_BASE_ADDR)
#define GICR_SGI_PPI_CPU3               ((GICR_SGI_PPI_Type *)GICR_CPU3_SGI_PPI_BASE_ADDR)

#define GICD_CTLR_ENABLE_G0_MASK        (0x1U)
#define GICD_CTLR_ENABLE_G1NS_MASK      (0x2U)
#define GICD_CTLR_ENABLE_G1S_MASK       (0x4U)
#define GICD_CTLR_ARE_S_MASK            (0x10U)
#define GICD_CTLR_ARE_NS_MASK           (0x20U)
#define GICD_CTLR_DS_MASK               (0x40U)
#define GICD_CTLR_RWP_MASK              (0x80000000U)
#define GICD_IROUTER_AFFx_MASK          (0xFF00FFFFFFUL)
#define GICD_IROUTER_IRM_MASK           (0x80000000UL)
#define GICD_IROUTER_AFF1_MASK          (0x000000FF00UL)
#define GICD_IROUTER_AFF1_SHIFT         (8UL)
#define GICD_IROUTER_AFF0_MASK          (0xFF000000FFUL)
#define GICD_IROUTER_AFF0_SHIFT         (0UL)
#define GICD_IPRIORITYR_OFFSETx_SET(p,x)    (((uint32_t)(((uint32_t)(p)) << (8U * (x)))) & (0xFFU << (8U * (x))))
#define GICD_IPRIORITYR_OFFSETx_GET(p,x)    (((uint32_t)(((uint32_t)(p)) & (0xFFU << (8U * (x))))) >> (8U * (x)))

#define GICR_WAKER_PROCESSOR_SLEEP_MASK  (0x2U)
#define GICR_WAKER_CHILDREN_ASLEEP_MASK  (0x4U)

#define GICR_IPRIORITYR_OFFSETx_SET(p,x)    (((uint32_t)(((uint32_t)(p)) << (8U * (x)))) & (0xFFU << (8U * (x))))
#define GICR_IPRIORITYR_OFFSETx_GET(p,x)    (((uint32_t)(((uint32_t)(p)) & (0xFFU << (8U * (x))))) >> (8U * (x)))

#define ICC_SRE_ELx_ENABLE_MASK          (0x8UL)
#define ICC_SRE_ELx_DIB_MASK             (0x4UL)
#define ICC_SRE_ELx_DFB_MASK             (0x2UL)
#define ICC_SRE_ELx_SRE_MASK             (0x1UL)
#define SCR_EL3_NS_MASK                  (0x1UL)
#define SCR_EL3_SMD_MASK                 (0x80UL)
#define ICC_CTLR_EL3_RM_MASK             (0x20UL)
#define ICC_CTLR_EL3_EOIMODE_EL1NS_MASK  (0x10UL)
#define ICC_CTLR_EL3_EOIMODE_EL1S_MASK   (0x8UL)
#define ICC_CTLR_EL3_EOIMODE_EL3_MASK    (0x4UL)
#define ICC_CTLR_EL3_CBPR_EL1NS_MASK     (0x2UL)
#define ICC_CTLR_EL3_CBPR_EL1S_MASK      (0x1UL)
#define ICC_IGRPEN0_EL1_ENABLE_MASK      (0x1UL)
#define ICC_IGRPEN1_EL3_ENABLE_G1S_MASK  (0x2UL)
#define ICC_IGRPEN1_EL3_ENABLE_G1NS_MASK (0x1UL)

#define ICC_PMR_EL1_PRIORITY_MASK           (0xFFUL)
#define ICC_PMR_EL1_PRIORITY_SHIFT          (0UL)
#define ICC_PMR_EL1_PRIORITY(x)             (((uint64_t)(((uint64_t)(x)) << ICC_PMR_EL1_PRIORITY_SHIFT)) & ICC_PMR_EL1_PRIORITY_MASK)



/*==================================================================================================
*                                          USER TYPES
==================================================================================================*/
typedef struct
{
          volatile uint32_t CTLR;               /* +0x0000 - RW - Distributor Control Register */
    const volatile uint32_t TYPER;              /* +0x0004 - RO - Interrupt Controller Type Register */
    const volatile uint32_t IIDR;               /* +0x0008 - RO - Distributor Implementer Identification Register */
    const volatile uint32_t padding0[13];       /* Reserved */
          volatile uint32_t SETSPI_NSR;         /* +0x0040 - WO - Non-Secure SPI Set Register (Used when SPI is signalled using MSI) */
    const volatile uint32_t padding1;           /* Reserved */
          volatile uint32_t CLRSPI_NSR;         /* +0x0048 - WO - Non-Secure SPI Clear Register (Used when SPI is signalled using MSI) */
    const volatile uint32_t padding2;           /* Reserved */
          volatile uint32_t SETSPI_SR;          /* +0x0050 - WO - Secure SPI Set Register (Used when SPI is signalled using MSI) */
    const volatile uint32_t padding3;           /* Reserved */
          volatile uint32_t CLRSPI_SR;          /* +0x0058 - WO - Secure SPI Clear Register (Used when SPI is signalled using MSI) */
    const volatile uint32_t padding4[9];        /* Reserved */
          volatile uint32_t IGROUPR[31];        /* +0x0080 - RW - Interrupt Group Registers */
    const volatile uint32_t padding5;           /* Reserved */
          volatile uint32_t ISENABLER[31];      /* +0x0100 - RW - Interrupt Set-Enable Registers */
    const volatile uint32_t padding6;           /* Reserved */
          volatile uint32_t ICENABLER[31];      /* +0x0180 - RW - Interrupt Clear-Enable Registers */
    const volatile uint32_t padding7;           /* Reserved */
          volatile uint32_t ISPENDR[31];        /* +0x0200 - RW - Interrupt Set-Pending Registers  */
    const volatile uint32_t padding8;           /* Reserved */
          volatile uint32_t ICPENDR[31];        /* +0x0280 - RW - Interrupt Clear-Pending Registers */
    const volatile uint32_t padding9;           /* Reserved */
          volatile uint32_t ISACTIVER[31];      /* +0x0300 - RW - Interrupt Set-Active Register */
    const volatile uint32_t padding10;          /* Reserved */
          volatile uint32_t ICACTIVER[31];      /* +0x0380 - RW - Interrupt Clear-Active Register */
    const volatile uint32_t padding11;          /* Reserved */
          volatile uint32_t IPRIORITYR[248];    /* +0x0400 - RW - Interrupt Priority Registers */
    const volatile uint32_t padding12[8];       /* Reserved */
          volatile uint32_t ITARGETSR[248];     /* +0x0800 - RW - Interrupt Processor Targets Registers */
    const volatile uint32_t padding13[8];       /* Reserved */
          volatile uint32_t ICFGR[62];          /* +0x0C00 - RW - Interrupt Configuration Registers */
    const volatile uint32_t padding14[2];       /* Reserved */
          volatile uint32_t IGRPMODR[31];       /* +0x0D00 - RW - Interrupt Group Modifier Registers */
    const volatile uint32_t padding15[33];      /* Reserved */
          volatile uint32_t NSACR[62];          /* +0x0E00 - RW - Non-Secure Access Control Registers */
    const volatile uint32_t padding16[2];       /* Reserved */
          volatile uint32_t SGIR;               /* +0x0F00 - WO - Software Generated Interrupt Register */
    const volatile uint32_t padding17[3];       /* Reserved */
          volatile uint32_t CPENDSGIR[4];       /* +0x0F10 - RW - SGI Clear-Pending Registers */
          volatile uint32_t SPENDSGIR[4];       /* +0x0F20 - RW - SGI Set-Pending Registers */
    const volatile uint32_t padding18[5236];    /* Reserved */
          volatile uint64_t IROUTER[960];       /* +0x6100 - RW - Interrupt Routing Registers (Controls SPI routing when ARE=1) */
    const volatile uint32_t padding19[4160];    /* Reserved */
    const volatile uint32_t ESTATUSR;           /* +0xC000 - RO - Extended Status Register*/
          volatile uint32_t ERRTESTR;           /* +0xC004 - WO - Error Test Register*/
    const volatile uint32_t padding20[31];      /* Reserved */
    const volatile uint32_t SPISR[30];          /* +0xC084 - RO - GIC-500 Shared Peripheral Interrupt Status Registers */
    const volatile uint32_t padding21[4021];    /* Reserved */
    const volatile uint32_t PIDR4;              /* +0xFFD0 - RO - Peripheral ID 4 Register */
    const volatile uint32_t PIDR5;              /* +0xFFD4 - RO - Peripheral ID 5 Register */
    const volatile uint32_t PIDR6;              /* +0xFFD8 - RO - Peripheral ID 6 Register */
    const volatile uint32_t PIDR7;              /* +0xFFDC - RO - Peripheral ID 7 Register */
    const volatile uint32_t PIDR0;              /* +0xFFE0 - RO - Peripheral ID 0 Register */
    const volatile uint32_t PIDR1;              /* +0xFFE4 - RO - Peripheral ID 1 Register */
    const volatile uint32_t PIDR2;              /* +0xFFE8 - RO - Peripheral ID 2 Register */
    const volatile uint32_t PIDR3;              /* +0xFFEC - RO - Peripheral ID 3 Register */
    const volatile uint32_t CIDR0;              /* +0xFFF0 - RO - Component ID 0 Register */
    const volatile uint32_t CIDR1;              /* +0xFFF4 - RO - Component ID 1 Register */
    const volatile uint32_t CIDR2;              /* +0xFFF8 - RO - Component ID 2 Register */
    const volatile uint32_t CIDR3;              /* +0xFFFC - RO - Component ID 3 Register */
} GICD_Type;

typedef struct
{
          volatile uint32_t CTLR;               /* +0x0000 - RW - Redistributor Control Register */
    const volatile uint32_t IIDR;               /* +0x0004 - RO - Redistributor Implementer Identification Register */
    const volatile uint64_t TYPER;              /* +0x0008 - RO - Redistributor Type Register */
    const volatile uint32_t padding0;           /* Reserved */
          volatile uint32_t WAKER;              /* +0x0014 - RW - Power Management Control Register */
    const volatile uint32_t padding1[22];       /* Reserved */
          volatile uint64_t PROPBASER;          /* +0x0070 - RW - Common LPI Configuration Table Register */
          volatile uint64_t PENDBASER;          /* +0x0078 - RW - LPI Pending Table Base Register */
    const volatile uint32_t padding2[16340];    /* Reserved */
    const volatile uint32_t PIDR4;              /* +0xFFD0 - RO - Peripheral ID 4 Register */
    const volatile uint32_t PIDR5;              /* +0xFFD4 - RO - Peripheral ID 5 Register */
    const volatile uint32_t PIDR6;              /* +0xFFD8 - RO - Peripheral ID 6 Register */
    const volatile uint32_t PIDR7;              /* +0xFFDC - RO - Peripheral ID 7 Register */
    const volatile uint32_t PIDR0;              /* +0xFFE0 - RO - Peripheral ID 0 Register */
    const volatile uint32_t PIDR1;              /* +0xFFE4 - RO - Peripheral ID 1 Register */
    const volatile uint32_t PIDR2;              /* +0xFFE8 - RO - Peripheral ID 2 Register */
    const volatile uint32_t PIDR3;              /* +0xFFEC - RO - Peripheral ID 3 Register */
    const volatile uint32_t CIDR0;              /* +0xFFF0 - RO - Component ID 0 Register */
    const volatile uint32_t CIDR1;              /* +0xFFF4 - RO - Component ID 1 Register */
    const volatile uint32_t CIDR2;              /* +0xFFF8 - RO - Component ID 2 Register */
    const volatile uint32_t CIDR3;              /* +0xFFFC - RO - Component ID 3 Register */
} GICR_Type;

typedef struct
{
    const volatile uint32_t padding0[32];       /* Reserved */
          volatile uint32_t IGROUPR0;           /* +0x0080 - RW - Interrupt Group Registers */
    const volatile uint32_t padding1[31];       /* Reserved */
          volatile uint32_t ISENABLER0;         /* +0x0100 - RW - Interrupt Set-Enable Registers */
    const volatile uint32_t padding2[31];       /* Reserved */
          volatile uint32_t ICENABLER0;         /* +0x0180 - RW - Interrupt Clear-Enable Registers */
    const volatile uint32_t padding3[31];       /* Reserved */
          volatile uint32_t ISPENDR0;           /* +0x0200 - RW - Interrupt Set-Pending Registers */
    const volatile uint32_t padding4[31];       /* Reserved */
          volatile uint32_t ICPENDR0;           /* +0x0280 - RW - Interrupt Clear-Pending Registers */
    const volatile uint32_t padding5[31];       /* Reserved */
          volatile uint32_t ISACTIVER0;         /* +0x0300 - RW - Interrupt Set-Active Register */
    const volatile uint32_t padding6[31];       /* Reserved */
          volatile uint32_t ICACTIVER0;         /* +0x0380 - RW - Interrupt Clear-Active Register */
    const volatile uint32_t padding7[31];       /* Reserved */
          volatile uint32_t IPRIORITYR[8];      /* +0x0400 - RW - Interrupt Priority Registers */
    const volatile uint32_t padding8[504];      /* Reserved */
          volatile uint32_t ICFGR[2];           /* +0x0C00 - RW - Interrupt Configuration Registers */
    const volatile uint32_t padding9[62];       /* Reserved */
          volatile uint32_t IGRPMODR0;          /* +0x0D00 - RW - Interrupt Group Modifier Register */
    const volatile uint32_t padding10[63];      /* Reserved */
          volatile uint32_t NSACR;              /* +0x0E00 - RW - Non-Secure Access Control Register */
    const volatile uint32_t padding11[11391];   /* Reserved */
    const volatile uint32_t MISCSTATUSR;        /* +0xC000 - RO - Miscellaneous Status Register */
    const volatile uint32_t padding12[31];      /* Reserved */
    const volatile uint32_t PPISR;              /* +0xC080 - RO - Private Peripheral Interrupt Status Register */
    const volatile uint32_t padding13[4063];    /* Reserved */
} GICR_SGI_PPI_Type;


/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/
/* ACCESS THE CPU INTERFACE USING SYSTEM REGISTER INTERFACE */
uint32_t read_SCR_EL3(void);
void write_SCR_EL3(uint32_t);
uint64_t read_MPIDR_EL1(void);

void write_ICC_SGI0R_EL1(uint64_t);
void write_ICC_SGI1R_EL1(uint64_t);
void write_ICC_ASGI1R_EL1(uint64_t);
void write_ICC_SRE_EL1(uint64_t);
void write_ICC_SRE_EL2(uint64_t);
void write_ICC_SRE_EL3(uint64_t);
void write_ICC_PMR_EL1(uint64_t);
void write_ICC_BPR0_EL1(uint64_t);
void write_ICC_BPR1_EL1(uint64_t);
void write_ICC_IGRPEN0_EL1(uint64_t);
void write_ICC_IGRPEN1_EL3(uint64_t);
uint64_t read_ICC_CTLR_EL3(void);
void write_ICC_CTLR_EL3(uint64_t);
uint32_t read_SCTLR_EL3(void);
void write_SCTLR_EL3(uint32_t);
uint64_t read_CPUECTLR_EL1(void);
void write_CPUECTLR_EL1(uint64_t);
uint32_t read_CSSELR_EL1(void);
void write_CSSELR_EL1(uint32_t);
uint32_t read_CCSIDR_EL1(void);
void write_DCCISW(uint32_t);
void write_ICIALLU(void);
void write_CPTR_EL3(uint32_t);
void write_CPTR_EL2(uint32_t);
void write_CPACR_EL1(uint32_t);
void write_MAIR_EL3(uint64_t);
void write_TCR_EL3(uint32_t);
void write_TTBR0_EL3(uint64_t);


void vGicInit(void);
void vGicDistributorInit(void);
void vGicRedistributorInit(void);
void vGicCpuInterfaceInit(void);

void vGicRedistributorInit_Cpu1(void);
void vGicRedistributorInit_Cpu2(void);


void vGicEnableIntID(uint32_t id);
void vGicDisableIntID(uint32_t id);
void vGicSetIntPriority(uint32_t id, uint8_t priority);
uint8_t ucGicGetIntPriority(uint32_t id);
void vGicSetIntPending(uint32_t id);
void vGicClearIntPending(uint32_t id);
uint8_t ucGicGetIntPending(uint32_t id);
void vGicSetIntGroup(uint32_t id, GIC_IntGroupType group);
GIC_IntGroupType GicGetIntGroup(uint32_t id);
void vGicSetPriorityMask(uint8_t priority);
void vGicRegisterIsrHandler(uint16_t irq_id, void (*isr_handler)(void));

#endif /* GIC_H */
