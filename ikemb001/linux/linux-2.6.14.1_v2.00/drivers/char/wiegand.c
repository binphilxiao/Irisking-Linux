/*
2008-10-09 Version 1 with interrupt
2009-02-25 Version 2
2009-03-16 Version 3
*/

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/hardware.h>
#include <asm/semaphore.h>
#include <asm/arch/regs-gpio.h>

int wiegand_major = 0;
int wiegand_minor = 0;
//module_param(wiegand_major, int, S_IRUGO);

u32 irq_data = 0;     //data of wig

//mem for interface with user space
u32 *buf_in;

static struct wiegand_t
{
	struct cdev cdev;
	dev_t devno;
	void __iomem *wig_mem;
	struct semaphore sem;
}*wiegand_dev;

u32 read_wiegand(struct wiegand_t *dev)
{
	u32 retval = 0;
	u32 value =0;
	int i;

	if (down_interruptible(&dev->sem))
		{
		printk("S3C2410_read down_interruptible failure!\n");
		return -ERESTARTSYS;
		}

	for(i=0;i<4;i++)
		{
		buf_in[i] = ioread8(dev->wig_mem + 4*(i+1));
		mb();
		retval = (retval << 8) + buf_in[i];
		}

	value = retval;

	up(&dev->sem);
	return retval;
}

irqreturn_t wiegand_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	s3c2410_gpio_setpin(S3C2410_GPE5, 0);
	irq_data = read_wiegand(wiegand_dev);
	s3c2410_gpio_setpin(S3C2410_GPE5, 1);
	printk("The Wiegand ID is %x\n",irq_data);
	return IRQ_HANDLED;
}

static int Arm9_Wiegand_open(struct inode *inode, struct file *filp)
{
	filp->private_data = wiegand_dev;	
	s3c2410_gpio_setpin(S3C2410_GPE5, 1);
	return 0;
}

static int Arm9_Wiegand_release(struct inode *inode, struct file *filp)
{
	filp->private_data = NULL;
	s3c2410_gpio_setpin(S3C2410_GPE5, 1);
	return 0;
}
/*
static int Arm9_Wiegand_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;

	s3c2410_gpio_setpin(S3C2410_GPE5, 1);

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (cmd)
	{
	case 1:   //output
		{
		s3c2410_gpio_cfgpin(S3C2410_GPH4,S3C2410_GPH4_OUTP);
		s3c2410_gpio_pullup(S3C2410_GPH4,1);
		break;
		}
	case 2:   //TXD1
		{
		s3c2410_gpio_cfgpin(S3C2410_GPH4,S3C2410_GPH4_TXD1);
		break;
		}
	case 3:   //high
		{
		s3c2410_gpio_setpin(S3C2410_GPH4, 1);
		break;
		}
	case 4:   //low
		{
		s3c2410_gpio_setpin(S3C2410_GPH4, 0);
		break;
		}
	default:
    		up(&dev->sem);
	return -ENOTTY;
	}
	up(&dev->sem);
	return 0;
}

static loff_t Arm9_Wiegand_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;

	s3c2410_gpio_setpin(S3C2410_GPE5, 1);

	switch(whence)
	{
	case 0: //SEEK_SET
		newpos = off;//*(u32 *)&n_off;
		break;

	case 1: // SEEK_CUR
		newpos = filp->f_pos + off;
		break;

	//case 2: // SEEK_END
		//newpos = dev->size + off;
		//break;

	default: // can't happen
		return -EINVAL;
	}
	//printk(KERN_WARNING"%s:1: filp=%p, sizeof(loff_t)=%d, newpos=%d, filp->f_pos=%d\n", __FUNCTION__, filp, sizeof(loff_t), (int)newpos, (int)filp->f_pos);
	if (newpos < 0)
		{
		//printk(KERN_WARNING"newpos < 0");
		return -EINVAL;
		}
	filp->f_pos = newpos;
	//printk(KERN_WARNING"%s:2: filp=%p, sizeof(loff_t)=%d, newpos=%d, filp->f_pos=%d\n", __FUNCTION__, filp, sizeof(loff_t), newpos, filp->f_pos);
	return newpos;
}
*/

static ssize_t Arm9_Wiegand_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;
/*
	u32 data;
	s3c2410_gpio_setpin(S3C2410_GPE5, 0);
	data = irq_data;
	s3c2410_gpio_setpin(S3C2410_GPE5, 1);
	if((copy_to_user(buf, &data, count)))
		return -EFAULT;
*/
	return count;
}

