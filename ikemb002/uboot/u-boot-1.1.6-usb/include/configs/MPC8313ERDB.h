/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * History
 * 20061201: Wilson Lo (Wilson.Lo@freescale.com)
 *           Initialized 
 * 20061210: Tanya Jiang (tanya.jiang@freescale.com)
 *           Code Cleanup
 */
/*
 * mpc8313epb board configuration file
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#undef DEBUG

/*
 * High Level Configuration Options
 */
#define CONFIG_E300		1	/* E300 Family */
#define CONFIG_MPC83XX		1	/* MPC83XX family */
#define CONFIG_MPC8313		1	/* MPC8313 specific */
#define CONFIG_MPC8313ERDB	1	/* MPC8313EVAL board specific */

#define CONFIG_PCI

#ifdef PCI_66M
#define CONFIG_83XX_CLKIN	66666666	/* in Hz */
#else
#define CONFIG_83XX_CLKIN	33333333	/* in Hz */
#endif

#ifndef CONFIG_SYS_CLK_FREQ
#ifdef PCI_66M
#define CONFIG_SYS_CLK_FREQ	66666666
#else
#define CONFIG_SYS_CLK_FREQ	33333333
#endif
#endif

/* System performance */
#define CFG_ACR_PIPE_DEP	3	/* Arbiter pipeline depth (0-3) */
#define CFG_ACR_RPTCNT		3	/* Arbiter repeat count (0-7) */
#define CFG_SPCR_TSEC1EP	3	/* eTSEC emergency priority (0-3) */

#define CFG_SCCR		( SCCR_RES \
				| SCCR_TSEC1CM_1	\
				| SCCR_TSEC1ON		\
				| SCCR_TSEC2ON		\
				| SCCR_ENCCM_3		\
				| SCCR_USBCM_3		\
				| SCCR_PCICM		)

#define CONFIG_BOARD_EARLY_INIT_F		/* call board_pre_init */
#undef  CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_EARLY_INIT_R		/* call board_init */
#undef  CONFIG_BOARD_EARLY_INIT_R


#define CFG_IMMR		0xE0000000

#undef CFG_DRAM_TEST				/* memory test, takes time */
#define CFG_MEMTEST_START	0x00001000	/* memtest region */
#define CFG_MEMTEST_END		0x1FF00000

/*
 * DDR Setup
 */
#undef CONFIG_SPD_EEPROM		/* use SPD EEPROM for DDR setup*/

#define CFG_DDR_BASE		0x00000000	/* DDR is system memory*/
#define CFG_SDRAM_BASE		CFG_DDR_BASE
#define CFG_DDR_SDRAM_BASE	CFG_DDR_BASE
#undef  CONFIG_DDR_2T_TIMING

#if defined(CONFIG_SPD_EEPROM)
/*
 * Determine DDR configuration from I2C interface.
 */
#define SPD_EEPROM_ADDRESS	0x51		/* DDR DIMM */
#else
/*
 * Manually set up DDR parameters
 * 256 MByte SODIMM - Micron MT4HTF3264HY-40EB4, 256MB, DDR2, 400, CL3
 * only using 64 data bits of 128 bit wide DIMM
 */
#define CONFIG_VERY_BIG_RAM
#define CSCONFIG_BA_BIT_3       0x00004000
#define CFG_DDR_SIZE		512		/* MB */
#define CFG_DDR_CONFIG		( CSCONFIG_EN | CSCONFIG_AP \
				| 0x00040000 /* TODO */ \
				| CSCONFIG_BA_BIT_3 \
				| CSCONFIG_ROW_BIT_14 | CSCONFIG_COL_BIT_10 )
				/* 0x80840102 */
#define CFG_DDRCDR		( DDRCDR_EN \
				| DDRCDR_PZ_NOMZ \
				| DDRCDR_NZ_NOMZ \
				| DDRCDR_M_ODR )
				/* 0x73000002 TODO ODR & DRN ? */

#define CFG_DDR_CLK_CNTL	DDR_SDRAM_CLK_CNTL_CLK_ADJUST_05
				/*0x02000000*/
