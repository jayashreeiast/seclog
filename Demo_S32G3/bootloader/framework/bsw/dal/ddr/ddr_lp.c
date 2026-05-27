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

#include "ddr_lp.h"
#include "ddr_init.h"

static void load_csr(uintptr_t load_from);
static void load_ddrc_regs(uintptr_t load_from);

/* Store Configuration Status Registers. */
void store_csr(uintptr_t store_at)
{
	size_t i;
	uint16_t csr;
	uintptr_t current_addr = store_at;

	writel(UNLOCK_CSR_ACCESS, MICROCONT_MUX_SEL);
	writel(HCLKEN_MASK | UCCLKEN_MASK, DDR_PHYA_UCCLKHCLKENABLES);

	for (i = 0; i < csr_to_store_size; i++) {
		csr = readw((uint32_t)(DDRSS_BASE_ADDR + csr_to_store[i]));
		writew(csr, current_addr);
		current_addr += sizeof(uint16_t);
	}

	writel(HCLKEN_MASK, DDR_PHYA_UCCLKHCLKENABLES);
	writel(LOCK_CSR_ACCESS, MICROCONT_MUX_SEL);
}

/* Load Configuration Status Registers. */
static void load_csr(uintptr_t load_from)
{
	size_t i;
	uint16_t csr;
	uintptr_t current_addr = load_from;

	for (i = 0; i < csr_to_store_size; i++) {
		csr = readw(current_addr);
		current_addr += sizeof(uint16_t);
		writew(csr, (uint32_t)(DDRSS_BASE_ADDR + csr_to_store[i]));
	}
}

/* Store DDRC registers which have been updated post-training. */
void store_ddrc_regs(uintptr_t store_at)
{
	size_t i;
	uint32_t value;
	uintptr_t current_addr = store_at;
	/* DDRC registers are stored right after the CSRs */
	current_addr += sizeof(uint16_t) * csr_to_store_size;
	current_addr += sizeof(uint32_t) - (current_addr % sizeof(uint32_t));

	for (i = 0; i < ddrc_to_store_size; i++) {
		value = readl((uint32_t)(DDRC_BASE_ADDR + ddrc_to_store[i]));
		writel(value, current_addr);
		current_addr += sizeof(uint32_t);
	}
}

/* Load DDRC registers. */
static void load_ddrc_regs(uintptr_t load_from)
{
	size_t i;
	uint32_t value;
	uintptr_t current_addr = load_from;
	/* DDRC registers are stored right after the CSRs */
	current_addr += sizeof(uint16_t) * csr_to_store_size;
	current_addr += sizeof(uint32_t) - (current_addr % sizeof(uint32_t));

	for (i = 0; i < ddrc_to_store_size; i++) {
		value = readl(current_addr);
		writel(value, (uint32_t)(DDRC_BASE_ADDR + ddrc_to_store[i]));
		current_addr += sizeof(uint32_t);
	}
}

