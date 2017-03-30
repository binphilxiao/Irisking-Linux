/* linux/arch/arm/mach-s3c2410/devs.c
 *
 * Copyright (c) 2004 Simtec Electronics
 * Ben Dooks <ben@simtec.co.uk>
 *
 * Base S3C2410 platform device definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Modifications:
 *     10-Mar-2005 LCVR Changed S3C2410_{VA,SZ} to S3C24XX_{VA,SZ}
 *     10-Feb-2005 BJD  Added camera from guillaume.gourat@nexvision.tv
 *     29-Aug-2004 BJD  Added timers 0 through 3
 *     29-Aug-2004 BJD  Changed index of devices we only have one of to -1
 *     21-Aug-2004 BJD  Added IRQ_TICK to RTC resources
 *     18-Aug-2004 BJD  Created initial version
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>
#include <asm/arch/nand.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <asm/arch/fb.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/arch/regs-serial.h>
#include <asm/arch/s3c2410_lcd.h>
#include <asm/arch/s3c2410_ts.h>
#include "devs.h"

/* Serial port registrations */

struct platform_device *s3c24xx_uart_devs[3];

/* USB Host Controller */

static struct resource s3c_usb_resource[] = {
	[0] = {
		.start = S3C2410_PA_USBHOST,
		.end   = S3C2410_PA_USBHOST + S3C24XX_SZ_USBHOST,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_USBH,
		.end   = IRQ_USBH,
		.flags = IORESOURCE_IRQ,
	}
};

static u64 s3c_device_usb_dmamask = 0xffffffffUL;

