#include <linux/delay.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/wait.h>
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

#define BUSY 1
#define READY 0

int wiegand_major = 0;
int wiegand_minor = 0;

#define wiegand_name "wiegand"
#define driver_name "s3c2410-wig"

u32 irq_data = 0;     //data of wig

static struct wiegand_t
{
	struct cdev cdev;
	dev_t devno;
	void __iomem *mem;
	u8* buf_wiegend_out;
	wait_queue_head_t que_out;
	struct semaphore sem_out;
	int out_stat_flag;
	u8* buf_wiegend_in;
	wait_queue_head_t que_in;
	struct semaphore sem_in;
	int in_stat_flag;
}*wiegand_dev;



static int Check_ID(void)
{
	u8 id;
	id=ioread8(wiegand_dev->mem+15);
	if (id==0x74)
		{
		printk("wiegand device ID:%02x\n",id);
		return 0;
		}
	else 
		{
		printk("wiegand device ID err\n");
		return -1;
		}
}

static void wiegand_SentOneSeq(u8 *dat)
{
	u8 tmp;
	wiegand_dev->out_stat_flag=BUSY;

	iowrite8(*(dat), wiegand_dev->mem+WIEGAND_OUT_BYTE0_REG);
	mb();
	iowrite8(*(dat), wiegand_dev->mem+WIEGAND_OUT_BYTE1_REG);
	mb();
	iowrite8(*(dat), wiegand_dev->mem+WIEGAND_OUT_BYTE2_REG);
	mb();
	iowrite8(*(dat), wiegand_dev->mem+WIEGAND_OUT_BYTE3_REG);
	
	mb();
	tmp=ioread8(wiegand_dev->mem+CONFIG_REG);
	tmp &=~(WIEGAND_OUT_EN_MASK);
	tmp |= WIEGAND_OUT_EN_MASK;
	iowrite8(tmp, wiegand_dev->mem+CONFIG_REG);
	mb();	
	
	wait_event_interruptible(wiegand_dev->que_out, wiegand_dev->out_stat_flag==READY);
}


static void wiegand_GetOneSeq(u8 *dat)
{
	*(dat)=ioread8(wiegand_dev->mem+WIEGAND_IN_BYTE0_REG);
	mb();
	*(dat+1)=ioread8(wiegand_dev->mem+WIEGAND_IN_BYTE1_REG);
	mb();
	*(dat+2)=ioread8(wiegand_dev->mem+WIEGAND_IN_BYTE2_REG);
	mb();
	*(dat+3)=ioread8(wiegand_dev->mem+WIEGAND_IN_BYTE3_REG);
	mb();
}

static void wiegand_in_interrupt(void)
{
	wiegand_dev->in_stat_flag==READY;
	wake_up_interruptible(&wiegand_dev->que_in);
}

static void wiegand_out_interrupt(void)
{
	u8 status,config;
	
    status= ioread8(wiegand_dev->mem+STATUS_REG);
    status &= (~WIEGAND_OUT_IRQ_MASK);
    iowrite8(status, wiegand_dev->mem+STATUS_REG);
    mb();

    config= ioread8(wiegand_dev->mem+CONFIG_REG);
    config &= (~WIEGAND_OUT_EN_MASK);
    iowrite8(config, wiegand_dev->mem+CONFIG_REG);
    mb();
	
    wiegand_dev->out_stat_flag=READY;	
	wake_up_interruptible(&wiegand_dev->que_out);
	
}

irqreturn_t wiegand_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	u8 status=0;
	
	status= ioread8(wiegand_dev->mem+STATUS_REG);
    	mb();

   	 if(status & WIEGAND_IN_IRQ_MASK)                    //韦根输入1
        	wiegand_in_interrupt();
    	else if(status & WIEGAND_OUT_IRQ_MASK)           //韦根输出使能 & 韦根输出中断
        	wiegand_out_interrupt();
		
    	return IRQ_HANDLED;
}

static int Arm9_wiegand_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int Arm9_wiegand_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int Arm9_wiegand_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	u8 tmp;
	switch (cmd)
	{
	case 0x10:   //G5V_IO
		{
			tmp=ioread8(wiegand_dev->mem+5);
			tmp &= 0xef;
			tmp |=(arg&0x1)<<4;
			iowrite8(tmp, wiegand_dev->mem+5);
			mb();
			break;
		}
	case 0x20:   //ep2c5_IO
		{
			tmp=ioread8(wiegand_dev->mem+5);
			tmp &= 0xd;
			tmp |=(arg&0x1)<<5;
			iowrite8(tmp, wiegand_dev->mem+5);
			mb();
			break;
		}
	case 0x30:   //IO_SHAN
		{
			s3c2410_gpio_cfgpin(S3C2410_GPC1, S3C2410_GPC1_OUTP);
			s3c2410_gpio_setpin(S3C2410_GPC1, (arg&0x01));
			break;
		}
	case 0x40:   //FM_RST
		{
			s3c2410_gpio_cfgpin(S3C2410_GPG4, S3C2410_GPG4_OUTP);
			s3c2410_gpio_setpin(S3C2410_GPG4, 1);
			mdelay(10);
			s3c2410_gpio_setpin(S3C2410_GPG4, 0);
			mdelay(10);
			s3c2410_gpio_setpin(S3C2410_GPG4, 1);
			break;
		}	
	default:
	return -ENOTTY;
	}
	return 0;
}


