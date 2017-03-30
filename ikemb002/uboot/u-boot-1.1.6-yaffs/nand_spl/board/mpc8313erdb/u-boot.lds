/*
 * (C) Copyright 2007, Freescale Semiconductor Inc.
 *
 * (C) Copyright 2006
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
 */

OUTPUT_ARCH(powerpc:common)
SECTIONS
{
  .text      :
  {
    start.o	(.text)

    *(.text)
    *(.fixup)
  }
  _etext = .;

  .reloc    :
  {
    _GOT_TABLE_ = .;
    *(.got*)
    __got_end = .;
    *(.got2)
  }
  __got_entries = (__got_end - _GOT_TABLE_) >> 2;

  .data    :
  {
    *(.rodata*)
    *(.data*)
    *(.sdata*)
  }

  _edata  =  .;

  __bss_start = .;
  .bss       :
  {
   *(.sbss)
   *(.bss)
  }

  _end = . ;
}
