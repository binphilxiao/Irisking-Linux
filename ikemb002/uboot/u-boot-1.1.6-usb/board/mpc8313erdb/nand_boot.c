/*
 * Copyright 2007 Freescale Semiconductor, Inc.
 * 
 * Origin from MPC8313EMDS of Nick.Spence@freescale.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <mpc83xx.h>
#include <ns16550.h>
#include <nand.h>
#include <asm/processor.h>

/* NAND ECC checking method - 0 = no hardware ECC check */
#define NAND_HARD_ECC ((CFG_NAND_BR0_PRELIM >> BR_DECC_SHIFT) & 3)

/* NAND Page Size : 0 = small page (512 bytes ), 1 = large page (2048 bytes) */
#define NAND_PGS ((CFG_NAND_OR0_PRELIM >> OR_FCM_PGS_SHIFT) & 1)

/* Timeout in case FCM does not complete */
#define NAND_TIMEOUT	 (1000000)

/* Delay before restarting after a fatal u-boot error */
#define RESTART_DELAY	 (0x4000000)

/* Error codes returned from nand_read_next_block()                */
#define NAND_OK          (1)	/* read block okay                 */
#define NAND_BAD_BLOCK   (0)	/* block marked bad - skip block   */
#define NAND_ERR_TIMEOUT (-1)	/* timeout error - fatal error     */
#define NAND_ERR_ECC     (-2)	/* uncorrectable ecc - fatal error */

/* Macros to control selected serial port */
#if CONFIG_CONS_INDEX == 1 && defined(CFG_NS16550_COM1)
#define NS16550_COM ((NS16550_t)CFG_NS16550_COM1)
#elif CONFIG_CONS_INDEX == 2 && defined(CFG_NS16550_COM2)
#define NS16550_COM ((NS16550_t)CFG_NS16550_COM2)
#else
#warning  "*****************************"
#warning  "** No console port defined **"
#warning  "*****************************"
#define NS16550_COM ((NS16550_t)0)
#define CFG_NAND_BOOT_QUIET
#endif /* CONFIG_CONS_INDEX */

/* Quiet Boot - only prints fatal error messages */
#if defined(CFG_NAND_BOOT_QUIET)
#define status_putc(c) { while (0); }
#define status_puts(s) { while (0); }
#else
#define status_putc(c) { putc(c); }
#define status_puts(s) { puts(s); }
#endif /* CFG_NAND_BOOT_QUIET */

#if !(NAND_HARD_ECC)
const u_char ecc_pos[] = {
#if (NAND_PGS)
	40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55,
	56, 57, 58, 59, 60, 61, 62, 63
#else
	0, 1, 2, 3, 6, 7
#endif /* NAND_PGS */
};
#endif /* !(NAND_HARD_ECC) */

/* u-boot version string from start.S */
extern char version_string[];

/* nand_ecc.c */
extern int nand_correct_data (u_char * dat, const u_char * ecc_pos, int blocks);

/* reset board */
static
void reset (void)
{
	int ctr;
	ulong msr;
#ifndef MPC83xx_RESET
	ulong addr;
#endif

	volatile immap_t *immap = (immap_t *) CFG_IMMR;

	/* add delay before reseting */
	ctr = RESTART_DELAY;
	while (ctr--) ;

#ifdef MPC83xx_RESET
	/* Interrupts and MMU off */
	__asm__ __volatile__ ("mfmsr    %0":"=r" (msr):);

	msr &= ~( MSR_EE | MSR_IR | MSR_DR);
	__asm__ __volatile__ ("mtmsr    %0"::"r" (msr));

	/* enable Reset Control Reg */
	immap->reset.rpr = 0x52535445;
	__asm__ __volatile__ ("sync");
	__asm__ __volatile__ ("isync");

	/* confirm Reset Control Reg is enabled */
	while(!((immap->reset.rcer) & RCER_CRE));

	/* perform reset, only one bit */
	immap->reset.rcr = RCR_SWHR;

#else   /* ! MPC83xx_RESET */

	immap->reset.rmr = RMR_CSRE;    /* Checkstop Reset enable */

	/* Interrupts and MMU off */
	__asm__ __volatile__ ("mfmsr    %0":"=r" (msr):);

	msr &= ~(MSR_ME | MSR_EE | MSR_IR | MSR_DR);
	__asm__ __volatile__ ("mtmsr    %0"::"r" (msr));

	/*
	 * Trying to execute the next instruction at a non-existing address
	 * should cause a machine check, resulting in reset
	 */
	addr = CFG_RESET_ADDRESS;
	((void (*)(void)) addr) ();
#endif  /* MPC83xx_RESET */
}