static ssize_t Arm9_wiegand_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	int i;
	down_interruptible(&wiegand_dev->sem_in);
	wait_event_interruptible(wiegand_dev->que_in, wiegand_dev->in_stat_flag==READY);
		
	for(i=0;i<(count/4);i++)
	{
		wiegand_GetOneSeq(wiegand_dev->buf_wiegend_in+i*4);
		wiegand_dev->in_stat_flag=BUSY;
		
		if (i+1<count/4)
			wait_event_interruptible(wiegand_dev->que_in, wiegand_dev->in_stat_flag==READY);
	}
	
	copy_to_user(buf, wiegand_dev->buf_wiegend_in, count);
	up(&wiegand_dev->sem_in);
	return count;
}

static ssize_t Arm9_wiegand_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	int i;
	down_interruptible(&wiegand_dev->sem_out);
		copy_from_user(wiegand_dev->buf_wiegend_out, buf, count);	
	for(i=0;i<(count/4);i++)
		wiegand_SentOneSeq(wiegand_dev->buf_wiegend_out+i*4);

	up(&wiegand_dev->sem_out);
	
	return count;
}

static struct file_operations wiegand_fops = {
	.owner		=	THIS_MODULE,
	.open		=	Arm9_wiegand_open,
	.release	       =	Arm9_wiegand_release,
	.ioctl		       =	Arm9_wiegand_ioctl,
	.read		=	Arm9_wiegand_read,
	.write		=	Arm9_wiegand_write,
};



static void wiegand_dev_destroy(struct device *ndev)
{
	iounmap(wiegand_dev->mem);
	free_irq(IRQ_EINT1, ndev);
	kfree(wiegand_dev->buf_wiegend_out);
	cdev_del(&wiegand_dev->cdev);
	unregister_chrdev_region(wiegand_dev->devno, 1);
	kfree(wiegand_dev);

	printk(KERN_INFO"S3C2410 wiegand unloaded\n");
}

static int wiegand_remove(struct device *ndev)
{
	wiegand_dev_destroy(ndev);
	return 0;
}

static int wiegand_dev_init(struct device *ndev)
{
	int result = 0;	

	if((wiegand_dev = kmalloc(sizeof(struct wiegand_t), GFP_KERNEL)) == NULL)//在内核空间中动态申请内存
		return -ENOMEM;
	memset(wiegand_dev, 0, sizeof(*wiegand_dev));
	wiegand_dev->mem = ioremap(GCS3_BASE,32);//内存空间映射：连接到CPLD的数据地址和总线

	if(Check_ID()<0)
		return ENODEV;
	
	if (wiegand_major)//申请设备号
	{
		wiegand_dev->devno = MKDEV(wiegand_major, wiegand_minor);
		result = register_chrdev_region(wiegand_dev->devno, 1, wiegand_name);
	}
	else
	{
		result = alloc_chrdev_region(&wiegand_dev->devno, 0, 1, wiegand_name);
		wiegand_major = MAJOR(wiegand_dev->devno);
		wiegand_minor = MINOR(wiegand_dev->devno);
	}

	if (result < 0)
	{
		printk(KERN_WARNING"%s: can't get major %d\n", __FUNCTION__, wiegand_major);
		return result;
	}
	cdev_init(&wiegand_dev->cdev, &wiegand_fops);//注册设备
	wiegand_dev->cdev.owner = THIS_MODULE;
	wiegand_dev->cdev.ops = &wiegand_fops;

	wiegand_dev->buf_wiegend_out=kmalloc(100,GFP_KERNEL);//建立数据区

	result  = cdev_add(&wiegand_dev->cdev, wiegand_dev->devno, 1);
	if (result )
		printk(KERN_NOTICE"Error %d adding wiegand%d", result, 0);

	init_MUTEX(&wiegand_dev->sem_out);//初始化信号向量
	init_MUTEX(&wiegand_dev->sem_in);//初始化信号向量

	init_waitqueue_head(&wiegand_dev->que_in);
	init_waitqueue_head(&wiegand_dev->que_out);
	wiegand_dev->in_stat_flag==BUSY;
	wiegand_dev->out_stat_flag==READY;

	result = request_irq(IRQ_EINT1, wiegand_interrupt, SA_INTERRUPT,"s3c2410-wiegand-int", ndev);//申请中断
	if (result)
		{
		printk(KERN_ERR "IRQ%d already in use\n", IRQ_EINT1);
		goto err;
		}
	return 0;

err:
	free_irq(IRQ_EINT1, ndev);
	wiegand_remove(ndev);
	return result;
}


static int wiegand_probe(struct device *ndev)
{
	int result;
	
	if (strcmp(ndev->bus_id, driver_name) != 0)
		return -1;

	if ((result = wiegand_dev_init(ndev)) != 0)
		wiegand_remove(ndev);

	printk("wiegand initialized\n");
	
	return result;
}

static struct device_driver wiegand_driver = {
	.name = driver_name,
	.bus = &platform_bus_type,
	.probe = wiegand_probe,
	.remove = wiegand_remove,
};

static int __init wiegand_init(void) 
{
	return driver_register(&wiegand_driver);
}

static void __exit wiegand_exit(void)
{
	driver_unregister(&wiegand_driver);
}

module_init(wiegand_init);
module_exit(wiegand_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("wiegand driver, Copyright(C) IrisKing");
