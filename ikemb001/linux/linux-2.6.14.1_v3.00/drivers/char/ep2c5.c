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

#define GCS3_BASE 0x18000000
#define CONFIG_REG 0
#define STATUS_REG 13

#define WIEGAND_IN_BYTE0_REG 6
#define WIEGAND_IN_BYTE1_REG 7
#define WIEGAND_IN_BYTE2_REG 8
#define WIEGAND_IN_BYTE3_REG 9

#define WIEGAND_OUT_BYTE0_REG 1
#define WIEGAND_OUT_BYTE1_REG 2
#define WIEGAND_OUT_BYTE2_REG 3
#define WIEGAND_OUT_BYTE3_REG 4

#define WIEGAND_IN_IRQ_MASK 0x1
#define WIEGAND_OUT_IRQ_MASK 0x10

#define WIEGAND_OUT_EN_MASK 0x08

int ep2c5_major = 0;
int ep2c5_minor = 0;

#define ep2c5_name "ep2c5"
#define driver_name "s3c2410-ep2c5"

u32 irq_data = 0;     //data of wig

//mem for interface with user space
u32 *buf_in;

static struct ep2c5_t
{
	struct cdev cdev;
	dev_t devno;
	void __iomem *mem;
	struct semaphore sem;
}*ep2c5_dev;

static void Check_ID(void)
{
	u8 id;
	id=ioread8(ep2c5_dev->mem+15);
	if (id==0x74)
		printk("ep2c5 device ID:%02x\n",id);
	else 
		printk("ep2c5 device ID err\n");
}

static void wiegand_in_interrupt(void)
{
}

static void wiegand_out_interrupt(void)
{
	u8 status,config;
    status= ioread8(ep2c5_dev->mem+STATUS_REG);
    status &= (~WIEGAND_OUT_IRQ_MASK);
    iowrite8(status, ep2c5_dev->mem+STATUS_REG);
    mb();

    config= ioread8(ep2c5_dev->mem+CONFIG_REG);
    config &= (~WIEGAND_OUT_EN_MASK);
    iowrite8(config, ep2c5_dev->mem+CONFIG_REG);
    mb();
	
    printk("wiegand output complete\n");
}

irqreturn_t ep2c5_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	u8 status=0;
	status= ioread8(ep2c5_dev->mem+STATUS_REG);
    	mb();
	
    	if (down_interruptible(&ep2c5_dev->sem))
		return IRQ_HANDLED;

   	 if(status & WIEGAND_IN_IRQ_MASK)                    //韦根输入1
        	wiegand_in_interrupt();
    	else if(status & WIEGAND_OUT_IRQ_MASK)           //韦根输出使能 & 韦根输出中断
        	wiegand_out_interrupt();
		
    	up(&ep2c5_dev->sem);
	
    	return IRQ_HANDLED;
}

static int Arm9_ep2c5_open(struct inode *inode, struct file *filp)
{
	Check_ID();
	return 0;
}

static int Arm9_ep2c5_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int Arm9_ep2c5_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	struct ep2c5_t *dev = (struct ep2c5_t *)filp->private_data;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	printk("iotcl\n");

	switch (cmd)
	{
	case 0x10:   //G5V_IO
		{
			printk("G5V_IO 0\n");
			iowrite8(0<<4, ep2c5_dev->mem+5);
			mb();
			break;
		}
	case 0x11:   //G5V_IO
		{
			printk("G5V_IO 1\n");
			iowrite8(1<<4, ep2c5_dev->mem+5);
			mb();
			break;
		}
	case 0x20:   //ep2c5_IO
		{
			printk("ep2c5_IO 0\n");
			iowrite8(0<<5, ep2c5_dev->mem+5);
			mb();
			break;
		}
	case 0x21:   //ep2c5_IO
		{
			printk("ep2c5_IO 1\n");
			iowrite8(1<<5, ep2c5_dev->mem+5);
			mb();
			break;
		}
	case 0x30:   //IO_SHAN
		{
			printk("IO_SHAN 0\n");
			s3c2410_gpio_cfgpin(S3C2410_GPC1, S3C2410_GPC1_OUTP);
			s3c2410_gpio_setpin(S3C2410_GPC1, 0);
			break;
		}
	case 0x31:   //IO_SHAN
		{
			printk("IO_SHAN 1\n");
			s3c2410_gpio_cfgpin(S3C2410_GPC1, S3C2410_GPC1_OUTP);
			s3c2410_gpio_setpin(S3C2410_GPC1, 1);
			break;
		}
	case 0x40:   //FM_RST
		{
			printk("FM_RST\n");
			s3c2410_gpio_cfgpin(S3C2410_GPG4, S3C2410_GPG4_OUTP);
			s3c2410_gpio_setpin(S3C2410_GPG4, 1);
			mdelay(10);
			s3c2410_gpio_setpin(S3C2410_GPG4, 0);
			mdelay(10);
			s3c2410_gpio_setpin(S3C2410_GPG4, 1);
			break;
		}	
	default:
    		up(&dev->sem);
	return -ENOTTY;
	}
	up(&dev->sem);
	return 0;
}


