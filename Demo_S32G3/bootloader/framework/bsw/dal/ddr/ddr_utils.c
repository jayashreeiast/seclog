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

#include "ddr_utils.h"

#ifdef STORE_CSR_ENABLE
#include "ddr_lp.h"
#endif

static uint32_t enable_axi_ports(void);
static uint32_t get_mail(uint32_t *mail);
static uint32_t ack_mail(void);
static uint32_t init_memory_ecc_scrubber(void);
static void sel_clk_src(uint32_t clk_src, bool *already_set);
static uint8_t get_max_cdd(const uint32_t cdd_addr[], size_t size);
static uint16_t get_max_delay(const uint32_t delay_addr[], size_t size);
static uint8_t get_avg_vref(const uint32_t vref_addr[], size_t size);
static uint32_t adjust_ddrc_config(void);
static bool is_lpddr4(void);

static struct space_timing_params tr_res = {
	.cdd = {.rr = 0, .rw = 0, .wr = 0, .ww = 0},
	.vref_ca = 0,
	.vref_dq = 0,
	.tphy_wrdata_delay = 0
};

/* Modify bitfield value with delta, given bitfield position and mask */
bool update_bf(uint32_t *v, uint8_t pos, uint32_t mask, int32_t delta)
{
	bool ret = false;
	uint32_t bf_val = (*v >> pos) & mask;
	int64_t new_val = (int64_t)(bf_val) + delta;

	if (mask >= (uint32_t)(new_val)) {
		*v = (*v & ~(mask << pos)) | ((uint32_t)(new_val) << pos);
		ret = true;
	}

	return ret;
}

/*
 * Set the ddr clock source, FIRC or DDR_PLL_PHI0.
 * @param clk_src - requested clock source
 * @param already_set - whether clock source is already set to clk_src or not
 */
static void sel_clk_src(uint32_t clk_src, bool *already_set)
{
	uint32_t tmp32;

	/* Check if the clock source is already set to clk_src*/
	tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSS);
	if (((tmp32 & CSS_SELSTAT_MASK) >> CSS_SELSTAT_POS) == clk_src) {
		*already_set = true;
	} else {
		*already_set = false;

		/* To wait till clock switching is completed */
		do {
			tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSS);
		} while (((tmp32 >> CSS_SWIP_POS) & CSS_SW_IN_PROGRESS) !=
			 CSS_SW_COMPLETED);

		/* Set DDR_CLK source on src_clk */
		tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSC);
		writel((CSC_SELCTL_MASK & tmp32) | (clk_src << CSC_SELCTL_POS),
		       MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSC);

		/* Request clock switch */
		tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSC);
		writel((CSC_CLK_SWITCH_REQUEST << CSC_CLK_SWITCH_POS) |
		       tmp32, MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSC);

		/* To wait till clock switching is completed */
		do {
			tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSS);
		} while (((tmp32 >> CSS_SWIP_POS) & CSS_SW_IN_PROGRESS) !=
			 CSS_SW_COMPLETED);

		/* To wait till Switch after request is succeeded */
		do {
			tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSS);
		} while (((tmp32 >> CSS_SW_TRIGGER_CAUSE_POS) &
			  CSS_SW_AFTER_REQUEST_SUCCEDED) !=
			 CSS_SW_AFTER_REQUEST_SUCCEDED);

		/* Make sure correct clock source is selected */
		do {
			tmp32 = readl(MC_CGM5_BASE_ADDR + OFFSET_MUX_0_CSS);
		} while (((tmp32 & CSS_SELSTAT_MASK) >> CSS_SELSTAT_POS) !=
			 clk_src);
	}
}

/* Deassert DDR controller and AXI ports reset signal */
#pragma weak deassert_ddr_reset
uint32_t deassert_ddr_reset(void)
{
	uint32_t prst, pstat, timeout = DEFAULT_TIMEOUT;
	bool already_set;

	/* Check that reset signal was previously asserted */
	prst = readl(0x40078040);
	if (((prst >> PRST_0_PERIPH_3_RST_POS) & FORCED_RESET_ON_PERIPH) !=
	    FORCED_RESET_ON_PERIPH)
		return DEASSERT_FAILED;

	/*
	 * Set ddr clock source on FIRC_CLK.
	 * If it's already set on FIRC_CLK, already_set becomes true.
	 */
	sel_clk_src(FIRC_CLK_SRC, &already_set);

	/* Deassert Reset To Controller and AXI Ports reset signal */
	writel(~(FORCED_RESET_ON_PERIPH << PRST_0_PERIPH_3_RST_POS) &
	       prst, 0x40078040);

	/* Verify reset status */
	do {
		pstat = readl(MG_RGM_PSTAT_0);
	} while ((--timeout != 0u) && (((pstat >> PRST_0_PERIPH_3_RST_POS)
		 & FORCED_RESET_ON_PERIPH) == FORCED_RESET_ON_PERIPH));

	if (timeout == 0u)
		return DEASSERT_FAILED;

	/* Check if the initial clock source was not already set FIRC */
	if (!already_set)
		sel_clk_src(DDR_PHI0_PLL, &already_set);

	return NO_ERR;
}

