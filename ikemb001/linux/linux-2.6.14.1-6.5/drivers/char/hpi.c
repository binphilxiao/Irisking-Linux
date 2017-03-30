#include <linux/module.h>
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
#include <asm/arch/regs-clock.h>
#include <asm/arch/hardware.h>
#include <asm/arch/map.h>
#include <asm/arch/regs-gpio.h>

//#define  HPI_BASEADDR           0x20000000      //      BANK 4

#define DM642_TEXT_ADDR 0x0
#define DM642_FRAMEBUFFER_ADDR 0x7C000000
#define DM642_BSS_ADDR 0x80800000
#define DM642_DATA_ADDR 0x81000000

static void __iomem *ctl_wr_base;
static void __iomem *ctl_rd_base;
static void __iomem *addr_wr_base;
static void __iomem *addr_rd_base;
static void __iomem *sdat_wr_base;
static void __iomem *sdat_rd_base;
static void __iomem *mdat_wr_base;
static void __iomem *mdat_rd_base;

#define HPI_CH0 0
#define HPI_CH1 1
#define HPI_CH2 2
#define HPI_CH3 3
#define HPI_CH4 4
#define HPI_CH5 5
#define HPI_CH6 6
#define HPI_CH7 7

#define HPI_LOADTEXT 0
#define HPI_LOADBSS 1
#define HPI_LOADFRMAE 2
#define HPI_LOADIMG 3
#define HPI_CHECKIMG 4
#define HPI_DM642START 5

static int cmmd=0;

static struct resource *s3c2410_hpi_mem_ctlw;
static struct resource *s3c2410_hpi_mem_ctlr;
static struct resource *s3c2410_hpi_mem_addrw;
static struct resource *s3c2410_hpi_mem_addrr;
static struct resource *s3c2410_hpi_mem_sdatw;
static struct resource *s3c2410_hpi_mem_sdatr;
static struct resource *s3c2410_hpi_mem_mdatw;
static struct resource *s3c2410_hpi_mem_mdatr;

static u8 * buf8;
static u32 *buf32;
static loff_t nowaddr;

typedef struct hpi_info {

	u16 irq;
	int major;
	int dev;
	u8 imode;
	struct file_operations * phpi_fops;

} hpi_info_t;
static hpi_info_t db;

static ssize_t hpi_write(struct file * file,const char * buffer,
size_t count,loff_t * ppos){
        unsigned long i,len,ok;
	buf32=(u32 *)buf8;
	copy_from_user(buf8,buffer,count);
//	iowrite32(nowaddr, addr_wr_base);
//	mb();
//	while(s3c2410_gpio_getpin(S3C2410_GPF3))printk("---wait---\n");
        if (count%4==0)
		len=count/4;
	else    
		len=count/4+1;
//	for (i=0;i<len;i++)
//		printk("====================0x%x\n",*(buf32+i));
	for (i=0;i<len;i++)
	{
		iowrite32(*(buf32+i), sdat_wr_base);
		mb();
		//while(s3c2410_gpio_getpin(S3C2410_GPF3));
	}
        return 0;
}


static int hpi_open(struct inode *inode, struct file *file){
	//printk("hpi_open\n");
       return 0;
}

static int hpi_release(struct inode *inode, struct file *file){
	//printk("hpi_release\n");
       return 0;
}

static int hpi_ioctl(struct inode *inode, struct file *file,
uint cmd, ulong arg){
	int i;
		switch (cmd) {
		case HPI_CH0:{
			cmmd=cmd;
			break;
		}
		case HPI_CH1:{
			cmmd=cmd;
			break;
		}
		case HPI_CH2:{
			cmmd=cmd;
			break;
		}
		case HPI_CH3:{
			cmmd=cmd;
			break;
		}
		case HPI_CH4:{
			cmmd=cmd;
			break;
		}
		case HPI_CH5:{
			cmmd=cmd;
			break;
		}
		case HPI_CH6:{
			cmmd=cmd;
			        s3c2410_gpio_setpin(S3C2410_GPA16,1);
			        for (i=0;i<10000;i++);
			        s3c2410_gpio_setpin(S3C2410_GPA16,0);
				for (i=0;i<20000;i++);
			        s3c2410_gpio_setpin(S3C2410_GPA16,1);
			break;
		}
		case HPI_CH7:{  //this is for test ioctl cmd
			cmmd=cmd;
			iowrite32(0x00020002, ctl_wr_base);
			break;
		}
		default:{
		       cmmd=0;
			break;
		}
	}
       return 0;
}

