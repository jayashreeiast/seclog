/*================================================================================================*/
/**
*    @file        sys_init.c
*    @version     24.12.0
*
*    @brief       Integration Framework - System initialization code
*
*    (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*    All Rights Reserved.
*
*    This file contains sample code only. It is not part of the production code deliverables.
*/
/*=================================================================================================
*    Platform      :      CORTEXM
*    Build Version :      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "sys_init.h"
#include "Mcal.h"
#include "typedefs.h"
#include "S32G399A_COMMON.h"
#include "S32G399A.h"
#include "S32G399A_SCB.h"
#include "S32G399A_MPU.h"
#include "nvic.h"

/*==================================================================================================
*                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                        LOCAL MACROS
==================================================================================================*/
#define NUM_MSCM_SPI_IRQ               ((uint32)240U)
#define MSCM_BASEADDR                  ((uint32)0x40198000UL)
#define MSCM_CPXNUM_REG                (MSCM_BASEADDR + 0x04UL)

/* Access control registers for the coprocessors. These are used for enabling the FPU */
#define SCB_CPACR                       ((uint32)0xE000ED88)
#define SCB_CPACR_CP10_MASK             ((uint32)0x300000U)
#define SCB_CPACR_CP11_MASK             ((uint32)0xC00000U)

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/
#if (MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)
#ifdef D_CACHE_ENABLE
/* M7_0 MPU Config only! */

/* Number of entries in the memory tables */
#define CPU_MPU_MEMORY_COUNT (5U)

/*
  Region  Description               Start       End           Size[KB]  Type              Inner Cache Policy    Outer Cache Policy    Shareable    Executable    Privileged Access    Unprivileged Access
--------  -----------------         ----------  ----------  ----------  ----------------  --------------------  --------------------  -----------  ------------  -------------------  ---------------------
       0  HSE Shared RAM            0x22C00000  0x22C03FFF          16  Normal            None                  None                  Yes          Yes           Read/Write           Read/Write
       1  Non-Cacheable RAM         0x34000000  0x3403FFFF         256  Normal            None                  None                  Yes          Yes           Read/Write           Read/Write
       2  SRAM                      0x34040000  0x3407FFFF         256  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       3  SRAM(stack+heap)          0x34080000  0x34087FFF          32  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       4  Peripherals               0x40000000  0x5FFFFFFF      524288  Device            None                  None                  Yes          No            Read/Write           Read/Write
*/

static const uint32 rbar[CPU_MPU_MEMORY_COUNT] = {0x22C00000UL, 0x34000000UL, 0x34040000UL, 0x34080000UL, 0x40000000UL};
static const uint32 rasr[CPU_MPU_MEMORY_COUNT] = {0x130C001BUL, 0x030C0023UL, 0x030B0023UL, 0x030B001DUL,  0x13050039UL};


#endif /* D_CACHE_ENABLE */
#endif /*(MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/
/* Variable used for debugger to attach at startup */
uint32 CATCH_CORE_RESET = 0x00U;
/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

#if (MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)
static void sys_m7_cache_init(void);

static void sys_m7_cache_disable(void);

static void sys_m7_cache_clean(void);
#endif /*(MCAL_PLATFORM_ARM  == MCAL_ARM_MARCH)*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
#if defined(LINARO) || defined(GCC)
    ASM_KEYWORD(".globl startup_getControlRegisterValueX");
    ASM_KEYWORD("startup_getControlRegisterValueX:");
    ASM_KEYWORD("mrs r0, CONTROL");
    ASM_KEYWORD("bx r14");

    ASM_KEYWORD(".globl startup_getAipsRegisterValueX");
    ASM_KEYWORD("startup_getAipsRegisterValueX:");
    ASM_KEYWORD("mrs r0, IPSR");
    ASM_KEYWORD("bx r14");

    extern uint32 startup_getControlRegisterValueX(void);
    extern uint32 startup_getAipsRegisterValueX(void);
#endif /*defined(LINARO)*/
    extern uint32 startup_getControlRegisterValue(void);
    extern uint32 startup_getAipsRegisterValue(void);
#endif /*MCAL_ENABLE_USER_MODE_SUPPORT*/
void startup_go_to_user_mode(void);