#define CFG_DDR_TIMING_3	0x00000000
#define CFG_DDR_TIMING_0	( ( 0 << TIMING_CFG0_RWT_SHIFT ) \
				| ( 0 << TIMING_CFG0_WRT_SHIFT ) \
				| ( 0 << TIMING_CFG0_RRT_SHIFT ) \
				| ( 0 << TIMING_CFG0_WWT_SHIFT ) \
				| ( 2 << TIMING_CFG0_ACT_PD_EXIT_SHIFT ) \
				| ( 2 << TIMING_CFG0_PRE_PD_EXIT_SHIFT ) \
				| ( 8 << TIMING_CFG0_ODT_PD_EXIT_SHIFT ) \
				| ( 2 << TIMING_CFG0_MRS_CYC_SHIFT ) )
				/* 0x00220802 */
#define CFG_DDR_TIMING_1	( ( 3 << TIMING_CFG1_PRETOACT_SHIFT ) \
				| ( 9 << TIMING_CFG1_ACTTOPRE_SHIFT ) \
				| ( 3 << TIMING_CFG1_ACTTORW_SHIFT ) \
				| ( 5 << TIMING_CFG1_CASLAT_SHIFT ) \
				| (13 << TIMING_CFG1_REFREC_SHIFT ) \
				| ( 3 << TIMING_CFG1_WRREC_SHIFT ) \
				| ( 2 << TIMING_CFG1_ACTTOACT_SHIFT ) \
				| ( 2 << TIMING_CFG1_WRTORD_SHIFT ) )
				/* 0x3935d322 */
#define CFG_DDR_TIMING_2	( ( 0 << TIMING_CFG2_ADD_LAT_SHIFT ) \
				| (31 << TIMING_CFG2_CPO_SHIFT ) \
				| ( 2 << TIMING_CFG2_WR_LAT_DELAY_SHIFT ) \
				| ( 2 << TIMING_CFG2_RD_TO_PRE_SHIFT ) \
				| ( 2 << TIMING_CFG2_WR_DATA_DELAY_SHIFT ) \
				| ( 3 << TIMING_CFG2_CKE_PLS_SHIFT ) \
				| (10 << TIMING_CFG2_FOUR_ACT_SHIFT) )
				/* 0x0f9048ca */ /* P9-45,may need tuning */
#define CFG_DDR_INTERVAL	( ( 800 << SDRAM_INTERVAL_REFINT_SHIFT ) \
				| ( 100 << SDRAM_INTERVAL_BSTOPRE_SHIFT ) )
				/* 0x03200064 */
#if defined(CONFIG_DDR_2T_TIMING)
#define CFG_SDRAM_CFG		( SDRAM_CFG_SREN \
				| 3 << SDRAM_CFG_SDRAM_TYPE_SHIFT \
				| SDRAM_CFG_2T_EN \
				| SDRAM_CFG_DBW_32 )
#else
#define CFG_SDRAM_CFG		( SDRAM_CFG_SREN \
				| 3 << SDRAM_CFG_SDRAM_TYPE_SHIFT \
				| SDRAM_CFG_DBW_32 )
				/* 0x43080000 */
#endif
#define CFG_SDRAM_CFG2		0x00401000;
/* set burst length to 8 for 32-bit data path */
#define CFG_DDR_MODE		( ( 0x4440 << SDRAM_MODE_ESD_SHIFT ) \
				| ( 0x0232 << SDRAM_MODE_SD_SHIFT ) )
				/* 0x44400232 */
#define CFG_DDR_MODE_2		0x8000C000;
#endif

/*
 * FLASH on the Local Bus
 */
#define CFG_FLASH_CFI				/* use the Common Flash Interface */
#define CFG_FLASH_CFI_DRIVER			/* use the CFI driver */
#define CFG_FLASH_BASE		0xFE000000	/* start of FLASH   */
#define CFG_FLASH_SIZE		8		/* flash size in MB */
#define CFG_FLASH_EMPTY_INFO			/* display empty sectors */
#define CFG_FLASH_USE_BUFFER_WRITE		/* buffer up multiple bytes */

#define CFG_FLASH_BR_PRELIM	(CFG_FLASH_BASE |    /* flash Base address */ \
				(2 << BR_PS_SHIFT) | /* 16 bit port size */ \
				BR_V)		     /* valid */
#define CFG_FLASH_OR_PRELIM	( 0xFF000000         /* 16 MByte */ \
				| OR_GPCM_XACS \
				| OR_GPCM_SCY_9 \
				| OR_GPCM_EHTR \
				| OR_GPCM_EAD )
				/* 0xFF006FF7	TODO SLOW 16 MB flash size */
