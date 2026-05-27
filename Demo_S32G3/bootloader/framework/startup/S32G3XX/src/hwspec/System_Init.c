/**************************************************************************************
*
*   Freescale(TM) and the Freescale logo are trademarks of Freescale Semiconductor, Inc.
*   All other product or service names are the property of their respective owners.
*   (c) Copyright 2016 Freescale Semiconductor Inc.
*   All Rights Reserved.
*
*   You can use this example for any purpose on any computer system with the
*   following restrictions:
*
*   1. This example is provided "as is", without warranty.
*
*   2. You don't remove this copyright notice from this example or any direct derivation
*      thereof.
*
*  Description:  S32GXX system initialization code
*
**************************************************************************************/

#include "Mcal.h"
#include "s32gxx_ip.h"
#include "s32gxx_interrupt_router.h"
#include <Os_platform_base.h>

/*Allocate a global variable which will be overwritten by the debugger if attached(in CMM), to catch the core after reset.*/
unsigned int CATCH_CORE_RESET = 0x00U;


#define MC_CGM_MUX_3_DC_0       *(volatile unsigned int*)(OSMC_CGM_BASE + 0x3C8u)    /*define clock generation  - mux 3 module register address*/
#define MC_ME_CTL_KEY           *(volatile unsigned int*)(OSMC_ME_BASE + 0x000u)     /*define partition 0 control key register address*/
#define MC_ME_PRTN0_PCONF       *(volatile unsigned int*)(OSMC_ME_BASE + 0x100u)     /*define partition 0 clock enable configuration register address*/
#define MC_ME_PRTN0_PUPD        *(volatile unsigned int*)(OSMC_ME_BASE + 0x104u)     /*define partition 0 clock update configuration register address */

/*set NVIC control register base addres*/
#define SCB 0xE000ED00

/* Application Interrupt and Reset Control Register */
#define SCB_AIRCR_OFFSET 0x0C

/* Configuration Control Register */
#define SCB_CCR_OFFSET   0x14

/* 0x018 RW System Handlers Priority Registers (4-7, 8-11, 12-15) */
#define SCB_SHP_OFFSET   0x18

/**
@brief      NVIC_SetPriorityGrouping - NVIC set priority grouping
@details    Function used to enable interrupts and set priority level
            
@return     None
*/
__attribute__ ((section (".systeminit"))) 
static void NVIC_SetPriorityGrouping(unsigned int PriorityGroup)
{
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  REG_WRITE32(SCB+SCB_AIRCR_OFFSET, (unsigned int)0x05FA0000 | PriorityGroup);
}

/** 
* @brief sys_enableAllInterrupts
* @details function used to enable all interrupts
*/
/*================================================================================================*/
void sys_enableAllInterrupts(void)
{
     ResumeAllInterrupts();
}

/*================================================================================================*/
/** 
* @brief sys_disableAllInterrupts
* @details function used to disable all interrupts
*/
/*================================================================================================*/
void sys_disableAllInterrupts(void)
{
    SuspendAllInterrupts();
}

/**
@brief      SystemInit - System Initialization function
@details    Function used for system initialization : system clock, interrupt router ...
            
@return     None
*/
__attribute__ ((section (".systeminit"))) 
void SystemInit(void)
{  
    NVIC_SetPriorityGrouping(0); /* 0 means 7 bits for prio, 1 for sub-prio */
    
    REG_WRITE32(SCB+SCB_SHP_OFFSET, 0x00);
    REG_WRITE32(SCB+SCB_CCR_OFFSET, REG_READ32(SCB+SCB_CCR_OFFSET)| 1);

    /* route all interrupts to cores */
    IR_ROUTE_ALL_2_CPU( IR_ROUTE_2_CPU2 | IR_ROUTE_2_CPU1 | IR_ROUTE_2_CPU0 );
  
    /* enable clock for both FTMs */
    MC_ME_PRTN0_PUPD = 0x1; /* PCUD */
    MC_ME_PRTN0_PCONF = 0x1; /* PCE */
    MC_ME_CTL_KEY = 0x5AF0;
    MC_ME_CTL_KEY = 0xA50F;

}

/* SRAM Controller which is in charge of 0x34000000 - 0x34400000 address space */
#define SRAM_0_CONTROLLER_BASE               0x4019C000

/* SRAM Controller which is in charge of 0x34400000 - 0x34800000 address space */
#define SRAM_1_CONTROLLER_BASE               0x401A0000

/* Offset from the base register which stores the first address which will be initialized */
#define SRAM_CONTROLLER_START_ADDR_OFFSET    0x4

/* Offset from the base register which stores the last address which will be initialized */
#define SRAM_CONTROLLER_END_ADDR_OFFSET      0x8

/* Register which stores the status of the current operation */
#define SRAM_CONTROLLER_STATUS_REG_OFFSET    0xC

/* This buffer is used by the bootloader in order to make sure that applications don't overlap */
#define SRAM_APPLICATION_BUFFER              0x1FF

/* Maximum end address which doesn't trigger a full initialization per controller */
#define SRAM_MAXIMUM_RANGE                   0x3FFF80

/* Command given to the SRAM Controller to start intialization */
#define SRAM_CONTROLLER_WRITE_CMD            0xFFFFFFFF