/* Sets default AXI parity. */
uint32_t set_axi_parity(void)
{
	uint32_t tmp32;

	/* Enable Parity For All AXI Interfaces */
	tmp32 = readl(DDR_SS_REG);
	writel(tmp32 | DDR_SS_AXI_PARITY_ENABLE_MASK, DDR_SS_REG);

	/* Set AXI_PARITY_TYPE to 0x1ff;   0-even, 1-odd */
	tmp32 = readl(DDR_SS_REG);
	writel(tmp32 | DDR_SS_AXI_PARITY_TYPE_MASK, DDR_SS_REG);

	/* For LPDDR4 Set DFI1_ENABLED to 0x1 */
	if (is_lpddr4()) {
		tmp32 = readl(DDR_SS_REG);
		writel(tmp32 | DDR_SS_DFI_1_ENABLED, DDR_SS_REG);
	}

	if (deassert_ddr_reset() != NO_ERR)
		return DEASSERT_FAILED;

	/* Enable HIF, CAM Queueing */
	writel(DBG1_DISABLE_DE_QUEUEING, DDRC_BASE_ADDR + OFFSET_DDRC_DBG1);

	/* Disable auto-refresh: RFSHCTL3.dis_auto_refresh = 1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_RFSHCTL3);
	writel((RFSHCTL3_DISABLE_AUTO_REFRESH | tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_RFSHCTL3);

	/* Disable power down: PWRCTL.powerdown_en = 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(((~PWRCTL_POWER_DOWN_ENABLE_MASK) & tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/* Disable self-refresh: PWRCTL.selfref_en = 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(((~PWRCTL_SELF_REFRESH_ENABLE_MASK) & tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/*
	 * Disable assertion of dfi_dram_clk_disable:
	 * PWRTL.en_dfi_dram_clk_disable = 0
	 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(((~PWRCTL_EN_DFI_DRAM_CLOCK_DIS_MASK) & tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/* Enable Quasi-Dynamic Programming */
	writel(SWCTL_SWDONE_ENABLE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);

	/* Confirm Register Programming Done Ack is Cleared */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) == SWSTAT_SW_DONE);

	/* DFI_INIT_COMPLETE_EN set to 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel((~DFIMISC_DFI_INIT_COMPLETE_EN_MASK) & tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Set SWCTL.sw_done to 1 */
	writel(SWCTL_SWDONE_DONE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);

	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) == SWSTAT_SW_NOT_DONE);

	return NO_ERR;
}

/* Enables AXI port n. Programming Mode: Dynamic */
static uint32_t enable_axi_ports(void)
{
	/* Port 0 Control Register */
	writel(ENABLE_AXI_PORT,
	       DDRC_UMCTL2_MP_BASE_ADDR + OFFSET_DDRC_PCTRL_0);
	/* Port 1 Control Register */
	writel(ENABLE_AXI_PORT,
	       DDRC_UMCTL2_MP_BASE_ADDR + OFFSET_DDRC_PCTRL_1);
	/* Port 2 Control Register */
	writel(ENABLE_AXI_PORT,
	       DDRC_UMCTL2_MP_BASE_ADDR + OFFSET_DDRC_PCTRL_2);

	return NO_ERR;
}

/*
 * Post PHY training setup - complementary settings that need to be
 * performed after running the firmware.
 * @param options - various flags controlling post training actions
 * (whether to init memory with ECC scrubber / whether to store CSR)
 */
