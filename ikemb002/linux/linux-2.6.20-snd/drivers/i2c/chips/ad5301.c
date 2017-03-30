/*
 * drivers/i2c/chips/ad5301.c
 *
 * I2C client/driver for the Analog Devices AD5301 Digital to analog Converter
 *
 * Author: Yashpal Dutta <yashpal.dutta@freescale.com>
 * Copyright (C) 2007 Freescale Semiconductor, Inc.
 *
 * Based on the m41t00.c by Mark Greer <mgreer@mvista.com>
 *
 * 2005 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
/*
 * This i2c client/driver wedges between the drivers/char/genrtc.c RTC
 * interface and the SMBus interface of the i2c subsystem.
 * It would be more efficient to use i2c msgs/i2c_transfer directly but, as
 * recommened in .../Documentation/i2c/writing-clients section
 * "Sending and receiving", using SMBus level communication is preferred.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

#define        AD5301_DRV_NAME         "ad5301"

static DEFINE_MUTEX(ad5301_mutex);

static struct i2c_driver ad5301_driver;
static struct i2c_client *save_client;

static unsigned short ignore[] = { I2C_CLIENT_END };
static unsigned short normal_addr[] = { 0x0c, I2C_CLIENT_END };

static struct i2c_client_address_data addr_data = {
	.normal_i2c = normal_addr,
	.probe = ignore,
	.ignore = ignore,
};

static struct workqueue_struct *ad5301_workqueue;

char ad5301_set_voltage(char voltage)
{
	char upper_nibble, lower_nibble;
	upper_nibble = 0x0f & (voltage >> 4);
	lower_nibble = 0xf0 & (voltage << 4);
	if(save_client != NULL)
	{
		i2c_smbus_write_byte_data(save_client, upper_nibble, lower_nibble);
		return 0;
	}
	else
	{
		return -1;
	}
}

char ad5301_get_voltage(void)
{
	if(save_client != NULL)
		return i2c_smbus_read_byte(save_client);
	else
	{
		return -1;
	}
}

static int ad5301_probe(struct i2c_adapter *adap, int addr, int kind)
{
	struct i2c_client *client;
	int rc;

	client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (!client)
		return -ENOMEM;

	strncpy(client->name, AD5301_DRV_NAME, I2C_NAME_SIZE);
	client->addr = addr;
	client->adapter = adap;
	client->driver = &ad5301_driver;

	ad5301_workqueue = create_singlethread_workqueue("ad5301");

	if ((rc = i2c_attach_client(client)) != 0) {
		kfree(client);
		return rc;
	}
	ad5301_set_voltage(0x8f);
	save_client = client;
	return 0;
}

static int ad5301_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, ad5301_probe);
}

static int ad5301_detach(struct i2c_client *client)
{
	int rc;

	if ((rc = i2c_detach_client(client)) == 0) {
		kfree(i2c_get_clientdata(client));
		destroy_workqueue(ad5301_workqueue);
	}
	return rc;
}

static struct i2c_driver ad5301_driver = {
	.driver = {
		.name   = AD5301_DRV_NAME,
	},
	.id = I2C_DRIVERID_AD5301,
	.attach_adapter = ad5301_attach,
	.detach_client = ad5301_detach,
};

static int __init ad5301_init(void)
{
	return i2c_add_driver(&ad5301_driver);
}

static void __exit ad5301_exit(void)
{
	i2c_del_driver(&ad5301_driver);
}

module_init(ad5301_init);
module_exit(ad5301_exit);
MODULE_AUTHOR("Yashpal Dutta <yashpal.dutta@freescale.com>");
MODULE_DESCRIPTION("Maxim/Dallas AD5301 DAC I2C Client Driver");
MODULE_LICENSE("GPL");
