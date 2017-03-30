/*
 * drivers/net/phy/vitesse_7385.c
 *
 * Dummy Ethernet switch driver for MPC8313ERDB platform
 *
 * Author:  Tony Li <Tony.Li@freescale.com>
 *      Based on the MPC8349-mITX switch code. All the substantial init
 *      operations has been done in u-boot. This code just creats a dummy
 *      phy device to deal with kernel stock and do not touch hardware.
 *
 * Copyright (C) 2006 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

MODULE_DESCRIPTION("Dummy Ethernet switch driver for MPC8313ERDB platform");
MODULE_AUTHOR("Tony Li");
MODULE_LICENSE("GPL");

static int vitesse_7385_config_init(struct phy_device *phydev)
{
	return 0;
}

static int vitesse_7385_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int vitesse_7385_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int vitesse_7385_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static int vitesse_7385_read_status(struct phy_device *phydev)
{
	/* Switch is always assumed to be up */
	phydev->speed = SPEED_1000;
	phydev->duplex = DUPLEX_FULL;
	phydev->pause = phydev->asym_pause = 0;
//	phydev->link = 1;
//	phydev->state = PHY_RUNNING;
//	netif_carrier_on(phydev->attached_dev);

	return 0;
}



/* Dummy Ethernet switch driver for MPC8313ERDB platform */
static struct phy_driver vitesse_7385_driver = {
	.phy_id		= MPC8313ERDB_SWITCHID,
	.name		= "MPC8313ERDB Ethernet Switch",
	.phy_id_mask	= 0x0000FFFF,
	.features	= PHY_GBIT_FEATURES,
	.flags		= 0,
	.config_init	= &vitesse_7385_config_init,
	.config_aneg	= &vitesse_7385_config_aneg,
	.read_status	= &vitesse_7385_read_status,
	.ack_interrupt	= &vitesse_7385_ack_interrupt,
	.config_intr	= &vitesse_7385_config_intr,
	.driver 	= { .owner = THIS_MODULE,},
};

static int __init vitesse_7385_init(void)
{
	return phy_driver_register(&vitesse_7385_driver);
}

static void __exit vitesse_7385_exit(void)
{
	phy_driver_unregister(&vitesse_7385_driver);
}

module_init(vitesse_7385_init);
module_exit(vitesse_7385_exit);