uint32_t post_train_setup(uint8_t options)
{
	uint32_t ret = NO_ERR;
	uint32_t tmp32;

	/*
	 * CalBusy.0 = 1, indicates the calibrator is actively calibrating.
	 * Wait Calibrating done.
	 */
	do {
		tmp32 = readl(DDR_PHYA_MASTER0_CALBUSY);
	} while ((tmp32 & MASTER0_CAL_ACTIVE) != MASTER0_CAL_DONE);

#ifdef STORE_CSR_ENABLE
	if ((options & STORE_CSR_MASK) != STORE_CSR_DISABLED)
		store_csr(RETENTION_ADDR);
#endif

	/* Set SWCTL.sw_done to 0 */
	writel(SWCTL_SWDONE_ENABLE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) != SWSTAT_SW_NOT_DONE);

	/* Set DFIMISC.dfi_init_start to 1*/
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel((DFIMISC_DFI_INIT_START_MASK | tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Set SWCTL.sw_done to 1 */
	writel(SWCTL_SWDONE_DONE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);

	/* Wait SWSTAT.sw_done_ack to 1*/
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) == SWSTAT_SW_NOT_DONE);

	/* Wait DFISTAT.dfi_init_complete to 1 */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFISTAT);
	} while ((tmp32 & DFISTAT_DFI_INIT_DONE) ==
		 DFISTAT_DFI_INIT_INCOMPLETE);

	/* Set SWCTL.sw_done to 0 */
	writel(SWCTL_SWDONE_ENABLE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) != SWSTAT_SW_NOT_DONE);

	/* Set dfi_init_start to 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel((~DFIMISC_DFI_INIT_START_MASK) & tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	if ((options & ADJUST_DDRC_MASK) != ADJUST_DDRC_DISABLED) {
		/* Overwrite DDRC register based on post training_results */
		ret = adjust_ddrc_config();
		if (ret != NO_ERR)
			return ret;
	}

	/* Set dfi_complete_en to 1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);
	writel(DFIMISC_DFI_INIT_COMPLETE_EN_MASK | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_DFIMISC);

	/* Set PWRCTL.selfref_sw to 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(((~PWRCTL_SELFREF_SW_MASK) & tmp32),
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/* Set SWCTL.sw_done to 1 */
	writel(SWCTL_SWDONE_DONE, DDRC_BASE_ADDR + OFFSET_DDRC_SWCTL);
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SWSTAT);
	} while ((tmp32 & SWSTAT_SWDONE_ACK_MASK) == SWSTAT_SW_NOT_DONE);

	/* Wait for DWC_ddr_umctl2 to move to normal operating mode */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_STAT);
	} while ((tmp32 & STAT_OPERATING_MODE_MASK) ==
		 STAT_OPERATING_MODE_INIT);

	/* Enable auto-refresh: RFSHCTL3.dis_auto_refresh = 0 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_RFSHCTL3);
	writel((~RFSHCTL3_DIS_AUTO_REFRESH_MASK) & tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_RFSHCTL3);

	/*
	 * If ECC feature is enabled (ECCCFG0[ecc_mode] > 0)
	 * initialize memory with the ecc scrubber
	 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG0);
	if (((tmp32 & ECCCFG0_ECC_MODE_MASK) > ECCCFG0_ECC_DISABLED) &&
	    ((options & INIT_MEM_MASK) != INIT_MEM_DISABLED)) {
		ret = init_memory_ecc_scrubber();
		if (ret != NO_ERR)
			return ret;
	}

#ifdef STORE_CSR_ENABLE
	if ((options & STORE_CSR_MASK) != STORE_CSR_DISABLED)
		store_ddrc_regs(RETENTION_ADDR);
#endif

	/* Enable power down: PWRCTL.powerdown_en = 1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(PWRCTL_POWER_DOWN_ENABLE_MASK | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/* Enable self-refresh: PWRCTL.selfref_en = 1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(PWRCTL_SELF_REFRESH_ENABLE_MASK | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/*
	 * Enable assertion of dfi_dram_clk_disable:
	 * PWRTL.en_dfi_dram_clk_disable = 1
	 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);
	writel(PWRCTL_EN_DFI_DRAM_CLOCK_DIS_MASK | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_PWRCTL);

	/*
	 * Each platform has a different number of AXI ports so this
	 * method should be implemented in hardware specific source
	 */
	ret = enable_axi_ports();

	return ret;
}

/* Wait until firmware finishes execution and return training result */
uint32_t wait_firmware_execution(void)
{
	uint32_t mail = 0;
	uint32_t ret = NO_ERR;

	while ((mail != TRAINING_OK_MSG) && (ret == NO_ERR)) {
		ret = get_mail(&mail);

		/* 0x07 means OK, 0xFF means failure */
		if (mail == TRAINING_FAILED_MSG) {
			/* Training stage failed */
			ret = TRAINING_FAILED;
		}
	}

	return ret;
}

/* Acknowledge received message */
static uint32_t ack_mail(void)
{
	uint32_t timeout = DEFAULT_TIMEOUT;
	/* ACK message */
	writel(APBONLY_DCTWRITEPROT_ACK_EN, DDR_PHYA_DCTWRITEPROT);
	uint32_t tmp32 = readl(DDR_PHYA_APBONLY_UCTSHADOWREGS);

	/* Wait firmware to respond to ACK (UctWriteProtShadow to be set) */
	while ((--timeout != 0u) && ((tmp32 & UCT_WRITE_PROT_SHADOW_MASK) ==
				     UCT_WRITE_PROT_SHADOW_ACK))
		tmp32 = readl(DDR_PHYA_APBONLY_UCTSHADOWREGS);

	if (timeout == 0u)
		return TIMEOUT_ERR;

	writel(APBONLY_DCTWRITEPROT_ACK_DIS, DDR_PHYA_DCTWRITEPROT);

	return NO_ERR;
}

/* Read available message from DDR PHY microcontroller */
static uint32_t get_mail(uint32_t *mail)
{
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t tmp32 = readl(DDR_PHYA_APBONLY_UCTSHADOWREGS);

	while ((--timeout != 0u) && ((tmp32 & UCT_WRITE_PROT_SHADOW_MASK) !=
				     UCT_WRITE_PROT_SHADOW_ACK))
		tmp32 = readl(DDR_PHYA_APBONLY_UCTSHADOWREGS);

	if (timeout == 0u)
		return TIMEOUT_ERR;

	*mail = readl(DDR_PHYA_APBONLY_UCTWRITEONLYSHADOW);
	/* ACK */
	return ack_mail();
}

