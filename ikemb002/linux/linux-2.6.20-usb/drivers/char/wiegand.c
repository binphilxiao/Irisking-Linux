/*
2009-07-15 Version 1 : wiegand input & wiegand output with interrupt.
*/

#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/interrupt.h>
#include <asm/semaphore.h>
#include <linux/platform_device.h>

#define IRQ_3 19


//mem for interface with user space
u8 *wig_buf;

static struct wiegand_t
{
	void __iomem *wig_mem;
	void __iomem *gpio;
	int wig_num;
	//u8 detect;          //configure
	struct semaphore sem;
}*wiegand_dev;

//这一段代码在中断函数内执行，从寄存器读取的数有错，因此还缺少对SEPNR[28]的写1清除中断挂起的操作。
//在中断函数之外的部分执行，都没有问题。
 void clear_reg_sepnp(void)
 {
    u32 reg_val;
    reg_val = in_be32(wiegand_dev->gpio + 0x2C);
    printk("kernel gpio = %x\n", wiegand_dev->gpio);
    printk("kernel SEPNR before = %x\n",reg_val);
    reg_val &= ~(0x10000000);
    reg_val |= 0x10000000;
    out_be32(wiegand_dev->gpio + 0x2C, reg_val);
    reg_val = in_be32(wiegand_dev->gpio + 0x2C);
    printk("kernel SEPNR after = %x\n",reg_val);
}

//韦根输入中断函数
void wig_in_read(int num)
{
    int i;
    u32 wig_data = 0;
    u8 config = ioread8(wiegand_dev->wig_mem);

    if (down_interruptible(&wiegand_dev->sem))
		return;

    for(i=0;i<4;i++)
        {
        wig_buf[i] = ioread8(wiegand_dev->wig_mem + i + 1 + 4*(num-1));
        mb();
        wig_data = (wig_data << 8) + wig_buf[i];
        }

    //中断处理完毕，configure相应位复位为1
    config |= (0x01<<(num-1));
    iowrite8(config, wiegand_dev->wig_mem);
    mb();

    up(&wiegand_dev->sem);
    printk("wig in %d = %x\n",num,wig_data);
}

//韦根输出中断函数
void wig_out_write(void)
{
    u8 config = ioread8(wiegand_dev->wig_mem);
    //中断处理完毕，configure相应位（中断输出使能 & 中断源判断）复位为1
    config |= 0x18;
    iowrite8(config, wiegand_dev->wig_mem);
    mb();
    printk("wiegand output complete\n");
}

static irqreturn_t wiegand_interrupt(int irq, void *dev_id)
{
	u8 config = ioread8(wiegand_dev->wig_mem);
    mb();

//clear_reg_sepnp();
//printk("detect = %x\n",config);

    if(!(config & 0x1))                    //韦根输入1
        wig_in_read(1);
    else if(!(config & 0x2))           //韦根输入2
        wig_in_read(2);
    else if(!(config & 0x4))           //韦根输入3
        wig_in_read(3);
    else if(!(config & 0x8) && !(config & 0x10))           //韦根输出使能 & 韦根输出中断
        wig_out_write();
    else if(!(config & 0x20))          //键盘
        printk("KeyBoard\n");
    else if(!(config & 0x40))          //防撬开关
        printk("FQ\n");
    else if(!(config & 0x80))          //一键复位
        printk("FW\n");
    else
        return IRQ_HANDLED;
    return IRQ_HANDLED;
}

static int FSL_Wiegand_open(struct inode *inode, struct file *filp)
{
	filp->private_data = wiegand_dev;
	wiegand_dev->gpio = ioremap(0xE0000700, 0x600);
	return 0;
}

static int FSL_Wiegand_release(struct inode *inode, struct file *filp)
{
    iounmap(wiegand_dev->gpio);
	filp->private_data = NULL;
	return 0;
}

static int FSL_Wiegand_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;
	int ret = 0;

	switch(cmd)
        {
        case 1:         //wiegand input 1
            {
            dev->wig_num = 1;
            break;
            }
        case 2:         //wiegand input 2
            {
            dev->wig_num = 2;
            break;
            }
        case 3:         //wiegand input 3
            {
            dev->wig_num = 3;
            break;
            }
        default:
            return -EINVAL;
        }
    return ret;
}

/*
static loff_t FSL_Wiegand_llseek(struct file *filp, loff_t off, int whence)
{
}
*/

static ssize_t FSL_Wiegand_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;
	ssize_t ret = 0;
	u32 wig_data = 0;
	int i;

	if (down_interruptible(&dev->sem))
		{
		printk("FSL_Wiegand_read down_interruptible failure!\n");
		return -ERESTARTSYS;
		}

    for(i=0;i<4;i++)
        {
        wig_buf[i] = ioread8(dev->wig_mem + 4*(dev->wig_num-1) + i + 1);
        mb();
        wig_data = (wig_data << 8) + wig_buf[i];
        }
    up(&dev->sem);

	if((copy_to_user(buf, &wig_data, count)))
		return -EFAULT;

    ret =count;
	return ret;
}