#define CFG_LBLAWBAR0_PRELIM	CFG_FLASH_BASE	/* window base at flash base */
#define CFG_LBLAWAR0_PRELIM	0x80000017	/* 16 MB window size */

#define CFG_MAX_FLASH_BANKS	1		/* number of banks */
#define CFG_MAX_FLASH_SECT	135		/* sectors per device */

#undef CFG_FLASH_CHECKSUM
#define CFG_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CFG_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CFG_MONITOR_BASE	TEXT_BASE	/* start of monitor */

#if (CFG_MONITOR_BASE < CFG_FLASH_BASE) && !defined(CONFIG_NAND_U_BOOT)
#define CFG_RAMBOOT
#else
#undef  CFG_RAMBOOT
#endif

#define CFG_INIT_RAM_LOCK	1
#define CFG_INIT_RAM_ADDR	0xFD000000	/* Initial RAM address */
#define CFG_INIT_RAM_END	0x1000		/* End of used area in RAM*/

#define CFG_GBL_DATA_SIZE	0x100		/* num bytes initial data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

#define CFG_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for Mon */
#define CFG_MALLOC_LEN		(128 * 1024)	/* Reserved for malloc */

/*
 * Local Bus LCRR and LBCR regs
 */
#ifdef PCI_66M
#define CFG_LCRR	LCRR_EADC_1 | LCRR_CLKDIV_2	/* 0x00010002 */
#else
#if defined(CONFIG_133M_266M) || defined(CONFIG_133M_400M)
#define CFG_LCRR	LCRR_EADC_1 | LCRR_CLKDIV_2	/* 0x00010002 */
#else
#define CFG_LCRR	LCRR_EADC_1 | LCRR_CLKDIV_4	/* 0x00010004 */
#endif
#endif

#define CFG_LBC_LBCR	( 0x00040000 /* TODO */ \
			| (0xFF << LBCR_BMT_SHIFT) \
			| 0xF )	/* 0x0004ff0f */

#define CFG_LBC_MRTPR	0x20000000  /*TODO */  /* LB refresh timer prescal, 266MHz/32 */

/* drivers/nand/nand.c */
#ifdef CONFIG_NAND_SPL
#define CFG_NAND_BASE		0xFFF00000
#else
#define CFG_NAND_BASE		0xE2800000 /* 0xF0000000 */
#endif
#define CFG_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS		1
#define CONFIG_MTD_NAND_VERIFY_WRITE

#define CFG_NAND_BR_PRELIM	( CFG_NAND_BASE \
				| (2<<BR_DECC_SHIFT) /* Use HW ECC */ \
				| BR_PS_8            /* Port Size = 8 bit */ \
				| BR_MS_FCM          /* MSEL = FCM */ \
				| BR_V )             /* valid */
#define CFG_NAND_OR_PRELIM	( 0xFFFC0000         /* length 32K for small, 256K for large */ \
				| OR_FCM_PGS \
				| OR_FCM_CSCT \
				| OR_FCM_CST \
				| OR_FCM_CHT \
				| OR_FCM_SCY_1 \
				| OR_FCM_TRLX \
				| OR_FCM_EHTR )
				/* 0xFFFF8396 for REVA1/REVA2 board, 
				 * will be fixed to 0xFFFF9396 for REVA board */
#define CFG_LBLAWBAR1_PRELIM	CFG_NAND_BASE
#define CFG_LBLAWAR1_PRELIM	0x80000011	/* 32KB for small, 256K for large */

/*
 * Swap CS0 / CS1 based upon NAND or NOR Flash Boot mode
 */
#if defined(CONFIG_NAND_U_BOOT)
#define CFG_BR0_PRELIM		CFG_NAND_BR_PRELIM  /* NAND Base Address */
#define CFG_OR0_PRELIM		CFG_NAND_OR_PRELIM  /* NAND Options */
#define CFG_BR1_PRELIM		CFG_FLASH_BR_PRELIM /* NOR Base address */
#define CFG_OR1_PRELIM		CFG_FLASH_OR_PRELIM /* NOR Options */
#else
#define CFG_BR0_PRELIM		CFG_FLASH_BR_PRELIM /* NOR Base address */
#define CFG_OR0_PRELIM		CFG_FLASH_OR_PRELIM /* NOR Options */
#define CFG_BR1_PRELIM		CFG_NAND_BR_PRELIM  /* NAND Base Address */
#define CFG_OR1_PRELIM		CFG_NAND_OR_PRELIM  /* NAND Options */
#endif /* CONFIG_NAND_U_BOOT */