/* Initialize memory with the ecc scrubber */
static uint32_t init_memory_ecc_scrubber(void)
{
	uint8_t region_lock;
	uint32_t tmp32, pattern = 0x00000000U;

	/* Save previous ecc region parity locked state. */
	region_lock = (uint8_t)(readl(DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG1) &
				(ECCCFG1_REGION_PARITY_LOCKED <<
				 ECCCFG1_REGION_PARITY_LOCK_POS));

	/* Enable ecc region lock. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG1);
	writel((ECCCFG1_REGION_PARITY_LOCKED <<
		ECCCFG1_REGION_PARITY_LOCK_POS) | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG1);

	/* Set SBRCTL.scrub_mode = 1. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel((SBRCTL_SCRUB_MODE_WRITE << SBRCTL_SCRUB_MODE_POS) |
	       tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Set SBRCTL.scrub_during_lowpower = 1. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel((SBRCTL_SCRUB_DURING_LOWPOWER_CONTINUED <<
		SBRCTL_SCRUB_DURING_LOWPOWER_POS) | tmp32,
	       DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Set SBRCTL.scrub_interval = 0. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(~(SBRCTL_SCRUB_INTERVAL_FIELD << SBRCTL_SCRUB_INTERVAL_POS) &
	       tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Set the desired pattern through SBRWDATA0 register. */
	writel(pattern, DDRC_BASE_ADDR + OFFSET_DDRC_SBRWDATA0);

	/* Enable the SBR by programming SBRCTL.scrub_en = 1. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(SBRCTL_SCRUB_EN | tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/*
	 * Poll until SBRSTAT.scrub_done = 1
	 * (all scrub writes commands have been sent).
	 */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRSTAT);
	} while ((tmp32 & SBRSTAT_SCRUB_DONE_MASK) ==
		 SBRSTAT_SCRUBBER_NOT_DONE);

	/*
	 * Poll until SBRSTAT.scrub_busy = 0
	 * (all scrub writes data have been sent).
	 */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRSTAT);
	} while ((tmp32 & SBRSTAT_SCRUBBER_BUSY_MASK) !=
		 SBRSTAT_SCRUBBER_NOT_BUSY);

	/* Disable SBR by programming SBRCTL.scrub_en = 0. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(~SBRCTL_SCRUB_EN & tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Enter normal scrub operation (Reads): SBRCTL.scrub_mode = 0. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(~(SBRCTL_SCRUB_MODE_WRITE << SBRCTL_SCRUB_MODE_POS) &
	       tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Set SBRCTL.scrub_interval = 1. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	tmp32 = ~(SBRCTL_SCRUB_INTERVAL_FIELD <<
		  SBRCTL_SCRUB_INTERVAL_POS) & tmp32;
	writel((SBRCTL_SCRUB_INTERVAL_VALUE_1 << SBRCTL_SCRUB_INTERVAL_POS) |
	       tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Enable the SBR by programming SBRCTL.scrub_en = 1. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);
	writel(SBRCTL_SCRUB_EN | tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_SBRCTL);

	/* Restore locked state of ecc region. */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG1);
	tmp32 = (tmp32 & ~(ECCCFG1_REGION_PARITY_LOCKED <<
			   ECCCFG1_REGION_PARITY_LOCK_POS)) |
		(region_lock << ECCCFG1_REGION_PARITY_LOCK_POS);
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_ECCCFG1);

	return NO_ERR;
}

/* Read lpddr4 mode register with given rank and index */
uint32_t read_lpddr4_mr(uint8_t mr_rank, uint8_t mr_index)
{
	uint32_t tmp32;
	uint8_t succesive_reads = 0;

	/* Set MRR_DDR_SEL_REG to 0x1 to enable LPDDR4 mode */
	tmp32 = readl(PERF_BASE_ADDR + OFFSET_MRR_0_DATA_REG_ADDR);
	writel((tmp32 | MRR_0_DDR_SEL_REG_MASK),
	       PERF_BASE_ADDR + OFFSET_MRR_0_DATA_REG_ADDR);

	/*
	 * Ensure no MR transaction is in progress:
	 * mr_wr_busy signal must be low
	 */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRSTAT);
		if ((tmp32 & MRSTAT_MR_BUSY) == MRSTAT_MR_NOT_BUSY)
			succesive_reads++;
		else
			succesive_reads = 0;
	} while (succesive_reads != REQUIRED_MRSTAT_READS);

	/* Set MR_TYPE = 0x1 (Read) and MR_RANK = desired rank */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);
	tmp32 |= MRCTRL0_MR_TYPE_READ;
	tmp32 = (tmp32 & ~(MRCTRL0_RANK_ACCESS_FIELD <<
			   MRCTRL0_RANK_ACCESS_POS)) |
		(mr_rank << MRCTRL0_RANK_ACCESS_POS);
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);

	/* Configure MR address: MRCTRL1[8:15] */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL1);
	tmp32 = (tmp32 & ~(MRCTRL1_MR_ADDRESS_FIELD <<
			   MRCTRL1_MR_ADDRESS_POS)) |
		((uint16_t)mr_index << MRCTRL1_MR_ADDRESS_POS);
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL1);

	dsb();

	/* Initiate MR transaction: MR_WR = 0x1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);
	writel(tmp32 | (MRCTRL0_WR_ENGAGE << MRCTRL0_WR_ENGAGE_POS),
	       DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);

	/* Wait until MR transaction completed */
	succesive_reads = 0;
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRSTAT);
		if ((tmp32 & MRSTAT_MR_BUSY) == MRSTAT_MR_NOT_BUSY)
			succesive_reads++;
		else
			succesive_reads = 0;
	} while (succesive_reads != REQUIRED_MRSTAT_READS);

	return readl(PERF_BASE_ADDR + OFFSET_MRR_1_DATA_REG_ADDR);
}