static ssize_t Arm9_Wiegand_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;
/*
	ssize_t retval = 0;
	u32 dat = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if ((copy_from_user(&dat, buf, count)))
		{
		retval = -EFAULT;
		goto out;
		}
	iowrite32(dat, dev->base.map);
	mb();

	retval = 1;
out:
	up(&dev->sem);
*/
	return count;
}

static struct file_operations wiegand_fops = {
	.owner		=	THIS_MODULE,
	.open		=	Arm9_Wiegand_open,
	.release	=	Arm9_Wiegand_release,
	//.ioctl		=	Arm9_Wiegand_ioctl,
	//.llseek		=	Arm9_Wiegand_llseek,
	.read		=	Arm9_Wiegand_read,
	.write		=	Arm9_Wiegand_write,
};

static int Wiegand_remove(struct device *ndev)
{
	iounmap(wiegand_dev->wig_mem);
	kfree(buf_in);
	kfree(wiegand_dev);
	free_irq(IRQ_EINT11, ndev);

	cdev_del(&wiegand_dev->cdev);
	unregister_chrdev_region(wiegand_dev->devno, 1);

	printk(KERN_INFO"S3C2410 wiegand unloaded\n");
	return 0;
}

static int Wiegand_probe(struct device *ndev)
{
	int err, result = 0;
	s3c2410_gpio_setpin(S3C2410_GPE5, 1);

//在内核空间中动态申请内存
	if((wiegand_dev = kmalloc(sizeof(struct wiegand_t), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "wiegand_t: out of memory\n");
		return -ENOMEM;
		}
	memset(wiegand_dev, 0, sizeof(*wiegand_dev));

	//可以自己设定需要几个字节的空间，当前是4字节
	if((buf_in = kmalloc(sizeof(u32), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "buffer: out of memory\n");
		return -ENOMEM;
		}
	memset(buf_in, 0, sizeof(*buf_in));

//内存空间映射：连接到CPLD的数据地址和总线
	wiegand_dev->wig_mem = ioremap(0x18000000,32);

//分配设备编号
	//static
	if (wiegand_major)
		{
		wiegand_dev->devno = MKDEV(wiegand_major, wiegand_minor);
		result = register_chrdev_region(wiegand_dev->devno, 1, "wiegand");
		}
	//dynamic
	else
		{
		result = alloc_chrdev_region(&wiegand_dev->devno, wiegand_minor, 1, "wiegand");
		wiegand_major = MAJOR(wiegand_dev->devno);
		}
	if (result < 0)
		{
		printk(KERN_WARNING"wiegand: can't get major %d\n", wiegand_major);
		return result;
		}

//注册设备
	//devno = MKDEV(wiegand_major, wiegand_minor);
	cdev_init(&wiegand_dev->cdev, &wiegand_fops);
	wiegand_dev->cdev.owner = THIS_MODULE;
	wiegand_dev->cdev.ops = &wiegand_fops;
	err = cdev_add(&wiegand_dev->cdev, &wiegand_dev->devno, 1);
	if (err)
		printk(KERN_NOTICE"Error %d adding wiegand%d", err, 0);

//初始化信号向量
	init_MUTEX(&wiegand_dev->sem);

//申请中断
	result = request_irq(IRQ_EINT11, wiegand_interrupt, SA_INTERRUPT,"s3c2410-wig-int", ndev);
	if (result)
		{
		printk(KERN_ERR "IRQ%d already in use\n", IRQ_EINT11);
		goto err;
		}
	printk(KERN_INFO"S3C2410 wiegand with interrupt initialized\n");
	return 0;

err:
	free_irq(IRQ_EINT11, ndev);
	Wiegand_remove(ndev);
	return result;
}

static struct device_driver Wiegand_driver = {
	.name = "s3c2410-wig",
	.bus = &platform_bus_type,
	.probe = Wiegand_probe,
	.remove = Wiegand_remove,
};

static int __init Wiegand_init(void) 
{
	return driver_register(&Wiegand_driver);
}

static void __exit Wiegand_exit(void)
{
	driver_unregister(&Wiegand_driver);
}

module_init(Wiegand_init);
module_exit(Wiegand_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Wiegand driver, Copyright(C) IrisKing");