/*
 * NAND Boot Configuration, for board/../nand_boot.c
 */
#define CFG_NAND_BR0_PRELIM	CFG_NAND_BR_PRELIM
#define CFG_NAND_OR0_PRELIM	CFG_NAND_OR_PRELIM
#define CFG_NAND_LBLAWBAR0_PRELIM	CFG_NAND_BASE
#define CFG_NAND_LBLAWAR0_PRELIM	0x80000011 /* 32KB for small, 256K for large */

#undef  CFG_NAND_BOOT_QUIET		/* Enable NAND boot status messages */
#define CFG_NAND_BOOT_SHOW_ECC_NUM	/* Show corrected ECC errors */
#define CFG_NAND_RELOC		(0x10000)	/* Stage 1 load address */
#define CFG_NAND_PAGE_SIZE	(2048)		/* NAND chip page size */
#define CFG_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size */
#define CFG_NAND_BAD_BLOCK_POS	(5)		/* Bad block marker location */
#define CFG_NAND_FMR		((14 << FMR_CWTO_SHIFT) | \
				(0 << FMR_AL_SHIFT))

#define CFG_NAND_U_BOOT_SIZE	(384 << 10)	/* Size of RAM U-Boot image */
#define CFG_NAND_U_BOOT_DST	(0x01000000)	/* Load NUB to this addr */
#define CFG_NAND_U_BOOT_START	(CFG_NAND_U_BOOT_DST + 0x120) /* NUB start */
/*
 * JFFS2 configuration
 */
#define CONFIG_JFFS2_NAND

/* mtdparts command line support */
#define CONFIG_JFFS2_CMDLINE
#define MTDIDS_DEFAULT		"nor0=nor,nand0=nand"
#define MTDPARTS_DEFAULT	"mtdparts=nand:-@4m(jffs2)"
#define NAND_CACHE_PAGES	32

#define CFG_VSC7385_BASE	0xF0000000

#define CONFIG_VSC7385_ENET			/* VSC7385 ethernet support */
#define CFG_BR2_PRELIM		0xf0000801	/* VSC7385 Base address */
#define CFG_OR2_PRELIM		0xfffe09ff	/* VSC7385, 128K bytes*/
#define CFG_LBLAWBAR2_PRELIM	CFG_VSC7385_BASE/* Access window base at VSC7385 base */
#define CFG_LBLAWAR2_PRELIM	0x80000010	/* Access window size 128K */

/* local bus read write buffer mapping */
#define CFG_BR3_PRELIM		0xFA000801	/* map at 0xFA000000 */
#define CFG_OR3_PRELIM		0xFFFF8FF7	/* 32kB */
#define CFG_LBLAWBAR3_PRELIM	0xFA000000
#define CFG_LBLAWAR3_PRELIM	0x8000000E	/* 32KB  */

/* pass open firmware flat tree */
#define CONFIG_OF_FLAT_TREE	1
#define CONFIG_OF_BOARD_SETUP	1

/* maximum size of the flat tree (8K) */
#define OF_FLAT_TREE_MAX_SIZE	8192

#define OF_CPU			"PowerPC,8313@0"
#define OF_SOC			"soc8313@e0000000"
#define OF_TBCLK		(bd->bi_busfreq / 4)
#define OF_STDOUT_PATH		"/soc8313@e0000000/serial@4500"

/*
 * Serial Port
 */
#define CONFIG_CONS_INDEX	1
#undef CONFIG_SERIAL_SOFTWARE_FIFO
#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE	1
#define CFG_NS16550_CLK		get_bus_freq(0)

#define CFG_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

#define CFG_NS16550_COM1	(CFG_IMMR+0x4500)
#define CFG_NS16550_COM2	(CFG_IMMR+0x4600)

/* Use the HUSH parser */
#define CFG_HUSH_PARSER
#ifdef  CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2 "> "
#endif