/* Write data in lpddr4 mode register with given index */
uint32_t write_lpddr4_mr(uint8_t mr_rank, uint8_t mr_index, uint8_t mr_data)
{
	uint32_t tmp32;
	uint8_t succesive_reads = 0;

	/* Set MRR_DDR_SEL_REG to 0x1 to enable LPDDR4 mode */
	tmp32 = readl(PERF_BASE_ADDR + OFFSET_MRR_0_DATA_REG_ADDR);
	writel(tmp32 | MRR_0_DDR_SEL_REG_MASK,
	       PERF_BASE_ADDR + OFFSET_MRR_0_DATA_REG_ADDR);

	/*
	 * Ensure no MR transaction is in progress:
	 * mr_wr_busy signal must be low
	 */
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRSTAT);
		if ((tmp32 & MRSTAT_MR_BUSY) == MRSTAT_MR_NOT_BUSY)
			succesive_reads++;
		else
			succesive_reads = 0;
	} while (succesive_reads != REQUIRED_MRSTAT_READS);

	/* Set MR_TYPE = 0x0 (Write) and MR_RANK = desired rank */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);
	tmp32 &= ~(MRCTRL0_MR_TYPE_READ);
	tmp32 = (tmp32 & ~(MRCTRL0_RANK_ACCESS_FIELD <<
			   MRCTRL0_RANK_ACCESS_POS)) |
		(mr_rank << MRCTRL0_RANK_ACCESS_POS);
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);

	/* Configure MR address: MRCTRL1[8:15] and MR data: MRCTRL1[0:7]*/
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL1);
	tmp32 = (tmp32 & (MRCTRL1_MR_DATA_ADDRESS_FIELD <<
			  MRCTRL1_MR_DATA_ADDRESS_POS)) |
		((uint16_t)mr_index << MRCTRL1_MR_ADDRESS_POS) | mr_data;
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL1);

	dsb();

	/* Initiate MR transaction: MR_WR = 0x1 */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);
	writel(tmp32 | (MRCTRL0_WR_ENGAGE << MRCTRL0_WR_ENGAGE_POS),
	       DDRC_BASE_ADDR + OFFSET_DDRC_MRCTRL0);

	/* Wait until MR transaction completed */
	succesive_reads = 0;
	do {
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_MRSTAT);
		if ((tmp32 & MRSTAT_MR_BUSY) == MRSTAT_MR_NOT_BUSY)
			succesive_reads++;
		else
			succesive_reads = 0;
	} while (succesive_reads != REQUIRED_MRSTAT_READS);

	return NO_ERR;
}