/* Command which will be stored in SRAM end address register in order to initialize all available memory */
#define SRAM_CONTROLLER_END_ADDR_CMD         0x1FFFF

/* Command which will be stored in SRAM start address register in order to initialize all available memory */
#define SRAM_CONTROLLER_BEGIN_ADDR_CMD       0x0

/* Shift needed for the 128-byte cuanta used by the SRAM controller at initialization */
#define SRAM_ADDR_SHIFT                      0x7

/* First address in the SRAM */
#define SRAM_START                           0x34000000

/* Memory size for each SRAM controller */
#define SRAM_CONTROLLER_SIZE                 0x400000

/* ID of the first SRAM controller */
#define SRAM_0_CONTROLLER_ID                 0x0

/* ID of the second SRAM controller */
#define SRAM_1_CONTROLLER_ID                 0x1

/* Total SRAM size */
#define SRAM_TOTAL_SIZE                      0x800000


/*================================================================================================*/
/**
* @brief   Initialize destination RAM memory
* @details Support function used by Sys to intialize SRAM memory
*
* @param[in]     Sys_RelativeStartAddress   Application start address relative to SRAM memory start
* @param[in]     Sys_RelativeEndAddress     Application end address relative to SRAM memory start
* @param[in]     controllerID              SRAM controller ID
*
* @param[out]    None
*/
/*================================================================================================*/
void Sys_Init_SRAM_Controller(int Sys_RelativeStartAddress, int Sys_RelativeEndAddress, char controllerID)
{
    VAR(unsigned int, AUTOMATIC) controllerBaseAddr;

    if(SRAM_0_CONTROLLER_ID == controllerID)
    {
        controllerBaseAddr = SRAM_0_CONTROLLER_BASE;
    } 
    else if(SRAM_1_CONTROLLER_ID == controllerID) 
    {
        controllerBaseAddr = SRAM_1_CONTROLLER_BASE;
        /* Substract SRAM_CONTROLLER_SIZE, as SRAM1 controller is in charge of memory from 0x34400000 to 0x34800000*/
        Sys_RelativeStartAddress = Sys_RelativeStartAddress - SRAM_CONTROLLER_SIZE;
        Sys_RelativeEndAddress = Sys_RelativeEndAddress - SRAM_CONTROLLER_SIZE;
    }
    
    /* Write start address */
    REG_WRITE32(controllerBaseAddr + SRAM_CONTROLLER_START_ADDR_OFFSET, (Sys_RelativeStartAddress >> SRAM_ADDR_SHIFT));
    
    /* Write end addresses */
    if(Sys_RelativeEndAddress < SRAM_MAXIMUM_RANGE)
    {
        /* This was not a command which initializes all the memory */
        REG_WRITE32(controllerBaseAddr + SRAM_CONTROLLER_END_ADDR_OFFSET, ((Sys_RelativeEndAddress + SRAM_APPLICATION_BUFFER) >> SRAM_ADDR_SHIFT));
    }
    else 
    {
        /* This command initializes all the available memory */
        REG_WRITE32(controllerBaseAddr + SRAM_CONTROLLER_END_ADDR_OFFSET, SRAM_CONTROLLER_END_ADDR_CMD);
    }
    /* Clear all the errors and start ram initialization */
    REG_WRITE32(controllerBaseAddr, SRAM_CONTROLLER_WRITE_CMD);
    /* Wait for operation to be completed */
    while(0x00 == (REG_READ32(controllerBaseAddr + SRAM_CONTROLLER_STATUS_REG_OFFSET) & 0x01));
    
}

/*================================================================================================*/
/**
@brief         Initialize IPC destination RAM
@details       Function used to initialize IPC shared memory

@param[in]     None
@param[out]    None
*/
/*================================================================================================*/
void Sys_Init_SRAM(int Sys_StartAddress, unsigned int size)
{
    /* Translate from ram addresses to relative addresess from the base */
    int startAddr = Sys_StartAddress - SRAM_START;
    int endAddr   = startAddr + size;
        
    /* Application is located only in SRAM0 controller region */
    if((startAddr >= 0x0) & (endAddr <= SRAM_CONTROLLER_SIZE))
    {   
        Sys_Init_SRAM_Controller(startAddr, endAddr, SRAM_0_CONTROLLER_ID);
    }
    
    /* Application is located only in SRAM1 controller region */
    if((startAddr >= SRAM_CONTROLLER_SIZE) & (endAddr < SRAM_TOTAL_SIZE))
    {   
        Sys_Init_SRAM_Controller(startAddr, endAddr, SRAM_1_CONTROLLER_ID);
    }
    
    /* If application starts at 0x343FFFFF or lower and ends at 0x34400000 or higher */
    if((startAddr <= SRAM_CONTROLLER_SIZE) & (endAddr > SRAM_CONTROLLER_SIZE))
    {
        /* Initialize controller 0 memory from the start address to maximum */
        Sys_Init_SRAM_Controller(startAddr, SRAM_CONTROLLER_SIZE - 1, SRAM_0_CONTROLLER_ID);
        Sys_Init_SRAM_Controller(SRAM_CONTROLLER_SIZE, endAddr, SRAM_1_CONTROLLER_ID);
    }
}