/*================================================================================================*/
/**
* @brief    startup_go_to_user_mode
* @details  Function called from startup.s to switch to user mode if MCAL_ENABLE_USER_MODE_SUPPORT
*           is defined
*/
/*================================================================================================*/
void startup_go_to_user_mode(void)
{
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
    ASM_KEYWORD("svc 0x1");
#endif /*MCAL_ENABLE_USER_MODE_SUPPORT*/
}

/*================================================================================================*/
/**
* @brief Sys_GoToSypervisor
* @details function used to enter to supervisor mode.
            check if it's needed to switch to supervisor mode and make the switch.
            Return 1 if switch was done
*/
/*================================================================================================*/

#ifdef MCAL_ENABLE_USER_MODE_SUPPORT

/* check if it's needed to switch to supervisor mode and make the switch.
Return 1 if switch was done */
uint8 Sys_GoToSupervisor(void)
{
    VAR(uint32, MCU_VAR) u32ControlRegValue;
    VAR(uint32, MCU_VAR) u32AipsRegValue;
    VAR(uint8, MCU_VAR)  u8SwithcToSupervisor;

#if defined(LINARO) || defined(GCC)
    /* if it's 0 then Thread mode is already in supervisor mode */
    u32ControlRegValue = startup_getControlRegisterValueX();
    /* if it's 0 the core is in Thread mode, otherwise in Handler mode */
    u32AipsRegValue = startup_getAipsRegisterValueX();
#else
    /* if it's 0 then Thread mode is already in supervisor mode */
    u32ControlRegValue = startup_getControlRegisterValue();
    /* if it's 0 the core is in Thread mode, otherwise in Handler mode */
    u32AipsRegValue = startup_getAipsRegisterValue();
#endif /*defined(LINARO) || defined(GCC)*/
    /* if core is already in supervisor mode for Thread mode, or running form Handler mode, there is no need to make the switch */
    if((0UL == (u32ControlRegValue & 1)) || (0 < (u32AipsRegValue & 0xFF)))
    {
        u8SwithcToSupervisor = 0U;
    }
    else
    {
        u8SwithcToSupervisor = 1U;
        Mcal_goToSupervisor();
    }
    return u8SwithcToSupervisor;
}

/*================================================================================================*/
/**
* @brief Sys_ToUser_Return
* @details function used to switch back to user mode for Thread mode, return a uint32 value passed as parameter
*/
/*================================================================================================*/
uint32 Sys_GoToUser_Return(uint8 u8SwithcToSupervisor, uint32 u32returnValue)
{
    if(1UL == u8SwithcToSupervisor)
    {
        Mcal_goToUser();
    }
    return u32returnValue;
}

/*make the switch back to user mode for Thread mode, return 0. */
uint32 Sys_GoToUser(void)
{
    Mcal_goToUser();
    return 0UL;
}
#endif /*MCAL_ENABLE_USER_MODE_SUPPORT*/

/*================================================================================================*/
/**
* @brief sys_enableAllInterrupts
* @details function used to enable all interrupts
*/
/*================================================================================================*/
void sys_enableAllInterrupts(void)
{
     OsIf_ResumeAllInterrupts() ;
}

/*================================================================================================*/
/**
* @brief sys_disableAllInterrupts
* @details function used to disable all interrupts
*/
/*================================================================================================*/
void sys_disableAllInterrupts(void)
{
    OsIf_SuspendAllInterrupts() ;
}

/*================================================================================================*/
/**
* @brief sys_disableIsrSource
* @details function used to disable the interrupt number id
*/
/*================================================================================================*/
void sys_disableIsrSource(uint8 id)
{
#if  (MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)
    gic400_disableIntID(id);
#else
    NVIC_DisableIRQ(id);
#endif /*MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64*/
}

/*================================================================================================*/
/**
* @brief sys_init
* @details function used to enable the interrupt number id and set up the priority
*/
/*================================================================================================*/
void sys_enableIsrSource(uint8_t id, uint8_t prio)
{
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
    Mcal_goToSupervisor();
#endif /*MCAL_ENABLE_USER_MODE_SUPPORT*/

#if (MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)
    gic400_enableIntID(id);
    gic400_setIntPriority(id, prio);
    gic400_makeIntGroup0(id);
    gic400_setIntTarget(id, GIC400_TARGET_CPU0);
#else
    NVIC_SetPriority(id, prio);
    NVIC_EnableIRQ(id);
#endif
}

