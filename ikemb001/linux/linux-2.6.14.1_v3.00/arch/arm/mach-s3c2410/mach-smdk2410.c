/**********************************************************************
 *
 * linux/arch/arm/mach-s3c2410/mach-smdk2410.c
 *
 * Copyright (C) 2004 by FS Forth-Systeme GmbH
 * All rights reserved.
 *
 * $Id: mach-smdk2410.c,v 1.1 2004/05/11 14:15:38 mpietrek Exp $
 * @Author: Jonas Dietsche
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * @History:
 * derived from linux/arch/arm/mach-s3c2410/mach-bast.c, written by
 * Ben Dooks <ben@simtec.co.uk>
 *
 * 10-Mar-2005 LCVR  Changed S3C2410_VA to S3C24XX_VA
 * 20-Sep-2005 BJD  Added static to non-exported items
 *
 ***********************************************************************/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/dm9000.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <asm/arch/regs-serial.h>
#include <asm/arch/s3c2410fb.h>
#include <asm/arch/regs-lcd.h>
#include "devs.h"
#include "cpu.h"



static struct map_desc smdk2410_iodesc[] __initdata = {
  /* nothing here yet */
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct resource s3c_d9k_resource[] = {
       [0] = {
                .start = 0x08000000 + 0x300,
                .end   = 0x08000000 + 0x300 + 0x03,
                .flags = IORESOURCE_MEM
        },
       [1] = {
                .start = 0x08000000 + 0x300 + 0x4,
                .end   = 0x08000000 + 0x300 + 0x4 + 0x7f,
                .flags = IORESOURCE_MEM
        },
       [2] = {
                .start = IRQ_EINT0,
                .end   = IRQ_EINT0,
                .flags = IORESOURCE_IRQ
        }
};
static struct dm9000_plat_data s3c_device_d9k_platdata = {
        .flags= DM9000_PLATF_16BITONLY
};

struct platform_device s3c_device_d9k = {
        .name= "dm9000",
        .id= 0,
        .num_resources= ARRAY_SIZE(s3c_d9k_resource),
        .resource= s3c_d9k_resource,
        .dev= {
	.platform_data = &s3c_device_d9k_platdata,
        }
};

static struct s3c2410_uartcfg smdk2410_uartcfgs[] = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	}
};

static struct platform_device *smdk2410_devices[] __initdata = {
	&s3c_device_usb,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c,
	&s3c_device_iis,
	&s3c_device_nand,
	&s3c_device_d9k,
	&s3c_device_adc,
	&s3c_device_hpi,
	&s3c_device_wig,
	&s3c_device_rtc,
};

static struct s3c24xx_board smdk2410_board __initdata = {
	.devices       = smdk2410_devices,
	.devices_count = ARRAY_SIZE(smdk2410_devices)
};

static struct s3c2410fb_mach_info smdk2410_lcd_platdata = {
	.width= 640,
	.height= 480,
	.xres = {
		.defval= 640,
		.min= 640,
		.max= 640,
	},
	.yres = {
		.defval= 480,
		.min= 480,
		.max= 480,
	},
	.bpp = {
		.defval= 16,
		.min= 16,
		.max= 16,
	},
	.regs = {
		.lcdcon1= S3C2410_LCDCON1_TFT16BPP | S3C2410_LCDCON1_TFT | S3C2410_LCDCON1_CLKVAL(1),
		.lcdcon2= S3C2410_LCDCON2_LINEVAL(639) | S3C2410_LCDCON2_VBPD(47) | S3C2410_LCDCON2_VFPD(15) | S3C2410_LCDCON2_VSPW(95),
		.lcdcon3= S3C2410_LCDCON3_HOZVAL(479) | S3C2410_LCDCON3_HBPD(34) | S3C2410_LCDCON3_HFPD(11),
		.lcdcon4= S3C2410_LCDCON4_HSPW(1) | S3C2410_LCDCON4_MVAL(13),
		.lcdcon5= S3C2410_LCDCON5_FRM565 | S3C2410_LCDCON5_HWSWP | S3C2410_LCDCON5_PWREN |S3C2410_LCDCON5_INVVFRAME | S3C2410_LCDCON5_INVVLINE,
	},
	.gpcup= 0xFFFFFFFF,
	.gpcup_mask= 0xFFFFFFFF,
	.gpccon= 0xaaaaaaaa,
	.gpccon_mask= 0xFFFFFFFF,
	.gpdup= 0xFFFFFFFF,
	.gpdup_mask= 0xFFFFFFFF,
	.gpdcon= 0xaaaaaaaa,
	.gpdcon_mask= 0xFFFFFFFF,
	.lpcsel= 0x00,
};
////////////////

static void __init smdk2410_map_io(void)
{
	set_s3c2410fb_info(&smdk2410_lcd_platdata);
	s3c24xx_init_io(smdk2410_iodesc, ARRAY_SIZE(smdk2410_iodesc));
	s3c24xx_init_clocks(0);
	s3c24xx_init_uarts(smdk2410_uartcfgs, ARRAY_SIZE(smdk2410_uartcfgs));
	s3c24xx_set_board(&smdk2410_board);
}

static void __init smdk2410_init_irq(void)
{
	s3c24xx_init_irq();
}

MACHINE_START(SMDK2410, "SMDK2410") /* @TODO: request a new identifier and switch
				    * to SMDK2410 */
	/* Maintainer: Jonas Dietsche */
	.phys_ram	= S3C2410_SDRAM_PA,
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,
	.map_io		= smdk2410_map_io,
	.init_irq	= smdk2410_init_irq,
	.timer		= &s3c24xx_timer,
MACHINE_END


