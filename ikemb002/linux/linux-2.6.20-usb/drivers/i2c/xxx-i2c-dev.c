/*
    i2c-dev.c - i2c-bus driver, char device interface

    Copyright (C) 1995-97 Simon G. Vogl
    Copyright (C) 1998-99 Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2003 Greg Kroah-Hartman <greg@kroah.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Note that this is a complete rewrite of Simon Vogl's i2c-dev module.
   But I have used so much of his original code and ideas that it seems
   only fair to recognize him as co-author -- Frodo */

/* The I2C_RDWR ioctl code is written by Kolja Waschk <waschk@telos.de> */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <asm/uaccess.h>

#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/platform_device.h>

#include <asm/io.h>
#include <linux/fsl_devices.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/delay.h>


#define MPC_I2C_ADDR  0x00
#define MPC_I2C_FDR 	0x04
#define MPC_I2C_CR	0x08
#define MPC_I2C_SR	0x0c
#define MPC_I2C_DR	0x10
#define MPC_I2C_DFSRR 0x14
#define MPC_I2C_REGION 0x20

#define CCR_MEN  0x80
#define CCR_MIEN 0x40
#define CCR_MSTA 0x20
#define CCR_MTX  0x10
#define CCR_TXAK 0x08
#define CCR_RSTA 0x04

#define CSR_MCF  0x80
#define CSR_MAAS 0x40
#define CSR_MBB  0x20
#define CSR_MAL  0x10
#define CSR_SRW  0x04
#define CSR_MIF  0x02
#define CSR_RXAK 0x01

struct mpc_i2c {
	void __iomem *base;
	u32 interrupt;
	wait_queue_head_t queue;
	struct i2c_adapter adap;
	int irq;
	u32 flags;
};

static struct i2c_driver i2cdev_driver;

struct i2c_dev {
	struct list_head list;
	struct i2c_adapter *adap;
	struct device *dev;
};

#define I2C_MINORS	256
static LIST_HEAD(i2c_dev_list);
static DEFINE_SPINLOCK(i2c_dev_list_lock);

static struct i2c_dev *i2c_dev_get_by_minor(unsigned index)
{
	struct i2c_dev *i2c_dev;

	spin_lock(&i2c_dev_list_lock);
	list_for_each_entry(i2c_dev, &i2c_dev_list, list) {
		if (i2c_dev->adap->nr == index)
			goto found;
	}
	i2c_dev = NULL;
found:
	spin_unlock(&i2c_dev_list_lock);
	return i2c_dev;
}

static struct i2c_dev *get_free_i2c_dev(struct i2c_adapter *adap)
{
	struct i2c_dev *i2c_dev;

	if (adap->nr >= I2C_MINORS) {
		printk(KERN_ERR "i2c-dev: Out of device minors (%d)\n",
		       adap->nr);
		return ERR_PTR(-ENODEV);
	}

	i2c_dev = kzalloc(sizeof(*i2c_dev), GFP_KERNEL);
	if (!i2c_dev)
		return ERR_PTR(-ENOMEM);
	i2c_dev->adap = adap;

	spin_lock(&i2c_dev_list_lock);
	list_add_tail(&i2c_dev->list, &i2c_dev_list);
	spin_unlock(&i2c_dev_list_lock);
	return i2c_dev;
}

static void return_i2c_dev(struct i2c_dev *i2c_dev)
{
	spin_lock(&i2c_dev_list_lock);
	list_del(&i2c_dev->list);
	spin_unlock(&i2c_dev_list_lock);
	kfree(i2c_dev);
}

static ssize_t show_adapter_name(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct i2c_dev *i2c_dev = i2c_dev_get_by_minor(MINOR(dev->devt));

	if (!i2c_dev)
		return -ENODEV;
	return sprintf(buf, "%s\n", i2c_dev->adap->name);
}
static DEVICE_ATTR(name, S_IRUGO, show_adapter_name, NULL);