static ssize_t hpi_read(struct file * file,char * buffer,
size_t count,loff_t * ppos){
        int i,len,ok,rdy;
        buf32=(u8 *)buf8;
//	iowrite32(nowaddr, addr_wr_base);
//	mb();
//	while(s3c2410_gpio_getpin(S3C2410_GPF3)) printk("---wait---\n");
	if (count%4==0)
		len=count/4;
	else
		len=count/4+1;
	for (i=0;i<len;i++) 
		{
		*(buf32+i)=ioread32(sdat_rd_base);
		mb();
		//while(s3c2410_gpio_getpin(S3C2410_GPF3));
		}
	copy_to_user(buffer, buf8,count);
        return 0;
}


static loff_t hpi_llseek(struct file * file, loff_t offset, int orig){
	nowaddr=orig+offset;
	        iowrite32(nowaddr, addr_wr_base);
		mb();
		//while(s3c2410_gpio_getpin(S3C2410_GPF3)) printk("---wait---\n");
	return (nowaddr);
}

static struct file_operations hpi_fops={
//owner :   THIS_MODULE ,
open :     hpi_open ,
read :     hpi_read ,
write :     hpi_write ,
ioctl :     hpi_ioctl ,
release :   hpi_release,
llseek:  hpi_llseek
};