/* Read Critical Delay Differences from message block and store max values */
void read_cdds(void)
{
	uint8_t cdd_rr = 0, cdd_ww = 0, cdd_wr = 0, cdd_rw = 0;
	uint32_t mstr;
	const uint32_t rank0_rw_addr[] = {CDD_CHA_RW_0_0, CDD_CHB_RW_0_0};
	const uint32_t rank0_wr_addr[] = {CDD_CHA_WR_0_0, CDD_CHB_WR_0_0};

	/* Max CDD values for single-rank */
	tr_res.cdd.rr = cdd_rr;
	tr_res.cdd.ww = cdd_ww;
	tr_res.cdd.rw = is_lpddr4() ?
			get_max_cdd(rank0_rw_addr, ARRAY_SIZE(rank0_rw_addr)) :
			readb(CDD_CHA_RW_0_0_DDR3);
	tr_res.cdd.wr = is_lpddr4() ?
			get_max_cdd(rank0_wr_addr, ARRAY_SIZE(rank0_wr_addr)) :
			readb(CDD_CHA_WR_0_0_DDR3);

	/* Check MSTR.active_ranks to identify multi-rank configurations */
	mstr = readl(DDRC_BASE_ADDR);
	if ((mstr & MSTR_ACT_RANKS_MASK) == MSTR_DUAL_RANK_VAL) {
		/* Compute max CDDs for both ranks depending on memory type */
		if (is_lpddr4()) {
			const uint32_t rr_addr[] = {
				CDD_CHA_RR_1_0, CDD_CHA_RR_0_1,
				CDD_CHB_RR_1_0, CDD_CHB_RR_0_1
				};
			const uint32_t ww_addr[] = {
				CDD_CHA_WW_1_0, CDD_CHA_WW_0_1,
				CDD_CHB_WW_1_0, CDD_CHB_WW_0_1
				};
			const uint32_t rw_addr[] = {
				CDD_CHA_RW_1_1, CDD_CHA_RW_1_0,
				CDD_CHA_RW_0_1, CDD_CHB_RW_1_1,
				CDD_CHB_RW_1_0, CDD_CHB_RW_0_1
				};
			const uint32_t wr_addr[] = {
				CDD_CHA_WR_1_1, CDD_CHA_WR_1_0,
				CDD_CHA_WR_0_1, CDD_CHB_WR_1_1,
				CDD_CHB_WR_1_0, CDD_CHB_WR_0_1
				};

			cdd_rr = get_max_cdd(rr_addr, ARRAY_SIZE(rr_addr));
			cdd_rw = get_max_cdd(rw_addr, ARRAY_SIZE(rw_addr));
			cdd_wr = get_max_cdd(wr_addr, ARRAY_SIZE(wr_addr));
			cdd_ww = get_max_cdd(ww_addr, ARRAY_SIZE(ww_addr));
		} else {
			const uint32_t rr_addr[] = {CDD_CHA_RR_1_0_DDR3,
						    CDD_CHA_RR_0_1_DDR3};
			const uint32_t ww_addr[] = {CDD_CHA_WW_1_0_DDR3,
						    CDD_CHA_WW_0_1_DDR3};
			const uint32_t rw_addr[] = {CDD_CHA_RW_1_1_DDR3,
						    CDD_CHA_RW_1_0_DDR3,
						    CDD_CHA_RW_0_1_DDR3};
			const uint32_t wr_addr[] = {CDD_CHA_WR_1_1_DDR3,
						    CDD_CHA_WR_1_0_DDR3,
						    CDD_CHA_WR_0_1_DDR3};

			cdd_rr = get_max_cdd(rr_addr, ARRAY_SIZE(rr_addr));
			cdd_rw = get_max_cdd(rw_addr, ARRAY_SIZE(rw_addr));
			cdd_wr = get_max_cdd(wr_addr, ARRAY_SIZE(wr_addr));
			cdd_ww = get_max_cdd(ww_addr, ARRAY_SIZE(ww_addr));
		}

		/* Update max CDD values if needed */
		if (cdd_rr > tr_res.cdd.rr)
			tr_res.cdd.rr = cdd_rr;
		if (cdd_rw > tr_res.cdd.rw)
			tr_res.cdd.rw = cdd_rw;
		if (cdd_wr > tr_res.cdd.wr)
			tr_res.cdd.wr = cdd_wr;
		if (cdd_ww > tr_res.cdd.ww)
			tr_res.cdd.ww = cdd_ww;
	}
}

/* Read trained VrefCA from message block and store average value */
void read_vref_ca(void)
{
	uint32_t mstr;
	const uint32_t rank0_vref_addr[] = {VREF_CA_A0, VREF_CA_B0};
	const uint32_t rank01_vref_addr[] = {VREF_CA_A0, VREF_CA_A1,
					     VREF_CA_B0, VREF_CA_B1};

	/* Check MSTR.active_ranks to identify multi-rank configurations */
	mstr = readl(DDRC_BASE_ADDR);
	if ((mstr & MSTR_ACT_RANKS_MASK) == MSTR_DUAL_RANK_VAL)
		tr_res.vref_ca = get_avg_vref(rank01_vref_addr,
					      ARRAY_SIZE(rank01_vref_addr));
	else
		tr_res.vref_ca = get_avg_vref(rank0_vref_addr,
					      ARRAY_SIZE(rank0_vref_addr));
}

/* Read trained VrefDQ from message block and store average value*/
void read_vref_dq(void)
{
	uint32_t mstr;
	const uint32_t rank0_vref_addr[] = {VREF_DQ_A0, VREF_DQ_B0};
	const uint32_t rank01_vref_addr[] = {VREF_DQ_A0, VREF_DQ_A1,
					     VREF_DQ_B0, VREF_DQ_B1};

	/* Check MSTR.active_ranks to identify multi-rank configurations */
	mstr = readl(DDRC_BASE_ADDR);
	if ((mstr & MSTR_ACT_RANKS_MASK) == MSTR_DUAL_RANK_VAL)
		tr_res.vref_dq = get_avg_vref(rank01_vref_addr,
					      ARRAY_SIZE(rank01_vref_addr));
	else
		tr_res.vref_dq = get_avg_vref(rank0_vref_addr,
					      ARRAY_SIZE(rank0_vref_addr));
}