static ssize_t i2cdev_read (struct file *file, char __user *buf, size_t count,
                            loff_t *offset)
{
	char *tmp;
	int ret;

	struct i2c_client *client = (struct i2c_client *)file->private_data;

	if (count > 8192)
		count = 8192;

	tmp = kmalloc(count,GFP_KERNEL);
	if (tmp==NULL)
		return -ENOMEM;

	pr_debug("i2c-dev: i2c-%d reading %zd bytes.\n",
		iminor(file->f_path.dentry->d_inode), count);

	ret = i2c_master_recv(client,tmp,count);
	if (ret >= 0)
		ret = copy_to_user(buf,tmp,count)?-EFAULT:ret;
	kfree(tmp);
	return ret;
}

static __inline__ void writeccr(struct mpc_i2c *i2c, u32 x)
{
	writeb(x, i2c->base + MPC_I2C_CR);
}

static irqreturn_t mpc_i2c_isr(int irq, void *dev_id)
{
	struct mpc_i2c *i2c = dev_id;
	if (readb(i2c->base + MPC_I2C_SR) & CSR_MIF) {
		/* Read again to allow register to stabilise */
		i2c->interrupt = readb(i2c->base + MPC_I2C_SR);
		writeb(0, i2c->base + MPC_I2C_SR);
		wake_up_interruptible(&i2c->queue);
	}
	return IRQ_HANDLED;
}

static int i2c_wait(struct mpc_i2c *i2c, unsigned timeout, int writing)
{
	unsigned long orig_jiffies = jiffies;
	u32 x;
	int result = 0;

	if (i2c->irq == 0)
	{
		while (!(readb(i2c->base + MPC_I2C_SR) & CSR_MIF)) {
			schedule();
			if (time_after(jiffies, orig_jiffies + timeout)) {
				pr_debug("I2C: timeout\n");
				result = -EIO;
				break;
			}
		}
		x = readb(i2c->base + MPC_I2C_SR);
		writeb(0, i2c->base + MPC_I2C_SR);
	} else {
		/* Interrupt mode */
		result = wait_event_interruptible_timeout(i2c->queue,
			(i2c->interrupt & CSR_MIF), timeout * HZ);

		if (unlikely(result < 0))
			pr_debug("I2C: wait interrupted\n");
		else if (unlikely(!(i2c->interrupt & CSR_MIF))) {
			pr_debug("I2C: wait timeout\n");
			result = -ETIMEDOUT;
		}

		x = i2c->interrupt;
		i2c->interrupt = 0;
	}

	if (result < 0)
		return result;

	if (!(x & CSR_MCF)) {
		pr_debug("I2C: unfinished\n");
		return -EIO;
	}

	if (x & CSR_MAL) {
		pr_debug("I2C: MAL\n");
		return -EIO;
	}

	if (writing && (x & CSR_RXAK)) {
		pr_debug("I2C: No RXAK\n");
		/* generate stop */
		writeccr(i2c, CCR_MEN);
		return -EIO;
	}
	return 0;
}

static void mpc_i2c_setclock(struct mpc_i2c *i2c)
{
	/* Set clock and filters */
	if (i2c->flags & FSL_I2C_DEV_SEPARATE_DFSRR) {
		writeb(0x31, i2c->base + MPC_I2C_FDR);
		writeb(0x10, i2c->base + MPC_I2C_DFSRR);
	} else if (i2c->flags & FSL_I2C_DEV_CLOCK_5200)
		writeb(0x3f, i2c->base + MPC_I2C_FDR);
	else
		writel(0x1031, i2c->base + MPC_I2C_FDR);
}

static void mpc_i2c_start(struct mpc_i2c *i2c)
{
	/* Clear arbitration */
	writeb(0, i2c->base + MPC_I2C_SR);
	/* Start with MEN */
	writeccr(i2c, CCR_MEN);
}

static void mpc_i2c_stop(struct mpc_i2c *i2c)
{
	writeccr(i2c, CCR_MEN);
}

