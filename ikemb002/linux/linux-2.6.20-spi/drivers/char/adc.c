/*
2009-06-30 Version 1
*/

#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/semaphore.h>

//mem for interface with user space
u8 *adc_in;

static struct adc_t
{
	void __iomem *adc_mem;
	struct semaphore sem;
}*adc_dev;


static int FSL_adc_open(struct inode *inode, struct file *filp)
{
	filp->private_data = adc_dev;	
	return 0;
}

static int FSL_adc_release(struct inode *inode, struct file *filp)
{
	filp->private_data = NULL;
	return 0;
}

static ssize_t FSL_adc_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	struct adc_t *dev = (struct adc_t *)filp->private_data;
    ssize_t ret = 0;

    if (down_interruptible(&dev->sem))
		{
		printk("FSL_adc_write down_interruptible failure!\n");
		return -ERESTARTSYS;
		}

    *adc_in = ioread8(dev->adc_mem + 0xD);
	mb();
    //printk("adc = %d\n",*adc_in);
    //printk("analog = %d\n",(*adc_in)*5/256);

    up(&dev->sem);
    
    if((copy_to_user(buf, adc_in, count)))
        return -EFAULT;

	ret =count;
	return ret;
}

/*
static ssize_t FSL_adc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
}
static int FSL_adc_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
}
static loff_t FSL_adc_llseek(struct file *filp, loff_t off, int whence)
{
}
*/

static struct file_operations adc_fops = {
	.owner		=	THIS_MODULE,
	.open		=	FSL_adc_open,
	.release	=	FSL_adc_release,
	.read		=	FSL_adc_read,
	//.write		=	FSL_adc_write,
	//.ioctl		=	FSL_adc_ioctl,
	//.llseek		=	FSL_adc_llseek,
};

static int __devexit ADC_remove(struct device *ndev)
{
	iounmap(adc_dev->adc_mem);
	kfree(adc_in);
	kfree(adc_dev);

	//cdev_del(&adc_dev->cdev);
	//unregister_chrdev_region(adc_dev->devno, 1);

	remove_proc_entry("ADC", NULL);
	printk(KERN_INFO "Freescale mpc8313 ADC removed\n");
	return 0;
}

static int ADC_probe(struct device *ndev)
{
	void __iomem *gpio;
	int reg_val;
	struct proc_dir_entry *proc_dir, *dm;

//在内核空间中动态申请内存
	if((adc_dev = kmalloc(sizeof(struct adc_t), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "adc_t: out of memory\n");
		return -ENOMEM;
		}
	memset(adc_dev, 0, sizeof(*adc_dev));

	//可以自己设定需要几个字节的空间，当前是1字节
	if((adc_in = kmalloc(sizeof(u8), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "buffer: out of memory\n");
		return -ENOMEM;
		}
	memset(adc_in, 0, sizeof(*adc_in));

//eLBC的LCS2的寄存器设置, GPCM Mode——开窗32KB，为的是与GPCM的最小区域（32KB—64MB）对应
    //System Configuration Registers
    gpio = ioremap(0xE0000030, 8);
    
    reg_val = 0xF0000000;
    out_be32(gpio, reg_val);           //LBLAWBAR2
    
    reg_val = 0x8000000E;
    out_be32(gpio + 4, reg_val);      //LBLAWAR2
    
    //eLBC Registers
    gpio = ioremap(0xE0005010, 8);
    
    reg_val = 0xF0000000 | (0x01<<11) | 0x1;
    out_be32(gpio, reg_val);                  //BR2
    
    reg_val = 0xFFFF8000 | 0x100 | 0xF0 | 0x2 | 0x1;
    out_be32(gpio + 4, reg_val);            //OR2,时序有问题找它
    
    iounmap(gpio);

//内存空间映射：连接到FPGA的数据地址和总线
	adc_dev->adc_mem = ioremap(0xF0000000,32);

	//创建proc文件
	proc_dir = proc_mkdir("char_adc", NULL);
	if(proc_dir == NULL)
		return -ENOMEM;
	dm = create_proc_entry("ADC", 0644, proc_dir);
	if(dm == NULL)
		return -ENOMEM;

	dm->proc_fops = &adc_fops;
	dm->owner = THIS_MODULE;

/*
//分配设备编号
	//static
	if (adc_major)
		{
		adc_dev->devno = MKDEV(adc_major, adc_minor);
		result = register_chrdev_region(adc_dev->devno, 1, "adc");
		}
	//dynamic
	else
		{
		result = alloc_chrdev_region(&adc_dev->devno, adc_minor, 1, "adc");
		adc_major = MAJOR(adc_dev->devno);
		}
	if (result < 0)
		{
		printk(KERN_WARNING"adc: can't get major %d\n", adc_major);
		return result;
		}

//注册设备
	//devno = MKDEV(adc_major, adc_minor);
	cdev_init(&adc_dev->cdev, &adc_fops);
	adc_dev->cdev.owner = THIS_MODULE;
	adc_dev->cdev.ops = &adc_fops;
	err = cdev_add(&adc_dev->cdev, &adc_dev->devno, 1);
	if (err)
		printk(KERN_NOTICE"Error %d adding adc%d", err, 0);
*/

//初始化信号向量
	init_MUTEX(&adc_dev->sem);

	printk(KERN_INFO"Freescale mpc8313 ADC initialized\n");
	return 0;
}

static struct device_driver ADC_driver = {
	.name = "mpc83xx_adc",
	.bus = &platform_bus_type,
	.probe = ADC_probe,
	.remove = __devexit_p(ADC_remove),
};

static int __init ADC_init(void) 
{
	return driver_register(&ADC_driver);
}

static void __exit ADC_exit(void)
{
	driver_unregister(&ADC_driver);
}

module_init(ADC_init);
module_exit(ADC_exit);

MODULE_AUTHOR("IrisKing");
MODULE_DESCRIPTION("Driver for ADC in MPC83xx uProcessor");
MODULE_LICENSE("GPL");