static void init_hpi(void){
	printk("init_hpi\n");
}
static int hpi_probe(struct device *ndev) {
	struct platform_device *pdev = to_platform_device(ndev);
	struct resource *res0,*res1,*res2,*res3,*res4,*res5,*res6,*res7;
	unsigned long i;
        //init_hpi();
	 db.phpi_fops = &hpi_fops;

	 
/////////////////////////////////////////////////////////////////////////////////////////	 

	res0 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res0 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_ctlw = request_mem_region(res0->start, res0->end-res0->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_ctlw == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	ctl_wr_base = ioremap_nocache(res0->start, res0->end - res0->start + 1);
	if (ctl_wr_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x   0x %x\n",ctl_wr_base,res0->end);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(ctl_wr_base));
	
/////////////////////////////////////////////////////////////////////////////////////////	 

	res1 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (res1 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_ctlr = request_mem_region(res1->start, res1->end-res1->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_ctlr == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	ctl_rd_base = ioremap_nocache(res1->start, res1->end - res1->start + 1);
	if (ctl_rd_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x   0x %x\n",ctl_rd_base,res1->end);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(ctl_rd_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res2 = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	if (res2 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_addrw = request_mem_region(res2->start, res2->end-res2->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_addrw == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	addr_wr_base = ioremap_nocache(res2->start, res2->end - res2->start + 1);
	if (addr_wr_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",addr_wr_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(addr_wr_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res3 = platform_get_resource(pdev, IORESOURCE_MEM, 3);
	if (res3 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_addrr = request_mem_region(res3->start, res3->end-res3->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_addrr == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	addr_rd_base = ioremap_nocache(res3->start, res3->end - res3->start + 1);
	if (addr_rd_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",addr_rd_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(addr_rd_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res4 = platform_get_resource(pdev, IORESOURCE_MEM, 4);
	if (res4 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_sdatw = request_mem_region(res4->start, res4->end-res4->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_sdatw == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	sdat_wr_base = ioremap_nocache(res4->start, res4->end - res4->start + 1);
	if (sdat_wr_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",sdat_wr_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(sdat_wr_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res5 = platform_get_resource(pdev, IORESOURCE_MEM, 5);
	if (res5 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_sdatr = request_mem_region(res5->start, res5->end-res5->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_sdatr == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	sdat_rd_base = ioremap_nocache(res5->start, res5->end - res5->start + 1);
	if (sdat_rd_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",sdat_rd_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(sdat_rd_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res6 = platform_get_resource(pdev, IORESOURCE_MEM, 6);
	if (res6 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_mdatw = request_mem_region(res6->start, res6->end-res6->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_mdatw == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	mdat_wr_base = ioremap_nocache(res6->start, res6->end - res6->start + 1);
	if (mdat_wr_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",mdat_wr_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(mdat_wr_base));
/////////////////////////////////////////////////////////////////////////////////////////	 

	res7 = platform_get_resource(pdev, IORESOURCE_MEM, 7);
	if (res7 == NULL) {
		dev_err(ndev, "failed to get memory region resource\n");
		return -ENOENT;
	}
	s3c2410_hpi_mem_mdatr = request_mem_region(res7->start, res7->end-res7->start+1,
				     pdev->name);
	if (s3c2410_hpi_mem_mdatr == NULL) {
		dev_err(ndev, "failed to reserve memory region\n");
		return -ENOENT;
	}
	mdat_rd_base = ioremap_nocache(res7->start, res7->end - res7->start + 1);
	if (mdat_rd_base == NULL) {
		dev_err(ndev, "failed ioremap_nocache()\n");
		return -EINVAL;
	}
	//printk("ioremap_nocache address is 0x%x\n",mdat_rd_base);
	//printk("ioremap_nocache paddress is 0x%x\n",__pa(mdat_rd_base));
/////////////////////////////////////////////////////////////////////////////////////////	
       /* {
                unsigned int *p;
                p = ioremap_nocache(0x48000000, 4);
                if (p)
                {
                        printk("%08x\n", *p);
                        iounmap(p);
                }
            p = ioremap_nocache(0x48000014, 4);
                if (p)
                { 
                        printk("%08x\n", *p);
                        iounmap(p);
                }
        }*/
        //s3c2410_gpio_cfgpin(S3C2410_GPF3,S3C2410_GPF3_INP);
        //s3c2410_gpio_pullup(S3C2410_GPF3,0);
	s3c2410_gpio_cfgpin(S3C2410_GPA16,S3C2410_GPA16_OUT);
	s3c2410_gpio_pullup(S3C2410_GPA16,1);
	s3c2410_gpio_setpin(S3C2410_GPA16,1);
	for (i=0;i<10000;i++);
	s3c2410_gpio_setpin(S3C2410_GPA16,0);
	for (i=0;i<20000;i++);
	s3c2410_gpio_setpin(S3C2410_GPA16,1);

        db.major=240;
        if (db.dev=register_chrdev(db.major, "dm642_hpi", db.phpi_fops))
        {
        		printk("Driver register fail\n");
			return -EINVAL;
        }
	buf8=(u8 *)kmalloc(0x10000,GFP_KERNEL);
	if (buf8)
	{
		buf32=(u32 *)buf8;
		printk("kmalloc successful\n");
	}
	iowrite32(DM642_TEXT_ADDR, addr_wr_base);
	for (i=0;i<100000;i++);
       printk("DM642 hpi driver initialized\n");
        return 0;
}


static int hpi_remove(struct device *dev) {
        unregister_chrdev(db.major,"dm642_hpi");
	if (s3c2410_hpi_mem_ctlw != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(ctl_wr_base);
		release_resource(s3c2410_hpi_mem_ctlw);
		kfree(s3c2410_hpi_mem_ctlw);
	}

	if (s3c2410_hpi_mem_ctlr != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(ctl_rd_base);
		release_resource(s3c2410_hpi_mem_ctlr);
		kfree(s3c2410_hpi_mem_ctlr);
	}

	if (s3c2410_hpi_mem_addrw != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(addr_wr_base);
		release_resource(s3c2410_hpi_mem_addrw);
		kfree(s3c2410_hpi_mem_addrw);
	}

	if (s3c2410_hpi_mem_addrr != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(addr_rd_base);
		release_resource(s3c2410_hpi_mem_addrr);
		kfree(s3c2410_hpi_mem_addrr);
	}

	if (s3c2410_hpi_mem_sdatw != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(sdat_wr_base);
		release_resource(s3c2410_hpi_mem_sdatw);
		kfree(s3c2410_hpi_mem_sdatw);
	}

	if (s3c2410_hpi_mem_sdatr != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(sdat_rd_base);
		release_resource(s3c2410_hpi_mem_sdatr);
		kfree(s3c2410_hpi_mem_sdatr);
	}

	if (s3c2410_hpi_mem_mdatw != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(mdat_wr_base);
		release_resource(s3c2410_hpi_mem_mdatw);
		kfree(s3c2410_hpi_mem_mdatw);
	}

	if (s3c2410_hpi_mem_mdatr != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(mdat_rd_base);
		release_resource(s3c2410_hpi_mem_mdatr);
		kfree(s3c2410_hpi_mem_mdatr);
	}

	kfree(buf8);
        printk(" unloaded\n");
        return 0;
}


static struct device_driver hpi_driver = {
        .name = "s3c2410-hpi",
        .bus = &platform_bus_type,
        .probe = hpi_probe,
        .remove = hpi_remove,
};
static int __init hpi_init(void) {
        return driver_register(&hpi_driver);
}

static void __exit hpi_exit(void) {
        driver_unregister(&hpi_driver);
}

module_init(hpi_init);
module_exit(hpi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abingo@263.net");
MODULE_DESCRIPTION("hpi driver");