#define LCRVAL LCR_8N1				/* 8 data, 1 stop, no parity */
#define MCRVAL (MCR_DTR | MCR_RTS)		/* RTS/DTR */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR) /* Clear & enable FIFOs */

static
void NS16550a_init (int baud_divisor)
{
	if (NS16550_COM) {
		NS16550_COM->ier = 0x00;
		NS16550_COM->lcr = LCR_BKSE | LCRVAL;
		NS16550_COM->dll = baud_divisor & 0xff;
		NS16550_COM->dlm = (baud_divisor >> 8) & 0xff;
		NS16550_COM->lcr = LCRVAL;
		NS16550_COM->mcr = MCRVAL;
		NS16550_COM->fcr = FCRVAL;
	}
}

/* print a single character, with an extra line feed for return characters */
void putc (const char c)
{
	if (NS16550_COM) {
		if (c == '\n') {
			while ((NS16550_COM->lsr & LSR_THRE) == 0);
			NS16550_COM->thr = '\r';
		}
		while ((NS16550_COM->lsr & LSR_THRE) == 0);
		NS16550_COM->thr = c;
	}
}

/* print an entire null terminated string */
void puts (const char *s)
{
	while (*s) {
		putc (*s++);
	}
}

/* read the next block from NAND flash and store it at the supplied address
 *
 * return values:
 *  NAND_OK          - block was successfully read and copied to the destination
 *  NAND_BAD_BLOCK   - block was marked bad so should be skipped
 *  NAND_ERR_TIMEOUT - page read did not complete (fatal error)
 *  NAND_ERR_ECC     - uncorrectable ECC (fatal error)
 */
