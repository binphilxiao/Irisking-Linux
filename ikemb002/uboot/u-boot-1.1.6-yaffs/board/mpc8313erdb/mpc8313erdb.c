/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006-2007. 
 * 
 * Initialized by Nick.Spence@freescale.com
 *                Wilson.Lo@freescale.com
 * ChangeLog
 * 200700402	Tanya Jiang (tanya.jiang@freescale.com)
 *		Add fixup for NAND on REVA and REVA1 board
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
#include <asm/mpc8349_pci.h>
#include <spd.h>
#include <miiphy.h>
#include <command.h>
#include <ft_build.h>
#if defined(CONFIG_SPD_EEPROM)
#include <spd_sdram.h>
#endif

int board_early_init_f(void)
{
#ifdef CONFIG_PM
	DECLARE_GLOBAL_DATA_PTR;
	volatile immap_t *im = (immap_t *)CFG_IMMR;
	
	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		gd->flags |= GD_FLG_SILENT;
#endif
	return 0;
}

#define ns2clk(ns) (ns / (1000000000 / CONFIG_83XX_CLKIN) + 1)

int checkboard(void)
{
	uint*	led_ptr;
	volatile immap_t *im = (immap_t *) CFG_IMMR;

	puts("Board: Freescale MPC8313ERDB\n");
	
	led_ptr = (uint*) (0xFA000000);

	/* fixup for MPC8313ERDB REVA board, ORx[FCM_BCTLD] need to be set;
	 * the default value of CFG_ORi_PRELIM[FCM_BCTLD] is clear
	 * that is ok for REVA1 and REVA2 board to identify NAND device
	 * SW4.3[REVA] 0 stands for REVA
	 * SW4.3[REVA] 1 stands for REVA1/REVA2
	 */
	if ( (*led_ptr & 0x1) == 0) {
#if defined(CFG_NAND_OR_PRELIM) && defined(CFG_NAND_BR_PRELIM)
#if !defined(CONFIG_NAND_U_BOOT ) 
	im->lbus.bank[1].or = CFG_NAND_OR_PRELIM | OR_FCM_BCTLD;
#else
#warning CONFIG_NAND_U_BOOT will not work on MPC8313ERDB REVA board
#endif
#endif
	}
	*led_ptr = 0x00000000;

	return 0;
}
#if defined(CONFIG_OF_FLAT_TREE) && defined(CONFIG_OF_BOARD_SETUP)

extern void ft_cpu_setup(void *blob, bd_t *bd);
extern void ft_pci_setup(void *blob, bd_t *bd);

void
ft_board_setup(void *blob, bd_t *bd)
{
	u32 *p;
	int len;

#ifdef CONFIG_PCI
	ft_pci_setup(blob, bd);
#endif
	ft_cpu_setup(blob, bd);

	p = ft_get_prop(blob, "/memory/reg", &len);
	if (p != NULL) {
		*p++ = cpu_to_be32(bd->bi_memstart);
		*p = cpu_to_be32(bd->bi_memsize);
	}
}
#endif
