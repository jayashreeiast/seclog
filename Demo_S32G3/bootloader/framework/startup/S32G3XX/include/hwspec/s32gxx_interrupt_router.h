#ifndef _S32GXX_INTERRUPT_ROUTER_H_
#define _S32GXX_INTERRUPT_ROUTER_H_
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
*  Description:  S32GXX interrupt router
*
**************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <Os_platform_base.h>

/* Interrupt Router base address */
#define IR_BASE_ADDR OSMSCM_BASE /**< MSCM  base address */ 

/* Interrupt Router registers */
#define IR_CP0IR   SYS_REG32( IR_BASE_ADDR + 0x800 ) /**< Interrupt Router CP0 Interrupt */
#define IR_CP1IR   SYS_REG32( IR_BASE_ADDR + 0x804 ) /**< Interrupt Router CP1 Interrupt */
#define IR_CP2IR   SYS_REG32( IR_BASE_ADDR + 0x808 ) /**< Interrupt Router CP2 Interrupt */

#define IR_CPGIR   SYS_REG32( IR_BASE_ADDR + 0x820 ) /**< Interrupt Router CPU Generate Interrupt */
#define IR_SPRC(x) SYS_REG16( IR_BASE_ADDR + 0x880 + ((x)<<1) ) /**< Interrupt Router Shared Peripheral Routing Control */
    
    
/* Interrupt Router user interface */
#define IR_CP0_CLEAR_SW_INT(x)  IR_CP0IR=(1<<(x))  /**< clear sw interrupt flag for CPU0 */
#define IR_CP1_CLEAR_SW_INT(x)  IR_CP1IR=(1<<(x))  /**< clear sw interrupt flag for CPU1 */
#define IR_CP2_CLEAR_SW_INT(x)  IR_CP2IR=(1<<(x))  /**< clear sw interrupt flag for CPU2 */

#define IR_GEN_SW_INT_2_CP0(x)  IR_CPGIR=((1<<16)|(x))  /**< generate sw interrupt for CPU0, x={0,1,2,3} */
#define IR_GEN_SW_INT_2_CP1(x)  IR_CPGIR=((1<<17)|(x))  /**< generate sw interrupt for CPU1, x={0,1,2,3} */
#define IR_GEN_SW_INT_2_CP2(x)  IR_CPGIR=((1<<18)|(x))  /**< generate sw interrupt for CPU1, x={0,1,2,3} */

#define IR_ROUTE_INT(id,cpu)    IR_SPRC(id)=(cpu)       /**< route interrupt to cpu, id=0->111, cpu={0,1} */


#define IR_ROUTE_2_CPU0     (1UL)
#define IR_ROUTE_2_CPU1     (2UL)
#define IR_ROUTE_2_CPU2     (4UL)

#define IR_ROUTE_NO_INT     (248UL)

/* route all interrupts to selected cpu */
#define IR_ROUTE_ALL_2_CPU(cpu)                     \
            {                                       \
                int x;                              \
                for (x=0; x<IR_ROUTE_NO_INT; x++) { \
                        IR_ROUTE_INT(x, (cpu));     \
                    }                               \
            }                                       \


#ifdef __cplusplus
}
#endif

#endif /* _S32GXX_INTERRUPT_ROUTER_H_ */

