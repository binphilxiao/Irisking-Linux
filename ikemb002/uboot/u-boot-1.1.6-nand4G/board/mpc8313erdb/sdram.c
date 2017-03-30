/*
 * Copyright (C) Freescale Semiconductor, Inc. 2007. 
 *
 * Origin from MPC8313EMDS of Nick.Spence@freescale.com
 *
 * (C) Copyright 2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 */

#include <common.h>
#include <ioports.h>
#include <mpc83xx.h>
#include <asm/mmu.h>
#include <spd.h>
#include <command.h>
#if defined(CONFIG_SPD_EEPROM)
#include <spd_sdram.h>
#endif

int fixed_sdram(void);

#if defined(CONFIG_NAND_SPL)
#define puts(v) {}
#define udelay(x) { int i,j; for (i=0; i<x; i++) for (j=0;j<10000;j++); }
#endif

long int initdram(int board_type)
{
	volatile immap_t *im = (immap_t *) CFG_IMMR;
	volatile lbus83xx_t *lbc= &im->lbus;

	u32 msize = 0;

	if ((im->sysconf.immrbar & IMMRBAR_BASE_ADDR) != (u32) im)
		return -1;

	puts("Initializing\n");

	/* DDR SDRAM - Main SODIMM */
	im->sysconf.ddrlaw[0].bar = CFG_DDR_BASE & LAWBAR_BAR;
#if defined(CONFIG_SPD_EEPROM)
	msize = spd_sdram();
#else
	msize = fixed_sdram();
#endif

	/* Local Bus setup lbcr and mrtpr */
	lbc->lbcr = CFG_LBC_LBCR;
	lbc->mrtpr = CFG_LBC_MRTPR;
	asm("sync");

	puts("   DDR RAM: ");
	/* return total bus SDRAM size(bytes)  -- DDR */
	return (msize * 1024 * 1024);
}

#ifdef CONFIG_PM
static void resume_from_s3(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	u32 magic = *(u32 *)0;

	typedef void (*func_t)(void);
	func_t resume = *(func_t *)4;

	if (magic == 0xf5153ae5)
		resume();

	gd->flags &= ~GD_FLG_SILENT;
	puts("\nS3 Resume failed: bad magic word\n");
}
#endif

#if !defined(CONFIG_SPD_EEPROM)
/*************************************************************************
 *  fixed sdram init -- doesn't use serial presence detect.
 ************************************************************************/
int fixed_sdram(void)
{
	u32 msize = CFG_DDR_SIZE;
#if defined(CONFIG_NAND_SPL) || (!defined(CFG_RAMBOOT) && !defined(CONFIG_NAND_U_BOOT))
	volatile immap_t *im = (immap_t *) CFG_IMMR;
	u32 ddr_size;
	u32 ddr_size_log2;

	msize = CFG_DDR_SIZE;

	for (ddr_size = msize << 20, ddr_size_log2 = 0;
	     (ddr_size > 1);
	     ddr_size = ddr_size >> 1, ddr_size_log2++) {
		if (ddr_size & 1) {
			return -1;
		}
	}
	im->sysconf.ddrlaw[0].bar = ((CFG_DDR_SDRAM_BASE >> 12) & 0xfffff);
	im->sysconf.ddrlaw[0].ar = LAWAR_EN | ((ddr_size_log2 - 1) & LAWAR_SIZE);
	im->sysconf.ddrcdr = CFG_DDRCDR;

#if (CFG_DDR_SIZE != 128)
#warning Currenly any ddr size other than 128 is not supported
#endif
	im->ddr.csbnds[0].csbnds = 0x00000007;
	im->ddr.cs_config[0] = CFG_DDR_CONFIG;

	/* currently we use only one CS, so disable the other banks */
	im->ddr.cs_config[1] = 0;
	im->ddr.cs_config[2] = 0;
	im->ddr.cs_config[3] = 0;

	im->ddr.sdram_clk_cntl = CFG_DDR_CLK_CNTL;
	im->ddr.timing_cfg_3 = CFG_DDR_TIMING_3;
	im->ddr.timing_cfg_1 = CFG_DDR_TIMING_1;
	im->ddr.timing_cfg_2 = CFG_DDR_TIMING_2;
	im->ddr.timing_cfg_0 = CFG_DDR_TIMING_0;
#ifdef CONFIG_PM
	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		im->ddr.sdram_cfg = CFG_SDRAM_CFG | SDRAM_CFG_BI;
	else
		im->ddr.sdram_cfg = CFG_SDRAM_CFG;
#else
	im->ddr.sdram_cfg = CFG_SDRAM_CFG;
#endif
	im->ddr.sdram_cfg2 = CFG_SDRAM_CFG2;
	im->ddr.sdram_mode = CFG_DDR_MODE;
	im->ddr.sdram_mode_2 = CFG_DDR_MODE_2;

	im->ddr.sdram_interval = CFG_DDR_INTERVAL;
	udelay(200);

	/* enable DDR controller */
	im->ddr.sdram_cfg |= SDRAM_CFG_MEM_EN;
#ifdef CONFIG_PM
	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		resume_from_s3();
#endif
#endif
	return msize;
}
#endif /*!CFG_SPD_EEPROM */