/*================================================================================================*/
/**
* @brief sys_init
* @details function used to register the interrupt handler in the interrupt vector
*/
/*================================================================================================*/
void sys_registerIsrHandler(uint8 irq_id, int_function_t isr_handler)
{
    OsIf_SuspendAllInterrupts() ;
#if  (MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)
    g_INT_vectors[irq_id] = isr_handler;
#else
    *(volatile uint32*)((*(volatile uint32*)(0xE000ED08)) + ((16 + irq_id) << 2)) = (uint32)isr_handler;
#endif
    OsIf_ResumeAllInterrupts() ;
}

/*================================================================================================*/
/**
* @brief sys_init
* @details function used to clear pending interrupt
*/
/*================================================================================================*/
#if  (MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)
void  sys_clearIntPending(uint8 irq_id)
{
    gic500_clearIntPending(irq_id);
}
#endif

/*================================================================================================*/
/*
 * system initialization : system clock, interrupt router ...
 */
#ifdef __ICCARM__
    #pragma default_function_attributes = @ ".systeminit"
#else
    __attribute__ ((section (".systeminit")))
#endif /*__ICCARM__*/

#ifdef __ICCARM__
/* Initialize RAM variables and RAM functions */
void Scatter_Loading(void)
{
    volatile char * from;
    volatile char * to;
    volatile uint32_t length;

#pragma language=extended
    /* Relocate the INT Vect table */
    #pragma section = "AppIntVec_Init"
    #pragma section = "AppIntVec"
    from = (char *)__segment_begin("AppIntVec_Init");
    to = (char *)__segment_begin("AppIntVec");
    length = __segment_size("AppIntVec");

     while(length--)
    {
        *to++ = *from++;

    }

    /* Relocate the  RAM code */
    #pragma section = "AppRAMCode"
    #pragma section = "AppRAMCode_Init"
    from = (char *)__segment_begin("AppRAMCode_Init");
    to = (char *)__segment_begin("AppRAMCode");
    length = __segment_size("AppRAMCode");
    while(length--)
    {
        *to++ = *from++;
    }

    /* Init DATA section */
    #pragma section = "AppData"
    #pragma section = "AppData_Init"
    from = (char *)__segment_begin("AppData_Init");
    to = (char *)__segment_begin("AppData");
    length = __segment_size("AppData");
    while(length--)
    {
        *to++ = *from++;
    }

    /* Init DATA NO_CACHE section */
    #pragma section = "AppData_NO_CACHE"
    #pragma section = "AppData_NO_CACHE_Init"
    from = (char *)__segment_begin("AppData_NO_CACHE_Init");
    to = (char *)__segment_begin("AppData_NO_CACHE");
    length = __segment_size("AppData_NO_CACHE");
    while(length--)
    {
        *to++ = *from++;
    }
    #pragma language=default
}
#endif /* #ifdef __ICCARM__ */