static ssize_t FSL_Wiegand_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	struct wiegand_t *dev = (struct wiegand_t *)filp->private_data;
	ssize_t ret = 0;
	u32 dat = 0;
	u8 gett;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if ((copy_from_user(&dat, buf, count)))
		{
		up(&dev->sem);
		return -EFAULT;
		}
printk("wiegand out data = %x\n",dat);

    gett = dat;
	iowrite8(gett, dev->wig_mem+1);
	mb();
	gett = dat>>8;
	iowrite8(gett, dev->wig_mem+2);
	mb();
	gett = dat>>16;
	iowrite8(gett, dev->wig_mem+3);
	mb();
	gett = dat>>24;
	iowrite8(gett, dev->wig_mem+4);
	mb();

    //设置韦根输出使能为低
    gett = ioread8(wiegand_dev->wig_mem);
    gett &= ~(0x18);
    iowrite8(gett, dev->wig_mem);

    up(&dev->sem);
	ret = count;
	return ret;
}

static struct file_operations wiegand_fops = {
	.owner		=	THIS_MODULE,
	.open		=	FSL_Wiegand_open,
	.release	=	FSL_Wiegand_release,
	.ioctl		    =	FSL_Wiegand_ioctl,
	//.llseek		=	FSL_Wiegand_llseek,
	.read		=	FSL_Wiegand_read,
	.write		=	FSL_Wiegand_write,
};

static int Wiegand_remove(struct device *ndev)
{
    free_irq(IRQ_3, ndev);
	iounmap(wiegand_dev->wig_mem);
	kfree(wig_buf);
	kfree(wiegand_dev);
	//cdev_del(&wiegand_dev->cdev);
	//unregister_chrdev_region(wiegand_dev->devno, 1);
	remove_proc_entry("wiegand", NULL);
	printk(KERN_INFO "Freescale mpc8313 wiegand removed\n");
	return 0;
}

static int Wiegand_probe(struct device *ndev)
{
    void __iomem *gpio;
    int reg_val;
    struct proc_dir_entry *proc_dir, *dm;

//在内核空间中动态申请内存
	if((wiegand_dev = kmalloc(sizeof(struct wiegand_t), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "wiegand_t: out of memory\n");
		return -ENOMEM;
		}
	memset(wiegand_dev, 0, sizeof(*wiegand_dev));

	//可以自己设定需要几个字节的空间，当前是4字节
	if((wig_buf = kmalloc(4*sizeof(u8), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "buffer: out of memory\n");
		return -ENOMEM;
		}
	memset(wig_buf, 0, 4*sizeof(*wig_buf));

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
    //reg_val = 0xF0000000 | (0x01<<11) | (01<<2) | 0x1;
    out_be32(gpio, reg_val);                  //BR2

    reg_val = 0xFFFF8000 | 0x100 | 0xF0 | 0x2 | 0x1;
    out_be32(gpio + 4, reg_val);            //OR2,时序有问题找它

    iounmap(gpio);

//内存空间映射：连接到FPGA的数据地址和总线
	wiegand_dev->wig_mem = ioremap(0xF0000000,32);

	//创建proc文件
	proc_dir = proc_mkdir("char_wig", NULL);
	if(proc_dir == NULL)
		return -ENOMEM;
	dm = create_proc_entry("wiegand", 0644, proc_dir);
	if(dm == NULL)
		return -ENOMEM;

    dm->proc_fops = &wiegand_fops;
    dm->owner = THIS_MODULE;

//申请中断

    gpio = ioremap(0xE000072C, 4);      //中断处理完毕后写1清除
    printk("gpio = %x\n",gpio);
    reg_val = in_be32(gpio);
    printk("SEPNR before = %x\n",reg_val);
    reg_val &= ~(0x10000000);
    reg_val |= 0x10000000;
    out_be32(gpio, reg_val);
    reg_val = in_be32(gpio);
    printk("SEPNR after = %x\n",reg_val);
    iounmap(gpio);

/*
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
*/

//初始化信号向量
	init_MUTEX(&wiegand_dev->sem);

    if (request_irq(IRQ_3, wiegand_interrupt, SA_INTERRUPT,"mpc8313-wig-int", ndev))
        {
        printk(KERN_ERR "IRQ%d already in use\n", IRQ_3);
        free_irq(IRQ_3, ndev);
        Wiegand_remove(ndev);
        return 1;
        }

	printk(KERN_INFO"Freescale mpc8313 wiegand with interrupt initialized\n");
	return 0;
}

static struct device_driver Wiegand_driver = {
	.name = "mpc83xx_wiegand",
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

MODULE_AUTHOR("IrisKing");
MODULE_DESCRIPTION("Driver for wiegand in MPC83xx uProcessor");
MODULE_LICENSE("GPL");