/* Calculate DFITMG1.dfi_t_wrdata_delay */
void compute_tphy_wrdata_delay(void)
{
	const uint32_t single_rank_dly_addr[] = {
		DBYTE0_TXDQSDLYTG0_U0, DBYTE0_TXDQSDLYTG0_U1,
		DBYTE1_TXDQSDLYTG0_U0, DBYTE1_TXDQSDLYTG0_U1,
		DBYTE2_TXDQSDLYTG0_U0, DBYTE2_TXDQSDLYTG0_U1,
		DBYTE3_TXDQSDLYTG0_U0, DBYTE3_TXDQSDLYTG0_U1
		};

	const uint32_t dual_rank_dly_addr[] = {
		DBYTE0_TXDQSDLYTG1_U0, DBYTE0_TXDQSDLYTG1_U1,
		DBYTE1_TXDQSDLYTG1_U0, DBYTE1_TXDQSDLYTG1_U1,
		DBYTE2_TXDQSDLYTG1_U0, DBYTE2_TXDQSDLYTG1_U1,
		DBYTE3_TXDQSDLYTG1_U0, DBYTE3_TXDQSDLYTG1_U1
		};

	uint16_t tx_dqsdly, tx_dqsdly_tg1, tctrl_delay, burst_length,
		 wrdata_use_dfi_phy_clk;
	uint32_t mstr, dfitmg0;

	/* Compute max tx_dqdqsdly for rank 0 */
	tx_dqsdly = get_max_delay(single_rank_dly_addr,
				  ARRAY_SIZE(single_rank_dly_addr));

	/* Check MSTR.active_ranks to identify multi-rank configurations */
	mstr = readl(DDRC_BASE_ADDR);
	if ((mstr & MSTR_ACT_RANKS_MASK) == MSTR_DUAL_RANK_VAL) {
		/* Compute max tx_dqdqsdly for rank 1 */
		tx_dqsdly_tg1 = get_max_delay(dual_rank_dly_addr,
					      ARRAY_SIZE(dual_rank_dly_addr));
		if (tx_dqsdly_tg1 > tx_dqsdly)
			tx_dqsdly = tx_dqsdly_tg1;
	}

	/* Extract coarse delay value + 1 for fine delay */
	tx_dqsdly = (tx_dqsdly >> TXDQDLY_COARSE) + 1U;

	/* Compute tctrl_delay */
	tctrl_delay = (uint16_t)((readw(ARDPTR_INITVAL_ADDR) / 2U) +
				 (DDRPHY_PIPE_DFI_MISC * 2U) + 3U);

	burst_length = (uint16_t)(mstr >> MSTR_BURST_RDWR_POS) &
		       MSTR_BURST_RDWR_MASK;
	dfitmg0 = readw(DDRC_BASE_ADDR + OFFSET_DDRC_DFITMG0);
	wrdata_use_dfi_phy_clk = (uint16_t)(dfitmg0 >> DFITMG0_PHY_CLK_POS) &
				 DFITMG0_PHY_CLK_MASK;

	/* Program */
	tr_res.tphy_wrdata_delay = tctrl_delay + 6U + burst_length +
				   wrdata_use_dfi_phy_clk + tx_dqsdly;
	tr_res.tphy_wrdata_delay = (tr_res.tphy_wrdata_delay / 2U) +
				   (tr_res.tphy_wrdata_delay % 2U);
}

