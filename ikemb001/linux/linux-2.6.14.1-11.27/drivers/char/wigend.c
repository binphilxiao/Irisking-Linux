#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/errno.h>

#include <linux/pm.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/semaphore.h>
#include <asm/arch/hardware.h>
#include <asm/arch/map.h>


static void __iomem * wil_base;
static u8 * buf8;
//static loff_t nowaddr;

#ifdef DEBUG
#define DPRINTK printk
#else
#define DPRINTK( x... )
#endif

static int smdk2410_wil_dev;
static struct resource *s3c2410_wil_mem;

static ssize_t smdk2410_wil_write(struct file *file, const char *buffer,
size_t count, loff_t * ppos)
{
	unsigned long i;
	printk("wigend write\n");
	copy_from_user(buf8,buffer,count);
	for (i=0;i<16;i++)
	{
		iowrite8(*(buf8+i), (wil_base+i*4));
		mb();
	}
	return 0;
}

static ssize_t smdk2410_wil_read(struct file *file, char *buffer,
size_t count, loff_t * ppos)
{
	unsigned long i;
	printk("wigend read\n");
	for (i=0;i<16;i++) 
	{
		*(buf8+i)=ioread8(wil_base+i*4);
		mb();
	}
	copy_to_user(buffer,buf8,count);
	return 0;
}

static int smdk2410_wil_ioctl(struct inode *inode, struct file *file,
uint cmd, ulong arg)
{
	printk("s3c2410 WIL ioctl\n");
	return 0;
}

static int smdk2410_wil_open(struct inode *inode, struct file *file)
{
       return 0;
}

static int smdk2410_wil_release(struct inode *inode, struct file *file)
{
       return 0;
}

static struct file_operations smdk2410wil_fops = {
write: smdk2410_wil_write,
read: smdk2410_wil_read,
ioctl: smdk2410_wil_ioctl,
open: smdk2410_wil_open,
release: smdk2410_wil_release
};


static int s3c2410wil_probe(struct device *dev) {
	 struct platform_device *pdev = to_platform_device(dev);
        struct resource *res;

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (res == NULL) {
                printk("failed to get memory region resouce\n");
                return -ENOENT;
        }

	 s3c2410_wil_mem = request_mem_region(res->start, 16,
				     pdev->name);
	if (s3c2410_wil_mem == NULL) {
		dev_err(dev, "failed to reserve memory region\n");
		return  -ENOENT;
	}
	printk("wil_base is %x\n",res->start);
	wil_base = ioremap(res->start, 16);
	if ( wil_base == NULL) {
		dev_err(dev, "failed ioremap()\n");
		return  -EINVAL;
	}
	 s3c2410_wil_mem = res;
        if (smdk2410_wil_dev=register_chrdev(230, "s3c2410-wil", &smdk2410wil_fops))
        {
        		printk("Driver register fail\n");
        }
	buf8=(u8 *)kmalloc(0x16,GFP_KERNEL);
	if (buf8)
	{
		printk("kmalloc successful\n");
	}
    return 0;
}

static int s3c2410wil_remove(struct device *dev) {
	 release_resource(s3c2410_wil_mem);

	 if (s3c2410_wil_mem != NULL) {
		iounmap(wil_base);
		release_resource(s3c2410_wil_mem);
		kfree(s3c2410_wil_mem);
	}
        return unregister_chrdev(smdk2410_wil_dev,"s3c2410-wil");
}

static struct device_driver s3c2410wil_driver = {
        .name = "s3c2410-wig",
        .bus = &platform_bus_type,
        .probe = s3c2410wil_probe,
        .remove = s3c2410wil_remove,
};

static int __init s3c2410_wil_init(void) {
        return driver_register(&s3c2410wil_driver);
}

static void __exit s3c2410_wil_exit(void) {
        driver_unregister(&s3c2410wil_driver);
}

module_init(s3c2410_wil_init);
module_exit(s3c2410_wil_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("superlp<xinshengtaier@eyou.com>");
MODULE_DESCRIPTION("S3C2410 wilgend sound driver");
