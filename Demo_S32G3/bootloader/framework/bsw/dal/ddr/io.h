/*
 * Copyright 2023 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IO_H_
#define IO_H_
/**
 * Read/Write operations
 */
#include <stdio.h>
#include <stdint.h>

#define dmb()                __asm __volatile__ ("" : : : "memory")

#define arch_putl(v, a)         (*(volatile uint32_t *)((size_t)(a)) = (v))
#define arch_getl(a)            (*(volatile uint32_t *)((size_t)(a)))

/* 16 bits*/
#define arch_putw(v, a)  (*(volatile uint16_t *)((size_t)(a)) = (v))
#define arch_getw(a)     (*(volatile uint16_t *)((size_t)(a)))

/* 8 bits */
#define arch_getb(a)     (*(volatile uint8_t *)((size_t)(a)))

#ifndef __ghs__
#define writel(v, c) \
__extension__({ uint32_t __v = (v); dmb(); arch_putl(__v, (c)); __v; })
#define readl(c) __extension__({ uint32_t __v = arch_getl(c); dmb(); __v; })

/* 16 bits*/
#define writew(v, c) \
__extension__({ uint16_t __v = (v); dmb(); arch_putw(__v, (c)); __v; })
#define readw(c) \
__extension__({ uint16_t __v = arch_getw(c); dmb(); __v; })

/* 8 bits*/
#define readb(c) \
__extension__({ uint8_t __v = arch_getb(c); dmb(); __v; })

#else
#define writel(v, c) arch_putl(v, c)
#define readl(c) arch_getl(c)

/* 16 bits*/
#define writew(v, c) arch_putw(v, c)
#define readw(c) arch_getw(c)

/* 8 bits*/
#define readb(c) arch_getb(c)
#endif

#endif /* IO_H_ */