static int mpc_write(struct mpc_i2c *i2c, int target,
		     const u8 * data, int length, int restart)
{
	int i;
	unsigned timeout = i2c->adap.timeout;
	u32 flags = restart ? CCR_RSTA : 0;

	/* Start with MEN */
	if (!restart)
		writeccr(i2c, CCR_MEN);
	/* Start as master */
	writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_MSTA | CCR_MTX | flags);
	/* Write target byte */
	writeb((target << 1), i2c->base + MPC_I2C_DR);

	if (i2c_wait(i2c, timeout, 1) < 0)
		return -1;

	for (i = 0; i < length; i++) {
		/* Write data byte */
		writeb(data[i], i2c->base + MPC_I2C_DR);

		if (i2c_wait(i2c, timeout, 1) < 0)
			return -1;
	}

	return 0;
}

static int mpc_read(struct mpc_i2c *i2c, int target,
		    u8 * data, int length, int restart)
{
	unsigned timeout = i2c->adap.timeout;
	int i;
	u32 flags = restart ? CCR_RSTA : 0;

	/* Start with MEN */
	if (!restart)
		writeccr(i2c, CCR_MEN);
	/* Switch to read - restart */
	writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_MSTA | CCR_MTX | flags);
	/* Write target address byte - this time with the read flag set */
	writeb((target << 1) | 1, i2c->base + MPC_I2C_DR);

	if (i2c_wait(i2c, timeout, 1) < 0)
		return -1;

	if (length) {
		if (length == 1)
			writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_MSTA | CCR_TXAK);
		else
			writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_MSTA);
		/* Dummy read */
		readb(i2c->base + MPC_I2C_DR);
	}

	for (i = 0; i < length; i++) {
		if (i2c_wait(i2c, timeout, 0) < 0)
			return -1;

		/* Generate txack on next to last byte */
		if (i == length - 2)
			writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_MSTA | CCR_TXAK);
		/* Generate stop on last byte */
		if (i == length - 1)
			writeccr(i2c, CCR_MIEN | CCR_MEN | CCR_TXAK);
		data[i] = readb(i2c->base + MPC_I2C_DR);
	}

	return length;
}

static int mpc_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	struct i2c_msg *pmsg;
	int i;
	int ret = 0;
	unsigned long orig_jiffies = jiffies;
	struct mpc_i2c *i2c = i2c_get_adapdata(adap);

	mpc_i2c_start(i2c);

	/* Allow bus up to 1s to become not busy */
	while (readb(i2c->base + MPC_I2C_SR) & CSR_MBB) {
		if (signal_pending(current)) {
			pr_debug("I2C: Interrupted\n");
			return -EINTR;
		}
		if (time_after(jiffies, orig_jiffies + HZ)) {
			pr_debug("I2C: timeout\n");
			return -EIO;
		}
		schedule();
	}

	for (i = 0; ret >= 0 && i < num; i++) {
		pmsg = &msgs[i];
		pr_debug("Doing %s %d bytes to 0x%02x - %d of %d messages\n",
			 pmsg->flags & I2C_M_RD ? "read" : "write",
			 pmsg->len, pmsg->addr, i + 1, num);
		if (pmsg->flags & I2C_M_RD)
			ret =
			    mpc_read(i2c, pmsg->addr, pmsg->buf, pmsg->len, i);
		else
			ret =
			    mpc_write(i2c, pmsg->addr, pmsg->buf, pmsg->len, i);
	}
	mpc_i2c_stop(i2c);
	return (ret < 0) ? ret : num;
}

static ssize_t i2cdev_write (struct file *file, const char __user *buf, size_t count,
                             loff_t *offset)
{
int i;
	int ret;
	char *tmp;
	struct i2c_client *client = (struct i2c_client *)file->private_data;

	struct i2c_adapter *adap=client->adapter;
	struct i2c_msg msg;

	if (count > 8192)
		count = 8192;

	tmp = kmalloc(count,GFP_KERNEL);
	if (tmp==NULL)
		return -ENOMEM;
	if (copy_from_user(tmp,buf,count)) {
		kfree(tmp);
		return -EFAULT;
	}

	pr_debug("i2c-dev: i2c-%d writing %zd bytes.\n",
		iminor(file->f_path.dentry->d_inode), count);

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = (char *)tmp;

	ret = mpc_xfer(&adap, &msg, 1);

	//ret = i2c_master_send(client,tmp,count);
	kfree(tmp);
	return ret;
}

