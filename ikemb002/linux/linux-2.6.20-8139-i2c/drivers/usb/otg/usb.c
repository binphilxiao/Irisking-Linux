/* Copyright (C) 2007 freescale semiconductor, Inc.
 *
 * Author: Li Yang <LeoLi@freescale.com>
 *         Jerry Huang <Chang-Ming.Huang@freescale.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/usb/otg.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>


static struct otg_transceiver *transceiver = 0;
static struct resource *otg_resources = 0;

/*
 * otg_get_transceiver - find the (single) OTG transceiver driver
 *
 * Returns the transceiver driver, after getting a refcount to it; or
 * null if there is no such transceiver.  The caller is responsible for
 * releasing that count.
 */
struct otg_transceiver *otg_get_transceiver(void)
{
    return transceiver;
}

int otg_set_transceiver(struct otg_transceiver *otg)
{
	if (transceiver && otg)
		return -EBUSY;

	 transceiver = otg;

	return 0;
}

struct resource *otg_get_resources(void)
{
	return otg_resources;
}

int otg_set_resources(struct resource *resources)
{
	otg_resources = resources;
	return 0;
}

EXPORT_SYMBOL(otg_get_transceiver);
EXPORT_SYMBOL(otg_set_transceiver);
EXPORT_SYMBOL(otg_get_resources);
EXPORT_SYMBOL(otg_set_resources);