/* I2C */
#define CONFIG_HARD_I2C			/* I2C with hardware support*/
#undef CONFIG_SOFT_I2C			/* I2C bit-banged */
#define CONFIG_FSL_I2C
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CFG_I2C_SPEED		400000	/* I2C speed and slave address */
#define CFG_I2C_SLAVE		0x7F
#define CFG_I2C_NOPROBES	{0x69}	/* Don't probe these addrs */
#define CFG_I2C_OFFSET		0x3000
#define CFG_I2C2_OFFSET		0x3100

/* TSEC */
#define CFG_TSEC1_OFFSET	0x24000
#define CFG_TSEC1		(CFG_IMMR+CFG_TSEC1_OFFSET)
#define CFG_TSEC2_OFFSET	0x25000
#define CFG_TSEC2		(CFG_IMMR+CFG_TSEC2_OFFSET)
#define CONFIG_NET_MULTI

/*
 * General PCI
 * Addresses are mapped 1-1.
 */
#define CFG_PCI1_MEM_BASE	0x80000000	
#define CFG_PCI1_MEM_PHYS	CFG_PCI1_MEM_BASE	
#define CFG_PCI1_MEM_SIZE	0x10000000	/* 256M */
#define CFG_PCI1_MMIO_BASE	0x90000000	
#define CFG_PCI1_MMIO_PHYS	CFG_PCI1_MMIO_BASE	
#define CFG_PCI1_MMIO_SIZE	0x10000000	/* 256M */
#define CFG_PCI1_IO_BASE	0x00000000		
#define CFG_PCI1_IO_PHYS	0xE2000000		
#define CFG_PCI1_IO_SIZE	0x00100000	/* 1M */	

#ifdef CONFIG_PCI

#define CONFIG_PCI_PNP		/* do pci plug-and-play */

#undef CONFIG_E1000
#undef CONFIG_EEPRO100
#undef CONFIG_TULIP

#undef CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup */

#if !defined(CONFIG_PCI_PNP)
	#define PCI_ENET0_IOADDR	0xFIXME
	#define PCI_ENET0_MEMADDR	0xFIXME
	#define PCI_IDSEL_NUMBER	0x0c	/* slot0->3(IDSEL)=12->15 */
#endif

#define CFG_PCI_SUBSYS_VENDORID 0x1057	/* Motorola */

#endif	/* CONFIG_PCI */

/*
 * TSEC configuration
 */
#define CONFIG_TSEC_ENET		/* TSEC ethernet support */

#if defined(CONFIG_TSEC_ENET)

#ifndef CONFIG_NET_MULTI
#define CONFIG_NET_MULTI	1
#endif

#define CONFIG_MII		1	/* MII PHY management */
#define CONFIG_MPC83XX_TSEC1	1
#define CONFIG_MPC83XX_TSEC1_NAME	"TSEC0"
//#define CONFIG_MPC83XX_TSEC2	1
//#define CONFIG_MPC83XX_TSEC2_NAME	"TSEC1"
#define TSEC1_PHY_ADDR		0x1c
#define TSEC2_PHY_ADDR		4
#define TSEC1_PHYIDX		0
#define TSEC2_PHYIDX		0

/* Options are: TSEC[0-1] */
#define CONFIG_ETHPRIME		"TSEC1"

#endif	/* CONFIG_TSEC_ENET */

/*
 * Configure on-board RTC
 */
#define CONFIG_RTC_DS1337
#define CFG_I2C_RTC_ADDR	0x68	/* at address 0x68	*/

/*
 * Environment
 */
#if defined(CONFIG_NAND_U_BOOT)
	#define CFG_ENV_IS_IN_NAND	1
	#define CFG_ENV_SIZE		CFG_NAND_BLOCK_SIZE
	#define CFG_ENV_OFFSET		((512<<10) - (CFG_NAND_BLOCK_SIZE<<1))
#elif !defined(CFG_RAMBOOT)
	#define CFG_ENV_IS_IN_FLASH	1
	#define CFG_ENV_ADDR		(CFG_MONITOR_BASE + 0x50000)
	#define CFG_ENV_SECT_SIZE	0x10000	/* 128K(one sector) for env */
	#define CFG_ENV_SIZE		0x2000

/* Address and size of Redundant Environment Sector */
#else
	#define CFG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
	#define CFG_ENV_ADDR		(CFG_MONITOR_BASE - 0x1000)
	#define CFG_ENV_SIZE		0x2000
#endif

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CFG_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