static int i2cdev_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client *)file->private_data;
	struct i2c_rdwr_ioctl_data rdwr_arg;
	struct i2c_smbus_ioctl_data data_arg;
	union i2c_smbus_data temp;
	struct i2c_msg *rdwr_pa;
	u8 __user **data_ptrs;
	int i,datasize,res;
	unsigned long funcs;

	dev_dbg(&client->adapter->dev, "ioctl, cmd=0x%02x, arg=0x%02lx\n",
		cmd, arg);

	switch ( cmd ) {
	case I2C_SLAVE:
	case I2C_SLAVE_FORCE:
		if ((arg > 0x3ff) ||
		    (((client->flags & I2C_M_TEN) == 0) && arg > 0x7f))
			return -EINVAL;
		if ((cmd == I2C_SLAVE) && i2c_check_addr(client->adapter,arg))
			return -EBUSY;
		client->addr = arg;
		return 0;
	case I2C_TENBIT:
		if (arg)
			client->flags |= I2C_M_TEN;
		else
			client->flags &= ~I2C_M_TEN;
		return 0;
	case I2C_PEC:
		if (arg)
			client->flags |= I2C_CLIENT_PEC;
		else
			client->flags &= ~I2C_CLIENT_PEC;
		return 0;
	case I2C_FUNCS:
		funcs = i2c_get_functionality(client->adapter);
		return put_user(funcs, (unsigned long __user *)arg);

	case I2C_RDWR:
		if (copy_from_user(&rdwr_arg,
				   (struct i2c_rdwr_ioctl_data __user *)arg,
				   sizeof(rdwr_arg)))
			return -EFAULT;

		/* Put an arbitrary limit on the number of messages that can
		 * be sent at once */
		if (rdwr_arg.nmsgs > I2C_RDRW_IOCTL_MAX_MSGS)
			return -EINVAL;

		rdwr_pa = (struct i2c_msg *)
			kmalloc(rdwr_arg.nmsgs * sizeof(struct i2c_msg),
			GFP_KERNEL);

		if (rdwr_pa == NULL) return -ENOMEM;

		if (copy_from_user(rdwr_pa, rdwr_arg.msgs,
				   rdwr_arg.nmsgs * sizeof(struct i2c_msg))) {
			kfree(rdwr_pa);
			return -EFAULT;
		}

		data_ptrs = kmalloc(rdwr_arg.nmsgs * sizeof(u8 __user *), GFP_KERNEL);
		if (data_ptrs == NULL) {
			kfree(rdwr_pa);
			return -ENOMEM;
		}

		res = 0;
		for( i=0; i<rdwr_arg.nmsgs; i++ ) {
			/* Limit the size of the message to a sane amount */
			if (rdwr_pa[i].len > 8192) {
				res = -EINVAL;
				break;
			}
			data_ptrs[i] = (u8 __user *)rdwr_pa[i].buf;
			rdwr_pa[i].buf = kmalloc(rdwr_pa[i].len, GFP_KERNEL);
			if(rdwr_pa[i].buf == NULL) {
				res = -ENOMEM;
				break;
			}
			if(copy_from_user(rdwr_pa[i].buf,
				data_ptrs[i],
				rdwr_pa[i].len)) {
					++i; /* Needs to be kfreed too */
					res = -EFAULT;
				break;
			}
		}
		if (res < 0) {
			int j;
			for (j = 0; j < i; ++j)
				kfree(rdwr_pa[j].buf);
			kfree(data_ptrs);
			kfree(rdwr_pa);
			return res;
		}

		res = i2c_transfer(client->adapter,
			rdwr_pa,
			rdwr_arg.nmsgs);
		while(i-- > 0) {
			if( res>=0 && (rdwr_pa[i].flags & I2C_M_RD)) {
				if(copy_to_user(
					data_ptrs[i],
					rdwr_pa[i].buf,
					rdwr_pa[i].len)) {
					res = -EFAULT;
				}
			}
			kfree(rdwr_pa[i].buf);
		}
		kfree(data_ptrs);
		kfree(rdwr_pa);
		return res;

	case I2C_SMBUS:
		if (copy_from_user(&data_arg,
		                   (struct i2c_smbus_ioctl_data __user *) arg,
		                   sizeof(struct i2c_smbus_ioctl_data)))
			return -EFAULT;
		if ((data_arg.size != I2C_SMBUS_BYTE) &&
		    (data_arg.size != I2C_SMBUS_QUICK) &&
		    (data_arg.size != I2C_SMBUS_BYTE_DATA) &&
		    (data_arg.size != I2C_SMBUS_WORD_DATA) &&
		    (data_arg.size != I2C_SMBUS_PROC_CALL) &&
		    (data_arg.size != I2C_SMBUS_BLOCK_DATA) &&
		    (data_arg.size != I2C_SMBUS_I2C_BLOCK_DATA) &&
		    (data_arg.size != I2C_SMBUS_BLOCK_PROC_CALL)) {
			dev_dbg(&client->adapter->dev,
				"size out of range (%x) in ioctl I2C_SMBUS.\n",
				data_arg.size);
			return -EINVAL;
		}
		/* Note that I2C_SMBUS_READ and I2C_SMBUS_WRITE are 0 and 1,
		   so the check is valid if size==I2C_SMBUS_QUICK too. */
		if ((data_arg.read_write != I2C_SMBUS_READ) &&
		    (data_arg.read_write != I2C_SMBUS_WRITE)) {
			dev_dbg(&client->adapter->dev,
				"read_write out of range (%x) in ioctl I2C_SMBUS.\n",
				data_arg.read_write);
			return -EINVAL;
		}

		/* Note that command values are always valid! */

		if ((data_arg.size == I2C_SMBUS_QUICK) ||
		    ((data_arg.size == I2C_SMBUS_BYTE) &&
		    (data_arg.read_write == I2C_SMBUS_WRITE)))
			/* These are special: we do not use data */
			return i2c_smbus_xfer(client->adapter, client->addr,
					      client->flags,
					      data_arg.read_write,
					      data_arg.command,
					      data_arg.size, NULL);

		if (data_arg.data == NULL) {
			dev_dbg(&client->adapter->dev,
				"data is NULL pointer in ioctl I2C_SMBUS.\n");
			return -EINVAL;
		}

		if ((data_arg.size == I2C_SMBUS_BYTE_DATA) ||
		    (data_arg.size == I2C_SMBUS_BYTE))
			datasize = sizeof(data_arg.data->byte);
		else if ((data_arg.size == I2C_SMBUS_WORD_DATA) ||
		         (data_arg.size == I2C_SMBUS_PROC_CALL))
			datasize = sizeof(data_arg.data->word);
		else /* size == smbus block, i2c block, or block proc. call */
			datasize = sizeof(data_arg.data->block);

		if ((data_arg.size == I2C_SMBUS_PROC_CALL) ||
		    (data_arg.size == I2C_SMBUS_BLOCK_PROC_CALL) ||
		    (data_arg.read_write == I2C_SMBUS_WRITE)) {
			if (copy_from_user(&temp, data_arg.data, datasize))
				return -EFAULT;
		}
		res = i2c_smbus_xfer(client->adapter,client->addr,client->flags,
		      data_arg.read_write,
		      data_arg.command,data_arg.size,&temp);
		if (! res && ((data_arg.size == I2C_SMBUS_PROC_CALL) ||
		              (data_arg.size == I2C_SMBUS_BLOCK_PROC_CALL) ||
			      (data_arg.read_write == I2C_SMBUS_READ))) {
			if (copy_to_user(data_arg.data, &temp, datasize))
				return -EFAULT;
		}
		return res;

	default:
		return i2c_control(client,cmd,arg);
	}
	return 0;
}