static
int nand_read_next_block (unsigned int *dst)
{
	volatile immap_t *im = (immap_t *) CFG_IMMR;
	volatile lbus83xx_t *lbc = &im->lbus;
	int buf_num;
	int page;
	unsigned char *srcc;
	unsigned int *src;
	int ecc_err;
	int ctr;
	unsigned int status;
#if !(NAND_HARD_ECC)
	int ecc_cnt;
	char ecc_char;

	ecc_cnt = 0;
#endif /* !(NAND_HARD_ECC) */

	ecc_err = 0;
	srcc = 0;

	/* Enable FCM detection of timeouts, ECC errors and completion */
	lbc->ltedr = 0;

	lbc->fbcr = 0;		/* read entire page to enable ECC */
	lbc->fbar++;		/* read next block, follows boot loaded block */
#if (NAND_PGS)
	lbc->fir = (FIR_OP_CW0 << FIR_OP0_SHIFT) |
	    (FIR_OP_CA << FIR_OP1_SHIFT) |
	    (FIR_OP_PA << FIR_OP2_SHIFT) |
	    (FIR_OP_CW1 << FIR_OP3_SHIFT) |
	    (FIR_OP_RBW << FIR_OP4_SHIFT);
#else
	lbc->fir = (FIR_OP_CW0 << FIR_OP0_SHIFT) |
	    (FIR_OP_CA << FIR_OP1_SHIFT) |
	    (FIR_OP_PA << FIR_OP2_SHIFT) |
	    (FIR_OP_RBW << FIR_OP3_SHIFT);
#endif /* NAND_PGS */
	lbc->fcr = (NAND_CMD_READ0 << FCR_CMD0_SHIFT) |
	    (NAND_CMD_READSTART << FCR_CMD1_SHIFT);

	/* read in each page of the block */
	for (page = 0; page < (CFG_NAND_BLOCK_SIZE / CFG_NAND_PAGE_SIZE);
	     page++) {
		if (NAND_PGS) {
			lbc->fpar = ((page << FPAR_LP_PI_SHIFT) & FPAR_LP_PI);
			buf_num = (page & 1) << 2;
		} else {
			lbc->fpar = ((page << FPAR_SP_PI_SHIFT) & FPAR_SP_PI);
			buf_num = page & 7;
		}
		lbc->fmr = CFG_NAND_FMR | 2;

		/* clear event registers */
		lbc->ltesr = lbc->ltesr;
		lbc->lteatr = 0;

		/* execute special operation on bank 0 */
		lbc->lsor = 0;
		asm ("sync");

		/* copy previous page to RAM */
		if (srcc) {
#if !(NAND_HARD_ECC)
			status =
			    nand_correct_data (srcc, ecc_pos,
					       sizeof (ecc_pos) / 3);
			ecc_cnt += status;
			if (status < 0)
				ecc_err = 1;
#endif /* !(NAND_HARD_ECC) */
			src = (unsigned int *)srcc;
			for (ctr = CFG_NAND_PAGE_SIZE / sizeof (unsigned int);
			     ctr; ctr--) {
				*(dst++) = *(src++);
			}
		}

		/* store the source address for the next page */
		srcc = (unsigned char *)((CFG_NAND_BR0_PRELIM & BR_BA) +
					 (buf_num * 1024));

		/* wait for FCM complete flag or timeout */
		status = 0;
		for (ctr = NAND_TIMEOUT; ctr; ctr--) {
			status = lbc->ltesr;
			if (status) {
				break;
			}
		}

		/* check for timeout or hardware ECC errors */
		if (status != LTESR_CC) {
#if (NAND_HARD_ECC)
			if (status & LTESR_PAR) {
				ecc_err = 1;
			} else
#endif /* NAND_HARD_ECC */
			{
				status_putc ('T');
				return NAND_ERR_TIMEOUT;
			}
		}

		/* Check if the block is marked as bad */
		if (page < 2) {
			if (srcc[CFG_NAND_PAGE_SIZE + CFG_NAND_BAD_BLOCK_POS] !=
			    0xFF) {
				status_putc ('B');
				return NAND_BAD_BLOCK;
			}
		}
	}

	/* copy last page to RAM */
#if !(NAND_HARD_ECC)
	status = nand_correct_data (srcc, ecc_pos, sizeof (ecc_pos) / 3);
	ecc_cnt += status;
	if (status < 0)
		ecc_err = 1;
#endif /* !(NAND_HARD_ECC) */
	src = (unsigned int *)srcc;
	for (ctr = CFG_NAND_PAGE_SIZE / sizeof (unsigned int); ctr; ctr--) {
		*(dst++) = *(src++);
	}

	/* abort if any of the pages had uncorrectable errors */
	if (ecc_err && (page > 1)) {
		status_putc ('U');
		return NAND_ERR_ECC;
	}
#if (NAND_HARD_ECC)
	status_putc ('.');
#else
#ifdef CFG_NAND_BOOT_SHOW_ECC_NONE
	ecc_char = '.';
#else
	if (ecc_cnt <= 0) {
		ecc_char = '.';
#ifdef CFG_NAND_BOOT_SHOW_ECC_NUM
	} else if (ecc_cnt <= 9) {
		ecc_char = '0' + ecc_cnt;
	} else {
		ecc_char = 'a' + ecc_cnt - 10;
#else
	} else {
		ecc_char = 'c';
#endif /* CFG_NAND_BOOT_SHOW_ECC_NUM */
	}
#endif /* CFG_NAND_BOOT_SHOW_ECC_NONE */
	status_putc (ecc_char);
#endif /* NAND_HARD_ECC */

	return NAND_OK;		/* block read completed ok */
}

/* initial C code called from start.S prior to relocating code to DDR
 *
 * This performs minimal CPU initailization, DDR initialization, a few
 * print statements and the calls relocate_code() to copy the code from
 * the NAND flash buffer to DDR.
 */