#define CFG_BASE_COMMANDS	( CONFIG_CMD_DFL	\
				| CFG_CMD_PING		\
				| CFG_CMD_DHCP		\
				| CFG_CMD_I2C		\
				| CFG_CMD_MII		\
				| CFG_CMD_DATE		\
				| CFG_CMD_PCI		\
				| CFG_CMD_JFFS2		\
				| CFG_CMD_NAND)

#define CFG_RAMBOOT_COMMANDS	( CFG_BASE_COMMANDS &	\
				~( CFG_CMD_ENV | CFG_CMD_LOADS ))

#if defined(CFG_RAMBOOT)
#if defined(CONFIG_PCI)
#define CONFIG_COMMANDS	(CFG_RAMBOOT_COMMANDS | CFG_CMD_PCI)
#else
#define CONFIG_COMMANDS	(CFG_RAMBOOT_COMMANDS)
#endif
#else
#if defined(CONFIG_PCI)
#define  CONFIG_COMMANDS	(CFG_BASE_COMMANDS | CFG_CMD_PCI)
#else
#define  CONFIG_COMMANDS	(CFG_BASE_COMMANDS)
#endif
#endif

#include <cmd_confdefs.h>

#undef CONFIG_WATCHDOG			/* watchdog disabled */

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP			/* undef to save memory */
#define CFG_LOAD_ADDR	0x2000000	/* default load address */
#define CFG_PROMPT	"=> "		/* Monitor Command Prompt */

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
	#define CFG_CBSIZE	1024	/* Console I/O Buffer Size */
#else
	#define CFG_CBSIZE	256	/* Console I/O Buffer Size */
#endif

#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS	16		/* max number of command args */
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_HZ		1000		/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CFG_BOOTMAPSZ	(8 << 20)	/* Initial Memory map for Linux*/

/* Cache Configuration */
#define CFG_DCACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT	5	/*log base 2 of the above value*/
#endif

#define CFG_RCWH_PCIHOST 0x80000000 /* PCIHOST  */

/* 66 MHz OSC
 Boot high 62040000 84606c00
 Boot Low  62040000 80606c00
*/
#ifdef PCI_66M 
/* 66MHz IN, 133MHz CSB, 266 DDR, 266 CORE */
#define CFG_HRCW_LOW (\
	HRCWL_LCL_BUS_TO_SCB_CLK_1X1 |\
	HRCWL_INIT |\
	HRCWL_DDR_TO_SCB_CLK_2X1 |\
	HRCWL_CSB_TO_CLKIN_2X1 |\
	HRCWL_CORE_TO_CSB_2X1)
#else
#if defined(CONFIG_133M_266M) /* 33MHz IN, 133MHz CSB, 266 DDR, 266 CORE */
#define CFG_HRCW_LOW (\
	HRCWL_LCL_BUS_TO_SCB_CLK_1X1 |\
	HRCWL_INIT |\
	HRCWL_DDR_TO_SCB_CLK_2X1 |\
	HRCWL_CSB_TO_CLKIN_4X1 |\
	HRCWL_CORE_TO_CSB_2X1)
#elif defined(CONFIG_133M_400M) /* 33MHz IN, 133MHz CSB, 266 DDR, 400 CORE */
#define CFG_HRCW_LOW (\
	HRCWL_LCL_BUS_TO_SCB_CLK_1X1 |\
	HRCWL_INIT |\
	HRCWL_DDR_TO_SCB_CLK_2X1 |\
	HRCWL_CSB_TO_CLKIN_4X1 |\
	HRCWL_CORE_TO_CSB_3X1)
#else /* 33MHz IN, 166MHz CSB, 333 DDR, 333 CORE */
#define CFG_HRCW_LOW (\
	HRCWL_LCL_BUS_TO_SCB_CLK_1X1 |\
	HRCWL_INIT |\
	HRCWL_DDR_TO_SCB_CLK_2X1 |\
	HRCWL_CSB_TO_CLKIN_5X1 |\
	HRCWL_CORE_TO_CSB_2X1)
#endif
#endif /* PCI_66M */

