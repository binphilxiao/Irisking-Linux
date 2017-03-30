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
#include <asm/arch/regs-adc.h>
#include <asm/arch/hardware.h>
#include <asm/arch/map.h>
//#include <asm/arch/S3C2410.h>

#define ADC_CH0 0
#define ADC_CH1 1
#define ADC_CH2 2
#define ADC_CH3 3
#define ADC_CH4 4
#define ADC_CH5 5
#define ADC_CH6 6
#define ADC_CH7 7
#define ADC_FRQ 8

#define FCLK 202800000
#define HCLK (202800000/2)
#define PCLK (202800000/4)
#define UCLK PCLK
#define ADC_FREQ 2500000

volatile u32 preScaler;
static  u8 channel=0;
//static void __iomem *adc_base;
u32 * adc_base;
u32 * adc_dat_base;
static struct clk* adc_clock;

#ifdef DEBUG
#define DPRINTK printk
#else
#define DPRINTK( x... )
#endif

//static int smdk2410_adc_major;
static int smdk2410_adc_dev;
static struct resource *s3c2410_adc_mem;

int ReadAdc(int ch)
{
    int rets,ret[10],i;
    writew(((1<<14)|(preScaler<<6)|(ch<<3)),adc_base + S3C2410_ADCCON);
    for(i=0;i<10000;i++);	//DELAY FOR INTERNAL WORK
    for (i=0;i<10;i++)
    {
    	ret[i]=readw(adc_base + S3C2410_ADCCON);
    	ret[i]= ret[i] + S3C2410_ADCCON_STARTMASK;   //start ADC
   	// printk("ADCCON is %x\n",ret);
    	writew(ret[i],adc_base + S3C2410_ADCCON);
    	while(readw(adc_base + S3C2410_ADCCON) & S3C2410_ADCCON_STARTMASK);	//check if Enable_start is low
    	ret[i]=readw(adc_base + S3C2410_ADCCON) & S3C2410_ADCCON_ECFLG;
    	ret[i]=readw(adc_dat_base) & S3C2410_ADCDAT0_XPDATA_MASK;
    }
    rets=(ret[0]+ret[1]+ret[2]+ret[3]+ret[4]+ret[5]+ret[6]+ret[7]+ret[8]+ret[9])/10;
   // printk("ADC value is %x\n",rets);
    return rets;
}

static ssize_t smdk2410_adc_write(struct file *file, const char *buffer,
size_t count, loff_t * ppos)
{
	DPRINTK("audio_write : end count=%d\n\n", 0);
	return 0;
}

static ssize_t smdk2410_adc_read(struct file *file, char *buffer,
size_t count, loff_t * ppos)
{
       int ret;
	printk("ADC channel is %d \n",channel);
	ret=ReadAdc(channel);
	ret=ret & 0x3ff;
	copy_to_user(buffer,&ret,2);
	//printk("ADC read : value=%d\n", ret);
	return ret;
}

static int smdk2410_adc_ioctl(struct inode *inode, struct file *file,
uint cmd, ulong arg)
{
	u8 ret;
	printk("s3c2410 ADC ioctl\n");
	switch (cmd) {
		case ADC_CH0:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH1:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH2:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH3:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH4:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH5:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH6:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_CH7:{
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
		}
		case ADC_FRQ:{
			preScaler = PCLK/arg -1;   
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(preScaler<<6);
			writew(ret,adc_base + S3C2410_ADCCON);
			break;
		}
		default:
			ret=(readw(adc_base + S3C2410_ADCCON) & (!S3C2410_ADCCON_PRSCVLMASK))|(cmd<<3);
			writew(ret,adc_base + S3C2410_ADCCON);
			channel=cmd;
			break;
	}
	return 0;
}

static int smdk2410_adc_open(struct inode *inode, struct file *file)
{
       u32 ret;
       preScaler = ADC_FREQ;
       printk("ADC conv. freq. = %dHz\n",preScaler);
       preScaler = PCLK/ADC_FREQ -1; 
	ret= (S3C2410_ADCCON_PRSCEN)|(preScaler<<6)|(channel<<3);	
	writew(ret,adc_base + S3C2410_ADCCON);
       return 0;
}

static int smdk2410_adc_release(struct inode *inode, struct file *file)
{
       return 0;
}

static struct file_operations smdk2410adc_fops = {
write: smdk2410_adc_write,
read: smdk2410_adc_read,
ioctl: smdk2410_adc_ioctl,
open: smdk2410_adc_open,
release: smdk2410_adc_release
};


static int s3c2410adc_probe(struct device *dev) {
	 struct platform_device *pdev = to_platform_device(dev);
        struct resource *res;
        //unsigned long flags;
        adc_clock = clk_get(NULL, "adc");
	if (!adc_clock) {
	printk(KERN_ERR "failed to get adc clock source\n");
	return -ENOENT;
	}
	clk_use(adc_clock);
	clk_enable(adc_clock);

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (res == NULL) {
                printk("failed to get memory region resouce\n");
                return -ENOENT;
        }

	 s3c2410_adc_mem = request_mem_region(res->start, 4,
				     pdev->name);
	if (s3c2410_adc_mem == NULL) {
		dev_err(dev, "failed to reserve memory region\n");
		return  -ENOENT;
	}
	adc_base = ioremap(res->start, 4);
	if ( adc_base == NULL) {
		dev_err(dev, "failed ioremap()\n");
		return  -EINVAL;
	}
	adc_dat_base = ioremap(res->start+0xc, 4);
	if ( adc_dat_base == NULL) {
		dev_err(dev, "failed ioremap()\n");
		return  -EINVAL;
	}
	 s3c2410_adc_mem = res;
        if (smdk2410_adc_dev=register_chrdev(250, "s3c2410-adc", &smdk2410adc_fops))
        {
        		printk("Driver register fail\n");
        }
        return 0;
}

static int s3c2410adc_remove(struct device *dev) {
	 printk("s3c2410 ADC unloaded\n");
	 release_resource(s3c2410_adc_mem);
	 if (adc_clock) {
		clk_disable(adc_clock);
		clk_unuse(adc_clock);
		clk_put(adc_clock);
		adc_clock = NULL;
	 }
	 	if (s3c2410_adc_mem != NULL) {
		pr_debug("s3c2410_hpi: releasing s3c2410_hpi_mem\n");
		iounmap(adc_base);
		release_resource(s3c2410_adc_mem);
		kfree(s3c2410_adc_mem);
	}
        return unregister_chrdev(smdk2410_adc_dev,"s3c2410-adc");
}

static struct device_driver s3c2410adc_driver = {
        .name = "s3c2410-adc",
        .bus = &platform_bus_type,
        .probe = s3c2410adc_probe,
        .remove = s3c2410adc_remove,
};

static int __init s3c2410_adc_init(void) {
        return driver_register(&s3c2410adc_driver);
}

static void __exit s3c2410_adc_exit(void) {
        driver_unregister(&s3c2410adc_driver);
}

module_init(s3c2410_adc_init);
module_exit(s3c2410_adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("superlp<xinshengtaier@eyou.com>");
MODULE_DESCRIPTION("S3C2410 uda1341 sound driver");