static int i2cdev_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct i2c_client *client;
	struct i2c_adapter *adap;
	struct i2c_dev *i2c_dev;

	i2c_dev = i2c_dev_get_by_minor(minor);
	if (!i2c_dev)
		return -ENODEV;

	adap = i2c_get_adapter(i2c_dev->adap->nr);
	if (!adap)
		return -ENODEV;

	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		i2c_put_adapter(adap);
		return -ENOMEM;
	}
	snprintf(client->name, I2C_NAME_SIZE, "i2c-dev %d", adap->nr);
	client->driver = &i2cdev_driver;

	/* registered with adapter, passed as client to user */
	client->adapter = adap;
	file->private_data = client;

	return 0;
}

static int i2cdev_release(struct inode *inode, struct file *file)
{
	struct i2c_client *client = file->private_data;

	i2c_put_adapter(client->adapter);
	kfree(client);
	file->private_data = NULL;

	return 0;
}

static struct file_operations i2cdev_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= i2cdev_read,
	.write		= i2cdev_write,
	.ioctl		= i2cdev_ioctl,
	.open		= i2cdev_open,
	.release	= i2cdev_release,
};

static struct class *i2c_dev_class;

static int i2cdev_attach_adapter(struct i2c_adapter *adap)
{
	struct i2c_dev *i2c_dev;
	int res;

	i2c_dev = get_free_i2c_dev(adap);
	if (IS_ERR(i2c_dev))
		return PTR_ERR(i2c_dev);

	/* register this i2c device with the driver core */
	i2c_dev->dev = device_create(i2c_dev_class, &adap->dev,
				     MKDEV(I2C_MAJOR, adap->nr),
				     "i2c-%d", adap->nr);
	if (IS_ERR(i2c_dev->dev)) {
		res = PTR_ERR(i2c_dev->dev);
		goto error;
	}
	res = device_create_file(i2c_dev->dev, &dev_attr_name);
	if (res)
		goto error_destroy;

	pr_debug("i2c-dev: adapter [%s] registered as minor %d\n",
		 adap->name, adap->nr);
	return 0;
error_destroy:
	device_destroy(i2c_dev_class, MKDEV(I2C_MAJOR, adap->nr));
error:
	return_i2c_dev(i2c_dev);
	return res;
}

