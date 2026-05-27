/*
 * Copyright 2025 NXP.
 */

#ifndef ACCESS_MACROS_H
#define ACCESS_MACROS_H

#include "Compiler.h"

#define OSSTRING(x) #x
#define OSREG32( address )  ( *(volatile uint32_t*)(address) )
// #define ASM_KEYWORD(x)      __asm volatile(x)

/** coprocessor access macros */
/* write 32bit to coprocessor */
#define __MCR(coproc, opcode1, Rd, CRn, CRm, opcode2) \
            __asm volatile(" "OSSTRING(mcr p##coproc)", "OSSTRING(opcode1)", %0, "OSSTRING(c##CRn)", "OSSTRING(c##CRm)", "OSSTRING(opcode2)" ": : "r"(Rd))
/* read 32bit from coprocessor */
#define __MRC(coproc, opcode1, CRn, CRm, opcode2) ({ \
            register uint32_t _tempRegRt; \
            __asm volatile(" "OSSTRING(mrc p##coproc)", "OSSTRING(opcode1)", %0, "OSSTRING(c##CRn)", "OSSTRING(c##CRm)", "OSSTRING(opcode2)" ": "=r"(_tempRegRt));   \
            _tempRegRt; })
/* write 64bit to coprocessor */
#define __MCRR(coproc, opcode1, Rd, Rd2, CRm) ({ \
            __asm volatile(" "OSSTRING(mcrr p##coproc)", "OSSTRING(opcode1)", %0, %1, "OSSTRING(c##CRm)" ": : "r"(Rd), "r"(Rd2)); })
/* read 64bit from coprocessor */
#define __MRRC(coproc, opcode1, CRm) ({ \
            uint32_t Rd, Rd2;                           \
            uint64_t retval;                            \
            __asm volatile(" "OSSTRING(mrrc p##coproc)", "OSSTRING(opcode1)", %0, %1, "OSSTRING(c##CRm)" ": "=r"(Rd), "=r"(Rd2): ); \
            retval = ((uint64_t)Rd2);                   \
            retval = (retval << 32);                    \
            retval |= Rd;                               \
            retval;})


#endif /* ACCESS_MACROS_H */