struct platform_device s3c_device_usb = {
	.name		  = "s3c2410-ohci",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_usb_resource),
	.resource	  = s3c_usb_resource,
	.dev              = {
		.dma_mask = &s3c_device_usb_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

EXPORT_SYMBOL(s3c_device_usb);

/* HPI Controllor */
static struct resource s3c_hpi_resource[] = {
	[0] = {
		.start = 0x20000000,
		.end   = 0x20000001,
		.flags = IORESOURCE_MEM,
	},
        [1] = {
		.start = 0x22000000,
		.end   = 0x22000001,
                .flags = IORESOURCE_MEM,
        },
        [2] = {
                .start = 0x20800000,
		.end   = 0x20800001,
                .flags = IORESOURCE_MEM,
	},
        [3] = {
                .start = 0x22800000,
                .end   = 0x22800000,
                .flags = IORESOURCE_MEM,
        },
        [4] = {
                .start = 0x21000000,
                .end   = 0x21000001,
                .flags = IORESOURCE_MEM,
        },
        [5] = {
                .start = 0x23000000,
		.end   = 0x23000001,
                .flags = IORESOURCE_MEM,
        },
        [6] = {
                .start = 0x21800000,
		.end   = 0x21800001,
                .flags = IORESOURCE_MEM,
	},
        [7] = {
		.start = 0x23800000,
		.end   = 0x23800001,
		.flags = IORESOURCE_MEM,			
	},

};

struct platform_device s3c_device_hpi = {
	.name            = "s3c2410-hpi",
	.id              = -1,
	.num_resources   = ARRAY_SIZE(s3c_hpi_resource),
	.resource        = s3c_hpi_resource
};

EXPORT_SYMBOL(s3c_device_hpi);

/* LCD Controller */

static struct s3c2410fb_mach_info s3c2410fb_info;

void __init set_s3c2410fb_info(struct s3c2410fb_mach_info *hard_s3c2410fb_info)
{
	        memcpy(&s3c2410fb_info,hard_s3c2410fb_info,sizeof(struct s3c2410fb_mach_info));
		        s3c_device_lcd.dev.platform_data = &s3c2410fb_info;
}
EXPORT_SYMBOL(set_s3c2410fb_info);

static struct resource s3c_lcd_resource[] = {
	[0] = {
		.start = S3C2410_PA_LCD,
		.end   = S3C2410_PA_LCD + S3C24XX_SZ_LCD,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_LCD,
		.end   = IRQ_LCD,
		.flags = IORESOURCE_IRQ,
	},
	
       /* [2] = {
		.start = 0x20080000,
		.end   = 0x20080001,
		.flags = IORESOURCE_MEM,
	},
	[3] = {
		.start = 0x22080000,
		.end   = 0x22080001,
		.flags = IORESOURCE_MEM,
	},
	[4] = {
		.start = 0x20880000,
		.end   = 0x20880001,
		.flags = IORESOURCE_MEM,
	},
	[5] = {
		.start = 0x22880000,
		.end   = 0x22880000,
		.flags = IORESOURCE_MEM,
	},
	[6] = {
		.start = 0x21080000,
		.end   = 0x21080001,
		.flags = IORESOURCE_MEM,
	},
	[7] = {
		.start = 0x23080000,
		.end   = 0x23080001,
		.flags = IORESOURCE_MEM,
	},
	[8] = {
		.start = 0x21880000,
		.end   = 0x21880001,
		.flags = IORESOURCE_MEM,
	},
	[9] = {
		.start = 0x23880000,
		.end   = 0x23880001,
		.flags = IORESOURCE_MEM,                        
	},*/
};

static u64 s3c_device_lcd_dmamask = 0xffffffffUL;

struct platform_device s3c_device_lcd = {
	.name		  = "s3c2410-lcd",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_lcd_resource),
	.resource	  = s3c_lcd_resource,
	.dev              = {
		.platform_data	= &s3c2410fb_info,
		.dma_mask		= &s3c_device_lcd_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};

EXPORT_SYMBOL(s3c_device_lcd);


/* NAND Controller */

static struct mtd_partition partition_info[] ={
	        { /* 128k */
name: "bootloader",
            size: 0x00030000,
	    offset: 0x0,
		      },
	        { /* 1MB+832k */
name: "kernel",
            size: 0x001d0000,
	    offset: 0x00030000,
		          },
	        { /* 2MB */
name: "root",
            size: 0x00600000,
	    offset: 0x00200000,
		          },
	        { /* 60MB */
name: "user",
            size: 0x00800000,
	    offset: 0x00800000,
		          },
		{
name: "lib",
            size: 0x00800000,
	    offset:0x01000000,
		          },
                {
name: "etc1",
                  size: 0x00800000,
		              offset:0x01800000,
			                                },
                {
name: "etc2",
                  size: 0x00800000,
		              offset:0x02000000,
			                                },
                {
name: "etc3",
                  size: 0x01400000,
		              offset:0x02800000,
			                                }
};

struct s3c2410_nand_set nandset ={
	      nr_partitions: 8, /* the number of partitions */
			           partitions: partition_info, /* partition table */
};

struct s3c2410_platform_nand superlpplatform={
	      tacls:0,
		          twrph0:30,
			        twrph1:0,
				      sets: &nandset,
				            nr_sets: 1,
};

static struct resource s3c_nand_resource[] = {
	        [0] = {
			                .start = S3C2410_PA_NAND,
					                .end   = S3C2410_PA_NAND + S3C24XX_SZ_NAND - 1,
							                .flags = IORESOURCE_MEM,
									        }
};


struct platform_device s3c_device_nand = {
	        .name = "s3c2410-nand",    /* Device name */
		        .id = -1,                  /* Device ID */
			        .num_resources = ARRAY_SIZE(s3c_nand_resource),
				        .resource = s3c_nand_resource, /* Nand Flash Controller Registers */
					        /* Add the Nand Flash device */
					        .dev = {
							        .platform_data = &superlpplatform
									        }
};

EXPORT_SYMBOL(s3c_device_nand);

/* Touchscreen */
static struct s3c2410_ts_mach_info s3c2410ts_info;

void __init set_s3c2410ts_info(struct s3c2410_ts_mach_info *hard_s3c2410ts_info)
{
		memcpy(&s3c2410ts_info,hard_s3c2410ts_info,sizeof(struct s3c2410_ts_mach_info));
}
EXPORT_SYMBOL(set_s3c2410ts_info);

struct platform_device s3c_device_ts = {
		.name		  = "s3c2410-ts",
			.id		  = -1,
				.dev              = {
					 		.platform_data	= &s3c2410ts_info,
								}
};
EXPORT_SYMBOL(s3c_device_ts);



/* USB Device (Gadget)*/

static struct resource s3c_usbgadget_resource[] = {
	[0] = {
		.start = S3C2410_PA_USBDEV,
		.end   = S3C2410_PA_USBDEV + S3C24XX_SZ_USBDEV,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_USBD,
		.end   = IRQ_USBD,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_usbgadget = {
	.name		  = "s3c2410-usbgadget",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_usbgadget_resource),
	.resource	  = s3c_usbgadget_resource,
};

EXPORT_SYMBOL(s3c_device_usbgadget);

/* Watchdog */

static struct resource s3c_wdt_resource[] = {
	[0] = {
		.start = S3C2410_PA_WATCHDOG,
		.end   = S3C2410_PA_WATCHDOG + S3C24XX_SZ_WATCHDOG,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_WDT,
		.end   = IRQ_WDT,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_wdt = {
	.name		  = "s3c2410-wdt",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_wdt_resource),
	.resource	  = s3c_wdt_resource,
};

EXPORT_SYMBOL(s3c_device_wdt);

/* I2C */

static struct resource s3c_i2c_resource[] = {
	[0] = {
		.start = S3C2410_PA_IIC,
		.end   = S3C2410_PA_IIC + S3C24XX_SZ_IIC,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_IIC,
		.end   = IRQ_IIC,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_i2c = {
	.name		  = "s3c2410-i2c",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_i2c_resource),
	.resource	  = s3c_i2c_resource,
};

EXPORT_SYMBOL(s3c_device_i2c);

/* IIS */

static struct resource s3c_iis_resource[] = {
	[0] = {
		.start = S3C2410_PA_IIS,
		.end   = S3C2410_PA_IIS + S3C24XX_SZ_IIS,
		.flags = IORESOURCE_MEM,
	}
};

static u64 s3c_device_iis_dmamask = 0xffffffffUL;

struct platform_device s3c_device_iis = {
	.name		  = "s3c2410-iis",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_iis_resource),
	.resource	  = s3c_iis_resource,
	.dev              = {
		.dma_mask = &s3c_device_iis_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

EXPORT_SYMBOL(s3c_device_iis);

/* RTC */

static struct resource s3c_rtc_resource[] = {
	[0] = {
		.start = S3C2410_PA_RTC,
		.end   = S3C2410_PA_RTC + 0xff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_RTC,
		.end   = IRQ_RTC,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = IRQ_TICK,
		.end   = IRQ_TICK,
		.flags = IORESOURCE_IRQ
	}
};

struct platform_device s3c_device_rtc = {
	.name		  = "s3c2410-rtc",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_rtc_resource),
	.resource	  = s3c_rtc_resource,
};

EXPORT_SYMBOL(s3c_device_rtc);

/* ADC */

static struct resource s3c_adc_resource[] = {
	[0] = {
		.start = S3C2410_PA_ADC,
		.end   = S3C2410_PA_ADC + S3C24XX_SZ_ADC,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TC,
		.end   = IRQ_ADC,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_adc = {
	.name		  = "s3c2410-adc",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_adc_resource),
	.resource	  = s3c_adc_resource,
};

EXPORT_SYMBOL(s3c_device_adc);
/* SDI */

static struct resource s3c_sdi_resource[] = {
	[0] = {
		.start = S3C2410_PA_SDI,
		.end   = S3C2410_PA_SDI + S3C24XX_SZ_SDI,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_SDI,
		.end   = IRQ_SDI,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_sdi = {
	.name		  = "s3c2410-sdi",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_sdi_resource),
	.resource	  = s3c_sdi_resource,
};

EXPORT_SYMBOL(s3c_device_sdi);

/* SPI (0) */

static struct resource s3c_spi0_resource[] = {
	[0] = {
		.start = S3C2410_PA_SPI,
		.end   = S3C2410_PA_SPI + 0x1f,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_SPI0,
		.end   = IRQ_SPI0,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_spi0 = {
	.name		  = "s3c2410-spi",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(s3c_spi0_resource),
	.resource	  = s3c_spi0_resource,
};

EXPORT_SYMBOL(s3c_device_spi0);

/* SPI (1) */

static struct resource s3c_spi1_resource[] = {
	[0] = {
		.start = S3C2410_PA_SPI + 0x20,
		.end   = S3C2410_PA_SPI + 0x20 + 0x1f,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_SPI1,
		.end   = IRQ_SPI1,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_spi1 = {
	.name		  = "s3c2410-spi",
	.id		  = 1,
	.num_resources	  = ARRAY_SIZE(s3c_spi1_resource),
	.resource	  = s3c_spi1_resource,
};

EXPORT_SYMBOL(s3c_device_spi1);

/* pwm timer blocks */

static struct resource s3c_timer0_resource[] = {
	[0] = {
		.start = S3C2410_PA_TIMER + 0x0C,
		.end   = S3C2410_PA_TIMER + 0x0C + 0xB,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TIMER0,
		.end   = IRQ_TIMER0,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_timer0 = {
	.name		  = "s3c2410-timer",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(s3c_timer0_resource),
	.resource	  = s3c_timer0_resource,
};

EXPORT_SYMBOL(s3c_device_timer0);

/* timer 1 */

static struct resource s3c_timer1_resource[] = {
	[0] = {
		.start = S3C2410_PA_TIMER + 0x18,
		.end   = S3C2410_PA_TIMER + 0x23,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TIMER1,
		.end   = IRQ_TIMER1,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_timer1 = {
	.name		  = "s3c2410-timer",
	.id		  = 1,
	.num_resources	  = ARRAY_SIZE(s3c_timer1_resource),
	.resource	  = s3c_timer1_resource,
};

EXPORT_SYMBOL(s3c_device_timer1);

/* timer 2 */

static struct resource s3c_timer2_resource[] = {
	[0] = {
		.start = S3C2410_PA_TIMER + 0x24,
		.end   = S3C2410_PA_TIMER + 0x2F,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TIMER2,
		.end   = IRQ_TIMER2,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_timer2 = {
	.name		  = "s3c2410-timer",
	.id		  = 2,
	.num_resources	  = ARRAY_SIZE(s3c_timer2_resource),
	.resource	  = s3c_timer2_resource,
};

EXPORT_SYMBOL(s3c_device_timer2);

/* timer 3 */

static struct resource s3c_timer3_resource[] = {
	[0] = {
		.start = S3C2410_PA_TIMER + 0x30,
		.end   = S3C2410_PA_TIMER + 0x3B,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TIMER3,
		.end   = IRQ_TIMER3,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device s3c_device_timer3 = {
	.name		  = "s3c2410-timer",
	.id		  = 3,
	.num_resources	  = ARRAY_SIZE(s3c_timer3_resource),
	.resource	  = s3c_timer3_resource,
};

EXPORT_SYMBOL(s3c_device_timer3);

#ifdef CONFIG_CPU_S3C2440

/* Camif Controller */

static struct resource s3c_camif_resource[] = {
	[0] = {
		.start = S3C2440_PA_CAMIF,
		.end   = S3C2440_PA_CAMIF + S3C2440_SZ_CAMIF,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_CAM,
		.end   = IRQ_CAM,
		.flags = IORESOURCE_IRQ,
	}

};

static u64 s3c_device_camif_dmamask = 0xffffffffUL;

struct platform_device s3c_device_camif = {
	.name		  = "s3c2440-camif",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_camif_resource),
	.resource	  = s3c_camif_resource,
	.dev              = {
		.dma_mask = &s3c_device_camif_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

EXPORT_SYMBOL(s3c_device_camif);

#endif // CONFIG_CPU_S32440