/* Re-program some of the DDRC registers based on post-training results. */
static uint32_t adjust_ddrc_config(void)
{
	uint32_t tmp32;
	uint8_t rd_gap, wr_gap, rd_gap_new, wr_gap_new, delta, min;
	uint8_t rd_gap_lp4 = 4, rd_gap_ddr3 = 2, wr_gap_lp4 = 5;
	uint8_t wr_gap_ddr3 = 3, min_lp4 = 7, min_ddr3 = 0xe, max = 0xf;
	uint32_t ret = NO_ERR;

	/* DRAMTMG2.rd2wr & DRAMTMG2.wr2rd */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DRAMTMG2);
	delta = (uint8_t)((tr_res.cdd.rw + (tr_res.cdd.rw % 2U)) / 2U);
	if (!update_bf(&tmp32, DRAMTMG2_RD_WR_POS, DRAMTMG2_RD_WR_MASK,
		       (int32_t)delta))
		return BITFIELD_EXCEEDED;
	delta = (uint8_t)((tr_res.cdd.ww + (tr_res.cdd.ww % 2U)) / 2U);
	if (!update_bf(&tmp32, DRAMTMG2_WR_RD_POS, DRAMTMG2_WR_RD_MASK,
		       (int32_t)delta))
		return BITFIELD_EXCEEDED;
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_DRAMTMG2);

	/* For LPDDR4 overwrite INIT6 and INIT7 DDRC registers. */
	if (is_lpddr4()) {
		/* INIT6.mr5 */
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_INIT6);
		tmp32 = (tmp32 & ~(INIT6_MR5_MASK)) | tr_res.vref_ca;
		writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_INIT6);

		/* INIT7.mr6 */
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_INIT7);
		tmp32 = (tmp32 & ~(INIT7_MR6_MASK)) | tr_res.vref_dq;
		writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_INIT7);
	}

	/* DFITMG1.dfi_t_wrdata_delay */
	tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_DFITMG1);
	tmp32 &= ~(DFITMG1_WRDATA_DELAY_MASK << DFITMG1_WRDATA_DELAY_POS);
	tmp32 |= (((uint32_t)tr_res.tphy_wrdata_delay) <<
		  DFITMG1_WRDATA_DELAY_POS);
	writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_DFITMG1);

	/* For multi-rank systems */
	tmp32 = readl(DDRC_BASE_ADDR);
	if ((tmp32 & MSTR_ACT_RANKS_MASK) == MSTR_DUAL_RANK_VAL) {
		uint8_t rd_gap_ct = is_lpddr4() ? rd_gap_lp4 : rd_gap_ddr3;
		uint8_t wr_gap_ct = is_lpddr4() ? wr_gap_lp4 : wr_gap_ddr3;

		min = is_lpddr4() ? min_lp4 : min_ddr3;
		tmp32 = readl(DDRC_BASE_ADDR + OFFSET_DDRC_RANKCTL);
		/* RANKCTL.diff_rank_rd_gap */
		rd_gap = (uint8_t)((tmp32 >> RANKCTL_RD_GAP_POS) &
				   RANKCTL_RD_GAP_MASK);
		rd_gap_new = (uint8_t)((rd_gap_ct + tr_res.cdd.rr +
					(tr_res.cdd.rr % 2U)) / 2U);

		/* ensure min and max of rd_gap field */
		rd_gap_new = (rd_gap_new < min) ? min : ((rd_gap_new > max) ?
							 max : rd_gap_new);
		if (rd_gap_new > rd_gap) {
			delta = (uint8_t)(rd_gap_new - rd_gap);
			if (!update_bf(&tmp32, RANKCTL_RD_GAP_POS,
				       RANKCTL_RD_GAP_MASK, (int32_t)delta))
				return BITFIELD_EXCEEDED;
		}

		/* RANKCTL.diff_rank_wr_gap */
		wr_gap = (uint8_t)((tmp32 >> RANKCTL_WR_GAP_POS) &
				   RANKCTL_WR_GAP_MASK);
		wr_gap_new = (uint8_t)((wr_gap_ct + tr_res.cdd.ww +
					(tr_res.cdd.ww % 2U)) / 2U);

		/* ensure min and max of wr_gap field */
		wr_gap_new = (wr_gap_new < min) ? min : ((wr_gap_new > max) ?
							 max : wr_gap_new);
		if (wr_gap_new > wr_gap) {
			delta = (uint8_t)(wr_gap_new - wr_gap);
			if (!update_bf(&tmp32, RANKCTL_WR_GAP_POS,
				       RANKCTL_WR_GAP_MASK, (int32_t)delta))
				return BITFIELD_EXCEEDED;
		}

		if (rd_gap_new > rd_gap || wr_gap_new > wr_gap)
			writel(tmp32, DDRC_BASE_ADDR + OFFSET_DDRC_RANKCTL);
	}

	return ret;
}

/* Check if memory type is LPDDR4 using MSTR register */
static bool is_lpddr4(void)
{
	uint32_t mstr;

	mstr = readl(DDRC_BASE_ADDR);
	return ((mstr & MSTR_DRAM_MASK) == MSTR_LPDDR4_VAL);
}

/*
 * Get maximum critical delay difference value.
 * @param cdd_addr[] - list of CDD memory addresses
 * @param size - number of CDDs to be read
 * @return max CDD value
 */
static uint8_t get_max_cdd(const uint32_t cdd_addr[], size_t size)
{
	uint8_t cdd, max = 0;
	int8_t signed_cdd;
	size_t i;

	for (i = 0; i < size; i++) {
		/* CDD has type int8_t - read as unsigned and cast to signed */
		signed_cdd = (int8_t)(readb(cdd_addr[i]));
		/* We need to use absolute value */
		cdd = (uint8_t)((signed_cdd >= 0) ? signed_cdd : -signed_cdd);
		max = (cdd > max) ? cdd : max;
	}
	return max;
}

/*
 * Get maximum delay value.
 * @param delay_addr[] - list of CDD memory addresses
 * @param size - number of values to be read
 * @return max delay value
 */
static uint16_t get_max_delay(const uint32_t delay_addr[], size_t size)
{
	uint16_t value, max = 0;
	size_t i;

	for (i = 0; i < size; i++) {
		value = readw(delay_addr[i]);
		max = (value > max) ? value : max;
	}
	return max;
}

/*
 * Compute average vref value.
 * @param vref_addr[] - list of vref memory addresses
 * @param size - number of values to be read
 * @return average vref value
 */
static uint8_t get_avg_vref(const uint32_t vref_addr[], size_t size)
{
	uint32_t sum = 0;
	size_t i;

	for (i = 0; i < size; i++)
		sum += readb(vref_addr[i]);

	return (uint8_t)(sum / size);
}