void SystemInit(void)
{
    uint32 i;
    uint8 regionNum = 0U;
    volatile short *mscm_addr;

#if  (MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)

   /* Configure Addresses for GIC_Distributor and Core */
    gic400_setGICAddr((void*)0x7D001000, (void*)0x7D002000);

    /* Configure interrupt controller (GICv2 assumed) */
    gic400_disableGIC();
    gic400_disableCPUInterface();
    gic400_enableGIC();
    gic400_enableCPUInterface();
    gic400_enableNonSecureCPUInterface();
    /* Configure priority mask to lowest value - enables all priorities */

    gic400_setPriorityMask(0xFF);
    /*gic400_enableSecureFIQs();*/

    /* fill the interrupts_vector with the default interrupt routine */
    for (i=0U; i<INT_NUM_SOURCES; i++)
    {
        g_INT_vectors[i] = (int_function_t)default_interrupt_routine;
    }

#else

#ifdef ENABLE_FPU
    /* Enable CP10 and CP11 coprocessors */
    REG_WRITE32(SCB_CPACR, REG_READ32(SCB_CPACR) | SCB_CPACR_CP10_MASK | SCB_CPACR_CP11_MASK);
#endif /* ENABLE_FPU */

    NVIC_SetPriorityGrouping(0); /* 0 means 7 bits for prio, 1 for sub-prio */

#endif /*(MCAL_PLATFORM_ARM  == MCAL_ARM_AARCH64)*/

    /* fill the interrupts_vector with the default interrupt routine */
    for (i = 0U; i < NUM_MSCM_SPI_IRQ; i++)
    {
        mscm_addr  = (volatile short*)(MSCM_BASEADDR + 0x880U + (i*2));
        *mscm_addr = 0x0007U;
    }

#if (MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)
#ifdef D_CACHE_ENABLE
/**************************************************************************/
                      /* DEFAULT MEMORY ENABLE*/
/**************************************************************************/
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();

    /*Checking if cache is enable before*/
    if (((((uint32)1U << (uint32)17U) & S32_SCB->CCR) != (uint32)0) || ((((uint32)1U << (uint32)16U) & S32_SCB->CCR) != (uint32)0))
    {
        /* synchronize cache before update mpu */
        sys_m7_cache_disable();
    }

    /* Set default memory regions */
    for (regionNum = 0U; regionNum < CPU_MPU_MEMORY_COUNT; regionNum++)
    {
        S32_MPU->RNR  = regionNum;
        S32_MPU->RBAR = rbar[regionNum];
        S32_MPU->RASR = rasr[regionNum];
    }

    /* Enable MPU */
    S32_MPU->CTRL |= S32_MPU_CTRL_ENABLE_MASK;
    MCAL_INSTRUCTION_SYNC_BARRIER();
    MCAL_DATA_SYNC_BARRIER();
#endif /* D_CACHE_ENABLE */

/**************************************************************************/
            /* ENABLE CACHE */
/**************************************************************************/
    sys_m7_cache_init();

#endif /*(MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)*/
}

#if (MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)
__attribute__ ((section (".systeminit")))

static void sys_m7_cache_init(void)
{
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;

#ifdef D_CACHE_ENABLE
    /*init Data caches*/
    S32_SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    MCAL_DATA_SYNC_BARRIER();
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        MCAL_DATA_SYNC_BARRIER();
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    MCAL_DATA_SYNC_BARRIER();
    S32_SCB->CCR |=  (uint32)SCB_CCR_DC_Msk;  /* enable D-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /* D_CACHE_ENABLE */

#ifdef I_CACHE_ENABLE
    /*init Code caches*/
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
    S32_SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
    S32_SCB->CCR |=  (uint32)SCB_CCR_IC_Msk;  /* enable I-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /* I_CACHE_ENABLE */
}

static void sys_m7_cache_disable(void)
{
    sys_m7_cache_clean();
    S32_SCB->CCR &= ~((uint32)1U << 17U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
    S32_SCB->CCR &= ~((uint32)1U << 16U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
}

static void sys_m7_cache_clean(void)
{
#ifdef D_CACHE_ENABLE
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;

    S32_SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    MCAL_DATA_SYNC_BARRIER();
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCCISW = (((sets << SCB_DCCISW_SET_Pos) & (uint32)SCB_DCCISW_SET_Msk) |
                      ((ways << SCB_DCCISW_WAY_Pos) & (uint32)SCB_DCCISW_WAY_Msk)  );
        MCAL_DATA_SYNC_BARRIER();
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    MCAL_DATA_SYNC_BARRIER();
    S32_SCB->CSSELR = (uint32)((S32_SCB->CSSELR) | 1U);
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /*D_CACHE_ENABLE*/

#ifdef I_CACHE_ENABLE
    S32_SCB->ICIALLU = 0UL;
#endif /*I_CACHE_ENABLE*/
    MCAL_DATA_SYNC_BARRIER();
}
#endif /*(MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)*/

/*================================================================================================*/
/**
@brief  Function reads the platform identity and returns CPU number value

@param[in]  none

@return core ID
*/
/*================================================================================================*/
uint8 Sys_GetCoreID(void)
{
    return REG_READ32(MSCM_CPXNUM_REG);
}

/*================================================================================================*/
/**
* @brief sys_halt
* @details function used to enter halt mode
*/
/*================================================================================================*/
void sys_halt(void)
{
    EXECUTE_WAIT();
}
/*================================================================================================*/
/**
* @brief sys_stop
* @details function used to enter stop mode
*/
/*================================================================================================*/
void sys_stop(void)
{
    EXECUTE_WAIT();
}

#ifdef __ICCARM__
    #pragma default_function_attributes =
#endif /* ICCARM */

#ifdef __cplusplus
}
#endif