void cpu_init_f (volatile immap_t * im)
{
	u8 spmf;
	u8 clkin_div;
	u32 csb_clk;

	/* RMR - Reset Mode Register - enable checkstop reset enable */
	im->reset.rmr = (RMR_CSRE & (1 << RMR_CSRE_SHIFT));

	/* LCRR - Clock Ratio Register - set up local bus timing */
	im->lbus.lcrr = CFG_LCRR;

#if defined(CFG_NAND_BR0_PRELIM)  \
	&& defined(CFG_NAND_OR0_PRELIM) \
	&& defined(CFG_NAND_LBLAWBAR0_PRELIM) \
	&& defined(CFG_NAND_LBLAWAR0_PRELIM)
	im->lbus.bank[0].br = CFG_NAND_BR0_PRELIM;
	im->lbus.bank[0].or = CFG_NAND_OR0_PRELIM;
	im->sysconf.lblaw[0].bar = CFG_NAND_LBLAWBAR0_PRELIM;
	im->sysconf.lblaw[0].ar = CFG_NAND_LBLAWAR0_PRELIM;
#else
#error  CFG_NAND_BR0_PRELIM, CFG_NAND_OR0_PRELIM, CFG_NAND_LBLAWBAR0_PRELIM & CFG_NAND_LBLAWAR0_PRELIM must be defined
#endif
	clkin_div = ((im->clk.spmr & SPMR_CKID) >> SPMR_CKID_SHIFT);
	spmf = ((im->reset.rcwl & RCWL_SPMF) >> RCWL_SPMF_SHIFT);

	if (im->reset.rcwh & HRCWH_PCI_HOST) {
#if defined(CONFIG_83XX_CLKIN)
		csb_clk = CONFIG_83XX_CLKIN * spmf;
#else
		csb_clk = 0;
#endif /* CONFIG_83XX_CLKIN */
	} else {
#if defined(CONFIG_83XX_PCICLK)
		csb_clk = CONFIG_83XX_PCICLK * spmf * (1 + clkin_div);
#else
		csb_clk = 0;
#endif /* CONFIG_83XX_PCICLK */
	}

	/* initialize selected port with appropriate baud rate */
	NS16550a_init (csb_clk / 16 / CONFIG_BAUDRATE);

	status_puts ("\nNAND SPL - ");
	status_puts ((char *)(&version_string));

	/* board specific DDR initialization */
	initdram (0);

	/* copy code to DDR and jump to it - this should not return */
	/* NOTE - code has to be copied out of NAND buffer before
	 * other blocks can be read.
	 */
	relocate_code (CFG_NAND_RELOC + 0x10000, 0, CFG_NAND_RELOC);

	/* should never get here */
	puts ("\nRelocate failed\n");

	/* delay then restart */
	reset ();
}

/* called after code is moved to DDR, to complete boot loading */
void board_init_r (gd_t * id, ulong dest_addr)
{
	int blockcopy_count;
	unsigned char *dst;
	void (*uboot) (void* dummy, void* immr);
	int ret;

	icache_enable ();	/* faster execution */

	status_puts ("\nLoading from NAND : ");

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	dst = (unsigned char *)CFG_NAND_U_BOOT_DST;
	blockcopy_count = ((CFG_NAND_U_BOOT_SIZE + CFG_NAND_BLOCK_SIZE - 1)
			   / CFG_NAND_BLOCK_SIZE);

	while (blockcopy_count) {
		ret = nand_read_next_block ((unsigned int *)dst);
		switch (ret) {
		case NAND_OK:
			/* advance to the next block */
			dst += CFG_NAND_BLOCK_SIZE;
			blockcopy_count--;
			break;
		case NAND_BAD_BLOCK:
			/* skip bad block */
			break;
		default:	/* fatal error */
#if defined(CFG_NAND_BOOT_QUIET)
			puts ("\nNAND SPL - ");
#else
			putc ('\n');
#endif /* CFG_NAND_BOOT_QUIET */
			if (ret == NAND_ERR_TIMEOUT)
				puts ("**FATAL** : NAND Flash operation timeout\n");
			else
				puts ("**FATAL** : uncorrectable ECC Error\n");

			/* delay then restart */
			reset ();
			break;
		}
	}
	
	/*
	 * Jump to U-Boot image
	 */
	uboot = (void (*)(void* dummy, void* immr))CFG_NAND_U_BOOT_START;
	(*uboot) (NULL, (void*) CFG_IMMR);
}