static ssize_t Arm9_ep2c5_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	printk("ep2c5 read\n");
	return count;
}

static ssize_t Arm9_ep2c5_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	printk("ep2c5 write\n");
	copy_from_user(buf_in, buf, 4);
	iowrite8(*(buf_in+0), ep2c5_dev->mem+1);
	mb();
	iowrite8(*(buf_in+1), ep2c5_dev->mem+2);
	mb();
	iowrite8(*(buf_in+2), ep2c5_dev->mem+3);
	mb();
	iowrite8(*(buf_in+3), ep2c5_dev->mem+4);
	mb();
	iowrite8(0x10, ep2c5_dev->mem+13);
	mb();		
	
	return count;
}

static struct file_operations ep2c5_fops = {
	.owner		=	THIS_MODULE,
	.open		=	Arm9_ep2c5_open,
	.release	       =	Arm9_ep2c5_release,
	.ioctl		       =	Arm9_ep2c5_ioctl,
	.read		=	Arm9_ep2c5_read,
	.write		=	Arm9_ep2c5_write,
};



static void ep2c5_dev_destroy(struct device *ndev)
{
	iounmap(ep2c5_dev->mem);
	free_irq(IRQ_EINT1, ndev);
	cdev_del(&ep2c5_dev->cdev);
	unregister_chrdev_region(ep2c5_dev->devno, 1);
	kfree(ep2c5_dev);

	printk(KERN_INFO"S3C2410 ep2c5 unloaded\n");
}

static int ep2c5_remove(struct device *ndev)
{
	ep2c5_dev_destroy(ndev);
	return 0;
}

static int ep2c5_dev_init(struct device *ndev)
{
	int result = 0;	
	dev_t dev;

	if((ep2c5_dev = kmalloc(sizeof(struct ep2c5_t), GFP_KERNEL)) == NULL)//在内核空间中动态申请内存
		return -ENOMEM;
	memset(ep2c5_dev, 0, sizeof(*ep2c5_dev));
	ep2c5_dev->devno=dev;
	ep2c5_dev->mem = ioremap(GCS3_BASE,32);//内存空间映射：连接到CPLD的数据地址和总线

	if (ep2c5_major)//申请设备号
	{
		dev = MKDEV(ep2c5_major, ep2c5_minor);
		result = register_chrdev_region(dev, 1, ep2c5_name);
	}
	else
	{
		result = alloc_chrdev_region(&dev, 0, 1, ep2c5_name);
		ep2c5_major = MAJOR(dev);
		ep2c5_minor = MINOR(dev);
	}

	if (result < 0)
	{
		printk(KERN_WARNING"%s: can't get major %d\n", __FUNCTION__, ep2c5_major);
		return result;
	}

	cdev_init(&ep2c5_dev->cdev, &ep2c5_fops);//注册设备
	ep2c5_dev->cdev.owner = THIS_MODULE;
	ep2c5_dev->cdev.ops = &ep2c5_fops;

	result  = cdev_add(&ep2c5_dev->cdev, ep2c5_dev->devno, 1);
	if (result )
		printk(KERN_NOTICE"Error %d adding ep2c5%d", result, 0);

	init_MUTEX(&ep2c5_dev->sem);//初始化信号向量

	result = request_irq(IRQ_EINT1, ep2c5_interrupt, SA_INTERRUPT,"s3c2410-ep2c5-int", ndev);//申请中断
	if (result)
		{
		printk(KERN_ERR "IRQ%d already in use\n", IRQ_EINT1);
		goto err;
		}
	//printk(KERN_INFO"S3C2410 ep2c5 with interrupt initialized\n");
	return 0;

err:
	free_irq(IRQ_EINT1, ndev);
	ep2c5_remove(ndev);
	return result;
}

static int ep2c5_probe(struct device *ndev)
{
	int result;
	
	if (strcmp(ndev->bus_id, driver_name) != 0)
		return -1;

	if ((result = ep2c5_dev_init(ndev)) != 0)
		ep2c5_remove(ndev);

	printk("ep2c5 initialized\n");
	
	return result;
}

static struct device_driver ep2c5_driver = {
	.name = driver_name,
	.bus = &platform_bus_type,
	.probe = ep2c5_probe,
	.remove = ep2c5_remove,
};

static int __init ep2c5_init(void) 
{
	return driver_register(&ep2c5_driver);
}

static void __exit ep2c5_exit(void)
{
	driver_unregister(&ep2c5_driver);
}

module_init(ep2c5_init);
module_exit(ep2c5_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("ep2c5 driver, Copyright(C) IrisKing");