#ifdef CONFIG_NAND_SPL
#ifdef CONFIG_SGMII_ETSEC2
#define CFG_HRCW_HIGH (\
	HRCWH_PCI_HOST |\
	HRCWH_PCI1_ARBITER_ENABLE |\
	HRCWH_CORE_ENABLE |\
	HRCWH_FROM_0XFFF00100 |\
	HRCWH_BOOTSEQ_DISABLE |\
	HRCWH_SW_WATCHDOG_DISABLE |\
	HRCWH_ROM_LOC_NAND_SP_8BIT |\
	HRCWH_RL_EXT_NAND |\
	HRCWH_TSEC1M_IN_MII |\
	HRCWH_TSEC2M_IN_MII |\
	HRCWH_BIG_ENDIAN |\
	HRCWH_LALE_NORMAL)
#else
#define CFG_HRCW_HIGH (\
	HRCWH_PCI_HOST |\
	HRCWH_PCI1_ARBITER_ENABLE |\
	HRCWH_CORE_ENABLE |\
	HRCWH_FROM_0XFFF00100 |\
	HRCWH_BOOTSEQ_DISABLE |\
	HRCWH_SW_WATCHDOG_DISABLE |\
	HRCWH_ROM_LOC_NAND_SP_8BIT |\
	HRCWH_RL_EXT_NAND |\
	HRCWH_TSEC1M_IN_MII |\
	HRCWH_TSEC2M_IN_MII |\
	HRCWH_BIG_ENDIAN |\
	HRCWH_LALE_NORMAL)
#endif
#else /* HRCWH: 0xa0606C00 */
#ifdef CONFIG_SGMII_ETSEC2
#define CFG_HRCW_HIGH (\
	HRCWH_PCI_HOST |\
	HRCWH_PCI1_ARBITER_ENABLE |\
	HRCWH_CORE_ENABLE |\
	HRCWH_FROM_0X00000100 |\
	HRCWH_BOOTSEQ_DISABLE |\
	HRCWH_SW_WATCHDOG_DISABLE |\
	HRCWH_ROM_LOC_LOCAL_16BIT |\
	HRCWH_RL_EXT_LEGACY |\
	HRCWH_TSEC1M_IN_MII |\
	HRCWH_TSEC2M_IN_MII |\
	HRCWH_BIG_ENDIAN |\
	HRCWH_LALE_NORMAL)
#else
#define CFG_HRCW_HIGH (\
	HRCWH_PCI_HOST |\
	HRCWH_PCI1_ARBITER_ENABLE |\
	HRCWH_CORE_ENABLE |\
	HRCWH_FROM_0X00000100 |\
	HRCWH_BOOTSEQ_DISABLE |\
	HRCWH_SW_WATCHDOG_DISABLE |\
	HRCWH_ROM_LOC_LOCAL_16BIT |\
	HRCWH_RL_EXT_LEGACY |\
	HRCWH_TSEC1M_IN_MII |\
	HRCWH_TSEC2M_IN_MII |\
	HRCWH_BIG_ENDIAN |\
	HRCWH_LALE_NORMAL)
#endif
#endif
/* System IO Config */
//#define CFG_SICRH	(SICRH_TSOBI1 | SICRH_TSOBI2) /* RGMII */
//#define CFG_SICRL	SICRL_USBDR /* Enable Internal USB Phy  */

#define CFG_SICRH     0x000FFF00	/*else*/
#define CFG_SICRL     0x00000803	/* Enable GPIO30,31,Internal USB Phy */


#define CFG_HID0_INIT	0x000000000
#define CFG_HID0_FINAL	HID0_ENABLE_MACHINE_CHECK

#define CFG_HID2 HID2_HBE

/* DDR @ 0x00000000 */
#define CFG_IBAT0L	(CFG_SDRAM_BASE | BATL_PP_10 | BATL_MEMCOHERENCE)
#define CFG_IBAT0U	(CFG_SDRAM_BASE | BATU_BL_256M | BATU_VS | BATU_VP)

/* PCI @ 0x80000000 */
#ifdef CONFIG_PCI
#define CFG_IBAT1L	(CFG_PCI1_MEM_BASE | BATL_PP_10 | BATL_MEMCOHERENCE)
#define CFG_IBAT1U	(CFG_PCI1_MEM_BASE | BATU_BL_256M | BATU_VS | BATU_VP)
#define CFG_IBAT2L	(CFG_PCI1_MMIO_BASE | BATL_PP_10 | BATL_CACHEINHIBIT | BATL_GUARDEDSTORAGE)
#define CFG_IBAT2U	(CFG_PCI1_MMIO_BASE | BATU_BL_256M | BATU_VS | BATU_VP)
#else
#define CFG_IBAT1L	(0)
#define CFG_IBAT1U	(0)
#define CFG_IBAT2L	(0)
#define CFG_IBAT2U	(0)
#endif