static int i2cdev_detach_adapter(struct i2c_adapter *adap)
{
	struct i2c_dev *i2c_dev;

	i2c_dev = i2c_dev_get_by_minor(adap->nr);
	if (!i2c_dev) /* attach_adapter must have failed */
		return 0;

	device_remove_file(i2c_dev->dev, &dev_attr_name);
	return_i2c_dev(i2c_dev);
	device_destroy(i2c_dev_class, MKDEV(I2C_MAJOR, adap->nr));

	pr_debug("i2c-dev: adapter [%s] unregistered\n", adap->name);
	return 0;
}

static int i2cdev_detach_client(struct i2c_client *client)
{
	return 0;
}

static struct i2c_driver i2cdev_driver = {
	.driver = {
		.name	= "dev_driver",
	},
	.id		= I2C_DRIVERID_I2CDEV,
	.attach_adapter	= i2cdev_attach_adapter,
	.detach_adapter	= i2cdev_detach_adapter,
	.detach_client	= i2cdev_detach_client,
};

static int __init i2c_dev_init(void)
{
	int res;

	printk(KERN_INFO "i2c /dev entries driver\n");

	res = register_chrdev(I2C_MAJOR, "i2c", &i2cdev_fops);
	if (res)
		goto out;

	i2c_dev_class = class_create(THIS_MODULE, "i2c-dev");
	if (IS_ERR(i2c_dev_class))
		goto out_unreg_chrdev;

	res = i2c_add_driver(&i2cdev_driver);
	if (res)
		goto out_unreg_class;

	return 0;

out_unreg_class:
	class_destroy(i2c_dev_class);
out_unreg_chrdev:
	unregister_chrdev(I2C_MAJOR, "i2c");
out:
	printk(KERN_ERR "%s: Driver Initialisation failed\n", __FILE__);
	return res;
}

static void __exit i2c_dev_exit(void)
{
	i2c_del_driver(&i2cdev_driver);
	class_destroy(i2c_dev_class);
	unregister_chrdev(I2C_MAJOR,"i2c");
}

MODULE_AUTHOR("Frodo Looijaard <frodol@dds.nl> and "
		"Simon G. Vogl <simon@tk.uni-linz.ac.at>");
MODULE_DESCRIPTION("I2C /dev entries driver");
MODULE_LICENSE("GPL");

module_init(i2c_dev_init);
module_exit(i2c_dev_exit);
