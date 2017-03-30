/*
 * This file was copied from drivers/nand/nand_ecc.c and optimized to reduce
 * the memory size and function calls to correct up to 1 bit error in each
 * 256 byte block of data.
 *
 * Copyright (C) 2006, Freescale Semiconductor
 *
 * Copyright (C) 2000-2004 Steven J. Hill (sjhill@realitydiluted.com)
 *                         Toshiba America Electronics Components, Inc.
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 or (at your option) any
 * later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this file; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * As a special exception, if other files instantiate templates or use
 * macros or inline functions from these files, or you compile these
 * files and link them with other works to produce a work based on these
 * files, these files do not by themselves cause the resulting work to be
 * covered by the GNU General Public License. However the source code for
 * these files must still be made available in accordance with section (3)
 * of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 */

#include <common.h>

/*
 * Pre-calculated 256-way 1 byte column parity
 */
static const u_char nand_ecc_precalc_table[] = {
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59,
	    0x03, 0x56, 0x55, 0x00,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c,
	    0x66, 0x33, 0x30, 0x65,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f,
	    0x65, 0x30, 0x33, 0x66,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a,
	    0x00, 0x55, 0x56, 0x03,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30,
	    0x6a, 0x3f, 0x3c, 0x69,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55,
	    0x0f, 0x5a, 0x59, 0x0c,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56,
	    0x0c, 0x59, 0x5a, 0x0f,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33,
	    0x69, 0x3c, 0x3f, 0x6a,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33,
	    0x69, 0x3c, 0x3f, 0x6a,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56,
	    0x0c, 0x59, 0x5a, 0x0f,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55,
	    0x0f, 0x5a, 0x59, 0x0c,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30,
	    0x6a, 0x3f, 0x3c, 0x69,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a,
	    0x00, 0x55, 0x56, 0x03,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f,
	    0x65, 0x30, 0x33, 0x66,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c,
	    0x66, 0x33, 0x30, 0x65,
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59,
	    0x03, 0x56, 0x55, 0x00
};

/**
 * nand_correct_data - [NAND Interface] Detect and correct bit error(s)
 * @dat:	raw data read from the chip
 * @ecc_pos:	ecc byte offsets (3 bytes per 512 data byte block)
 * @blocks:	Number of 512 byte blocks to be processed
 *
 * Detect and / or correct 1 bit error per 256 byte block
 */
int nand_correct_data (u_char * dat, const u_char * ecc_pos, int blocks)
{
	u_char tmp1, tmp2;
	u_char a, b, c, d1, d2, d3, add, bit, i;
	u_char idx, reg1, reg2, reg3;
	int j, ctr, ret;
	u_char *ecc;

	ecc = dat + CFG_NAND_PAGE_SIZE;
	ctr = 0;
	while (blocks--) {

		/* Initialize variables */
		ret = reg1 = reg2 = reg3 = 0;

		/* Build up column parity */
		for (j = 0; j < 256; j++) {

			/* Get CP0 - CP5 from table */
			idx = nand_ecc_precalc_table[dat[j]];
			reg1 ^= (idx & 0x3f);

			/* All bit XOR = 1 ? */
			if (idx & 0x40) {
				reg3 ^= (u_char) j;
				reg2 ^= ~((u_char) j);
			}
		}

		/* Create non-inverted ECC code from line parity */

		/* Initialize variables */
		a = b = c = 0x80;
		tmp1 = tmp2 = 0;

		/* Calculate first ECC byte */
		for (i = 0; i < 4; i++) {
			if (reg3 & a)	/* LP15,13,11,9 --> ecc_code[0] */
				tmp1 |= b;
			b >>= 1;
			if (reg2 & a)	/* LP14,12,10,8 --> ecc_code[0] */
				tmp1 |= b;
			b >>= 1;
			a >>= 1;
		}

		/* Calculate second ECC byte */
		for (i = 0; i < 4; i++) {
			if (reg3 & a)	/* LP7,5,3,1 --> ecc_code[1] */
				tmp2 |= c;
			c >>= 1;
			if (reg2 & a)	/* LP6,4,2,0 --> ecc_code[1] */
				tmp2 |= c;
			c >>= 1;
			a >>= 1;
		}

		/* Calculate final ECC code */
		/* Do error detection */
		d1 = (~tmp1) ^ ecc[*(ecc_pos++)];
		d2 = (~tmp2) ^ ecc[*(ecc_pos++)];
		d3 = (((~reg1) << 2) | 0x03) ^ ecc[*(ecc_pos++)];

		if ((d1 | d2 | d3) != 0) {
			/* 1 or more errors detected */
			a = (d1 ^ (d1 >> 1)) & 0x55;
			b = (d2 ^ (d2 >> 1)) & 0x55;
			c = (d3 ^ (d3 >> 1)) & 0x54;

			/* Found and correct single bit error in the data */
			if ((a == 0x55) && (b == 0x55) && (c == 0x54)) {
				a = b = c = 0x80;
				add = 0;
				for (i = 0; i < 4; i++) {
					if (d1 & b)
						add |= a;
					b >>= 2;
					a >>= 1;
				}
				for (i = 0; i < 4; i++) {
					if (d2 & c)
						add |= a;
					c >>= 2;
					a >>= 1;
				}
				bit = 0;
				b = 0x04;
				c = 0x80;
				for (i = 0; i < 3; i++) {
					if (d3 & c)
						bit |= b;
					c >>= 2;
					b >>= 1;
				}
				b = 0x01;
				a = dat[add];
				a ^= (b << bit);
				dat[add] = a;
				ret = 1;
			} else {
				while (d1) {
					if (d1 & 0x01)
						ret++;
					d1 >>= 1;
				}
				while (d2) {
					if (d2 & 0x01)
						ret++;
					d2 >>= 1;
				}
				while (d3) {
					if (d3 & 0x01)
						ret++;
					d3 >>= 1;
				}
			}
		}

		/* this page had more than 1 error so it is uncorrectable */
		if (ret > 1)
			return -1;

		/* advance to the next page */
		dat += 256;
		ctr += ret;
	}

	/* return number of ECC errors that we corrected */
	return ctr;
}