/* Transition the DDR SubSystem from normal mode to retention mode. */
void ddrss_to_io_retention_mode(void)
{
	uint32_t sbrctl, pwrctl, swctl, dfimisc, tmp32;

	/* Disable AXI Ports */
	writel(DISABLE_AXI_PORT, DDRC_UMCTL2_MP_BASE_ADDR +
	       OFFSET_DDRC_PCTRL_0);
	writel(DISABLE_AXI_PORT, DDRC_UMCTL2_MP_BASE_ADDR +
	       OFFSET_DDRC_PCTRL_1);
	writel(DISABLE_AXI_PORT, DDRC_UMCTL2_MP_BASE_ADDR +
	       OFFSET_DDRC_PCTRL_2);

	do {
		tmp32 = readl(DDRC_UMCTL2_MP_BASE_ADDR + OFFSET_DDRC_STAT);
	} while (tmp32 != STAT_RESET_VALUE);

	/* Disable Scrubber */
	sbrctl = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(sbrctl & (~SCRUB_EN_MASK), DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRSTAT);
	} while ((tmp32 & SCRUB_BUSY_MASK) != SBRSTAT_SCRUBBER_NOT_BUSY);

	/* Move to Self Refresh */
	pwrctl = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(pwrctl | SELFREF_SW_MASK, DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_STAT);
	} while ((tmp32 & OPERATING_MODE_MASK) != OPERATING_MODE_SELF_REFRESH);

	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_STAT);
	} while ((tmp32 & SELFREF_TYPE_MASK) != SELFREF_TYPE_NOT_AUTO_SR_CTRL);

	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_STAT);
	} while ((tmp32 & SELFREF_STATE_MASK) != SELFREF_STATE_SRPD);

	/* Transition Phy to LP3 */
	writel(DFIMISC_TRANSITION_PHY_TO_LP3, DDRC_BASE_ADDR +
	       OFFSET_DDRC_DFIMISC);
	swctl = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);
	writel(swctl & (~SW_DONE_MASK), DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);

	dfimisc = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel(dfimisc | DFI_FREQUENCY(DFIMISC_LP3_PHY_STATE),
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Set DFIMISC.dfi_init_start to 1. */
	dfimisc = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel(dfimisc | DFI_INIT_START_MASK,
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Wait DFISTAT.dfi_init_complete to be 1. */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFISTAT);
	} while ((tmp32 & DFI_INIT_COMPLETE_MASK) !=
		 DFISTAT_DFI_INIT_INCOMPLETE);

	dfimisc = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel(dfimisc | DFI_FREQUENCY(DFIMISC_LP3_PHY_STATE),
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Set DFIMISC.dfi_init_start to 0. */
	dfimisc = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel(dfimisc & (~DFI_INIT_START_MASK),
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Wait DFISTAT.dfi_init_complete to be 1. */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFISTAT);
	} while ((tmp32 & DFI_INIT_COMPLETE_MASK) ==
		 DFISTAT_DFI_INIT_INCOMPLETE);

	swctl = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);
	writel(swctl | SW_DONE_MASK, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);

	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SW_DONE_ACK_MASK) == SWSTAT_SW_NOT_DONE);

	/* Set PwrOkIn to 0 */
	tmp32 = readl(DDR_RET_CONTROL_REG);
	writel(tmp32 & (~DDR_RET_CONTROL_MASK), DDR_RET_CONTROL_REG);
	tmp32 = readl(DDR_CONFIG_0_REG);
	writel(tmp32 | DDR_CONFIG_0_MEM_RET, DDR_CONFIG_0_REG);
}

/* Transition the DDR SubSystem from retention mode to normal mode. */
uint32_t ddrss_to_normal_mode(uintptr_t csr_array)
{
	uint32_t pwrctl, init0, ret;

	ret = load_register_cfg(ddrc_cfg_size, ddrc_cfg);
	load_ddrc_regs(csr_array);
	if (ret != NO_ERR)
		return ret;

	writel(UNLOCK_CSR_ACCESS, MICROCONT_MUX_SEL);
	ret = load_dq_cfg(dq_swap_cfg_size, dq_swap_cfg);
	if (ret != NO_ERR)
		return ret;

	writel(LOCK_CSR_ACCESS, MICROCONT_MUX_SEL);

	init0 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_INIT0);
	init0 |= SKIP_DRAM_INIT_MASK;
	writel(init0, DDRC_BASE_ADDR + OFFSET_DDRC_INIT0);

	pwrctl = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(pwrctl | SELFREF_SW_MASK, DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/* Setup AXI ports parity */
	ret = set_axi_parity();
	if (ret != NO_ERR)
		return ret;

	writel(UNLOCK_CSR_ACCESS, MICROCONT_MUX_SEL);
	ret = load_register_cfg_16(phy_cfg_size, phy_cfg);
	if (ret != NO_ERR)
		return ret;

	/* Optimal PLL */
	set_optimal_pll();
	writel(LOCK_CSR_ACCESS, MICROCONT_MUX_SEL);

	/* Reload saved CSRs */
	writel(UNLOCK_CSR_ACCESS, MICROCONT_MUX_SEL);
	load_csr(csr_array);
	ret = load_register_cfg_16(pie_cfg_size, pie_cfg);
	if (ret != NO_ERR)
		return ret;

	writel(LOCK_CSR_ACCESS, MICROCONT_MUX_SEL);

	return post_train_setup(STORE_CSR_DISABLED | INIT_MEM_DISABLED |
				ADJUST_DDRC_DISABLED);
}
