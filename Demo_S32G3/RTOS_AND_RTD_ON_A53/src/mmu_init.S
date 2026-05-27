/* Copyright 2021-2025 NXP */
/* License: BSD 3-clause
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __ghs__
  .arch	armv8-a
#endif

.section .mmu_page
  .align	12

ttb0: .space	4096, 0

ttb1: .space	4096, 0

ttb2: .space	4096, 0

ttb3: .space	4096, 0

ttb4: .space	4096, 0

ttb5: .space	4096, 0

ttb6: .space  4096, 0

  .text
  .align 2
  .globl MMU_Init
#ifndef __ghs__
  .type MMU_Init, %function
#else
  .type MMU_Init, $function
#endif
MMU_Init:

  /*
   * attr0 : 0xFF, normal cachealbe
   * attr1 : 0x44, normal non-cacheable
   * attr2 : 0x04, device nGnRE
   * attr3 : 0x00, device nGnRnE
   */
  ldr	x0, =0x0444FF
  msr    mair_el1, x0
  /* Invalidate TLBs at the current exception level */

  /* RES1
     RES1
     PS     40-bit
     ORGN0  outer WB no WA cacheable
     IRGN0  inner WB no WA cacheable
     T0SZ   39-bit region size */
  ldr    x0, =(1 << 31) | (1 << 23) | (2 << 16) | (3 << 10) | (3 << 8) | 25
  msr    tcr_el1, x0

   /* Page table setup
   1GB@0: 405 normal non-cacheable
   1GB@1GB: 40D device nGnRnE
   1GB@2GB: 401 normal cacheable
   */
  adrp   x0, ttb0
  add    x0, x0, :lo12:ttb0
  mov    x1, #0x405
  ldr    x2, = (0x40D | (3 << 53))
  mov    x3, #0x407
  ldr    x4, = (0x40F | (3 << 53))

  str    x1, [x0]                    /* 0x0~0x40000000  normal non-cacheable*/

  ldr    x5, = ttb1
  orr    x5, x5, #0x3
  str    x5, [x0, #(0x40000000 >> 30 << 3)]

  ldr    x5, = ttb1
  mov    x6, #0x40000000
  mov    x7, #0
  mov    x8, #0

  1: orr  x8, x6, x2
  str    x8, [x5, x7, lsl #3]
  add    x6, x6, #0x200000
  add    x7, x7, #1
  cmp    x7, #(0x3000000/0x200000)
  blt    1b

  ldr    x9, = ttb2
  orr    x9, x9, #0x3
  str    x9, [x5, #(0x3000000 >> 21 << 3)]
  ldr    x9, = ttb2
  mov    x10, #0x43000000
  mov    x7, #0
  mov    x8, #0

  1: orr x8, x10, x4
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x100000/0x1000)
  blt    1b

  mov    x8, #0

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x200000/0x1000)
  blt    1b

  ldr   x9, = ttb3
  orr    x9, x9, #0x3
  str    x9, [x5, #(0x3200000 >> 21 << 3)]
  ldr    x9, = ttb3
  mov    x10, #0x43200000
  mov    x7, #0
  mov    x8, #0

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x140000/0x1000)
  blt    1b
  mov    x8, #0
  
  1: orr x8, x10, x4
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x200000/0x1000)
  blt    1b

  mov    x6, #0x43400000
  mov    x7, #(0x3400000/0x200000)
  mov    x8, #0

  1: orr  x8, x6, x2
  str    x8, [x5, x7, lsl #3]
  add    x6, x6, #0x200000
  add    x7, x7, #1
  cmp    x7, #(0x3800000/0x200000)
  blt    1b

  ldr   x9, = ttb4
  orr    x9, x9, #0x3
  str    x9, [x5, #(0x3800000 >> 21 << 3)]
  ldr    x9, = ttb4
  mov    x10, #0x43800000
  mov    x7, #0
  mov    x8, #0

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x50000/0x1000)
  blt    1b
  mov    x8, #0

  1: orr x8, x10, x4
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x200000/0x1000)
  blt    1b
  
  mov    x6, #0x43A00000
  mov    x7, #(0x3A00000/0x200000)
  mov    x8, #0
  1: orr  x8, x6, x2
  str    x8, [x5, x7, lsl #3]
  add    x6, x6, #0x200000
  add    x7, x7, #1
  cmp    x7, #(0x40000000/0x200000)
  blt    1b
  
  orr    x1, x1, #(2 << 30)
  str    x1, [x0, #0x10]
  /*
  ldr    x5, = ttb5
  orr    x5, x5, #0x3
  str    x5, [x0, #0x10]

  ldr    x5, = ttb5
  mov    x6, #0x80000000
  mov    x7, #0
  mov    x8, #0

  1: orr  x8, x6, x1
  str    x8, [x5, x7, lsl #3]
  add    x6, x6, #0x200000
  add    x7, x7, #1
  cmp    x7, #(0x40000000/0x200000)
  blt    1b

  ldr    x5, = ttb5
  ldr    x9, = ttb6
  orr    x9, x9, #0x3
  str    x9, [x5]

  ldr    x9, = ttb6
  mov    x10, #0x80000000
  mov    x7, #0
  mov    x8, #0
  mov    x3, #0x403

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x75000/0x1000)
  blt    1b

  mov    x3, #0x407

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0xCB000/0x1000)
  blt    1b

  mov    x3, #0x403

  1: orr x8, x10, x3
  str    x8, [x9, x7, lsl #3]
  add    x10, x10, #0x1000
  add    x7, x7, #1
  cmp    x7, #(0x200000/0x1000)
  blt    1b
  */

  msr    ttbr0_el1, x0             /* set TTBR */

  /* Ensure all translation table writes have drained into memory,
     the TLB invalidation is complete,
     and translation register writes are committed before enabling the MMU */
  dsb    ish
  isb

  mrs    x0, sctlr_el1
  ldr    x4, =(1 << 1)
  bic    x0, x0, x4         /* clear A, disable alignment fault check */
  /* I enable instruction cache
     C enable data and unified caches
     M enable EL3 MMU */
  ldr    x1, =(1 << 12) | (1 << 2) | 1
  orr    x0, x0, x1                /* set bits */
  msr    sctlr_el1, x0
  /* Ensure the MMU enable takes effect immediately */
  isb

  ret

#ifndef __ghs__
  .pool
#endif
  .size MMU_Init, . - MMU_Init