/* PCI2 not supported on 8313 */
#define CFG_IBAT3L	(0)
#define CFG_IBAT3U	(0)
#define CFG_IBAT4L	(0)
#define CFG_IBAT4U	(0)

/* IMMRBAR @ 0xE0000000, PCI IO @ 0xE2000000 & BCSR @ 0xE2400000 */
#define CFG_IBAT5L	(CFG_IMMR | BATL_PP_10 | BATL_CACHEINHIBIT | BATL_GUARDEDSTORAGE)
#define CFG_IBAT5U	(CFG_IMMR | BATU_BL_256M | BATU_VS | BATU_VP)

/* SDRAM @ 0xF0000000, stack in DCACHE 0xFDF00000 & FLASH @ 0xFE000000 */
#define CFG_IBAT6L	(0xF0000000 | BATL_PP_10 | BATL_MEMCOHERENCE)
#define CFG_IBAT6U	(0xF0000000 | BATU_BL_256M | BATU_VS | BATU_VP)

#define CFG_IBAT7L	(0)
#define CFG_IBAT7U	(0)

#define CFG_DBAT0L	CFG_IBAT0L
#define CFG_DBAT0U	CFG_IBAT0U
#define CFG_DBAT1L	CFG_IBAT1L
#define CFG_DBAT1U	CFG_IBAT1U
#define CFG_DBAT2L	CFG_IBAT2L
#define CFG_DBAT2U	CFG_IBAT2U
#define CFG_DBAT3L	CFG_IBAT3L
#define CFG_DBAT3U	CFG_IBAT3U
#define CFG_DBAT4L	CFG_IBAT4L
#define CFG_DBAT4U	CFG_IBAT4U
#define CFG_DBAT5L	CFG_IBAT5L
#define CFG_DBAT5U	CFG_IBAT5U
#define CFG_DBAT6L	CFG_IBAT6L
#define CFG_DBAT6U	CFG_IBAT6U
#define CFG_DBAT7L	CFG_IBAT7L
#define CFG_DBAT7U	CFG_IBAT7U

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD	0x01	/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM	0x02	/* Software reboot */

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed of kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Environment Configuration
 */
#define CONFIG_ENV_OVERWRITE

#if defined(CONFIG_TSEC_ENET)
#define CONFIG_ETHADDR		00:04:9f:ef:23:33
//#define CONFIG_HAS_ETH1
//#define CONFIG_ETH1ADDR		00:E0:0C:00:7E:21
#endif

#define CONFIG_IPADDR		10.2.1.235
#define CONFIG_SERVERIP		10.2.1.115

#define CONFIG_HOSTNAME		mpc8313erdb

#define CONFIG_LOADADDR		200000	/* default location for tftp and bootm */
#define CONFIG_BOOTDELAY	10	/* -1 disables auto-boot */
#define CONFIG_BAUDRATE		115200

#define	CONFIG_EXTRA_ENV_SETTINGS						\
	"netdev=eth0\0"								\
	"gatewayip=10.2.1.1\0"							\
	"netmask=255.255.255.0\0"						\
	"ramargs=setenv bootargs root=/dev/ram rw\0"				\
	"nfsargs=setenv bootargs root=/dev/nfs rw "				\
		"nfsroot=10.2.1.19:/root/powerpc/nfs2\0"			\
	"yfsargs=setenv bootargs root=/dev/mtdblock6 rootfstype=yaffs2 rw"	\
		" console=ttyS0,115200\0"					\
	"addip=setenv bootargs ${bootargs} "					\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"		\
		":${hostname}:${netdev}:off panic=1\0"				\
	"addtty=setenv bootargs ${bootargs} console=ttyS0,${baudrate}\0"	\
	"mtdparts=mtdparts=nand0:1M(u-boot),3M(kernel),-(yaffs2)\0"		\
	""
//	"mtdids=nand0=nand0\0"
#define CONFIG_BOOTCOMMAND	"bootm fe100000 fe300000 fe700000"
#endif	/* __CONFIG_H */
