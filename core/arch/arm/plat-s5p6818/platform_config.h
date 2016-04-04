/*
 * Copyright (C) 2016  Nexell Co., Ltd.
 * Author: Bon-gyu, KOO <freestyle@nexell.co.kr>
 *
 * All rights reserved.
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

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/* Make stacks aligned to data cache line length */
#define STACK_ALIGNMENT		64

#ifdef ARM64
#ifdef CFG_WITH_PAGER
#error "Pager not supported for ARM64"
#endif
#endif /* ARM64 */

#define GIC_BASE				0xC0009000
#define GICD_OFFSET				0x0
#define GICC_OFFSET				0x1000

#define UART0_BASE				0xC00A1000
#define UART1_BASE				0xC00A0000
#define UART2_BASE				0xC00A2000
#define UART3_BASE				0xC00A3000
#define UART4_BASE				0xC006D000
#define UART5_BASE				0xC006F000

/* S5P6818 UART */
#ifdef PLAT_UART_BASE
#define CONSOLE_UART_BASE		(PLAT_UART_BASE)
#else
#define CONSOLE_UART_BASE		(UART3_BASE)
#endif
#define CONSOLE_BAUDRATE		115200
#define CONSOLE_UART_CLK_IN_HZ	19200000


#define HEAP_SIZE		(24 * 1024)

/*
 * nexell memory map
 *
 * We use only non-secure DRAM (TZDRAM and TZSRAM are emulated).
 */

#define DRAM0_BASE		0x40000000
#define DRAM0_SIZE		0x7EE00000

#ifdef CFG_WITH_PAGER

#define TZSRAM_BASE		0xBEE00000
#define TZSRAM_SIZE		(200 * 1024)

#define TZDRAM_BASE		0xBEF00000
#define TZDRAM_SIZE		(15 * 1024 * 1024)

#else /* CFG_WITH_PAGER */

#define TZDRAM_BASE		0xBEE00000
#define TZDRAM_SIZE		(16 * 1024 * 1024)

#endif /* CFG_WITH_PAGER */

#define CFG_SHMEM_START		0xBED00000
#define CFG_SHMEM_SIZE		(1024 * 1024)

#define CFG_TEE_CORE_NB_CORE	8

#define CFG_TEE_RAM_VA_SIZE	(1024 * 1024)

#define CFG_TEE_LOAD_ADDR	0xBEE00000

#ifdef CFG_WITH_PAGER

#define CFG_TEE_RAM_START	TZSRAM_BASE
#define CFG_TEE_RAM_PH_SIZE	TZSRAM_SIZE
#define CFG_TA_RAM_START	ROUNDUP(TZDRAM_BASE, CORE_MMU_DEVICE_SIZE)
#define CFG_TA_RAM_SIZE		ROUNDDOWN(TZDRAM_SIZE, CORE_MMU_DEVICE_SIZE)

#else /* CFG_WITH_PAGER */

#define CFG_TEE_RAM_PH_SIZE	CFG_TEE_RAM_VA_SIZE
#define CFG_TEE_RAM_START	TZDRAM_BASE
#define CFG_TA_RAM_START	ROUNDUP((TZDRAM_BASE + CFG_TEE_RAM_VA_SIZE), \
					CORE_MMU_DEVICE_SIZE)

#define CFG_TA_RAM_SIZE		ROUNDDOWN((TZDRAM_SIZE - CFG_TEE_RAM_VA_SIZE),\
					CORE_MMU_DEVICE_SIZE)

#endif /* CFG_WITH_PAGER */

#define DEVICE0_BASE		ROUNDDOWN(CONSOLE_UART_BASE, \
					CORE_MMU_DEVICE_SIZE)
#define DEVICE0_SIZE		CORE_MMU_DEVICE_SIZE
#define DEVICE0_TYPE		MEM_AREA_IO_NSEC

#endif /* PLATFORM_CONFIG_H */
