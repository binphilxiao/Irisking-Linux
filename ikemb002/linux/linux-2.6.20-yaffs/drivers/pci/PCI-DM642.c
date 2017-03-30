/*
struct DM642_register{
	//Configuration Registers
	unsigned int VID;       //vendor ID
	unsigned int DID;       //device ID

	unsigned int command;
	unsigned int status;

	unsigned char RID;      //revision ID
	unsigned long CC;       //class_code;

	unsigned char CLS;      //Cache Line Size
	unsigned char LT;       //Latency Timer 
	unsigned char HT;       //Header Type

	void __iomem *mmio_addr;
	void __iomem *ioaddr;

	unsigned int SVI;       //Subsystem Vendor ID
	unsigned int SI;        //Subsystem ID

	unsigned char CP;       //Capabilities Pointer

	unsigned char IL;       //Interrupt Line
	unsigned char IP;       //Interrupt Pin
	unsigned char Min_Grant;
	unsigned char Max_Latency;

	unsigned char CI;       //Capability ID
	unsigned char NIP;      //Next Item Pointer
	unsigned int PMC;       //Power Management Capabilities

	unsigned int PMCS;      //Power Management Control/Status
	unsigned char PD;       //Power Data

	//IO Registers
	unsigned long HSR;		//Host status register
	unsigned long HDCR;		//Host-to-DSP control register
	unsigned long DSPP;		//DSP page register

	//Memory-Mapped Registers
	unsigned long RSTSRC;           //DSP reset source/status register
	unsigned long PCIIS;            //PCI interrupt source register
	unsigned long PCIIEN;           //PCI interrupt enable register
	unsigned long DSPMA;            //DSP master address register
	unsigned long PCIMA;            //PCI master address register
	unsigned long PCIMC;            //PCI master control register
	unsigned long CDSPA;            //Current DSP address register
	unsigned long CPCIA;            //Current PCI address register
	unsigned long CCNT;             //Current byte count register
	unsigned long EEADD;            //EEPROM address register
	unsigned long EEDAT;            //EEPROM data register
	unsigned long EECTL;            //EEPROM control register
	unsigned long TRCTL;            //PCI transfer request control register
};

int dma_transfer_wr(int wr, size_t count, struct DM642_card *card)
{
	unsigned int temp;

	//映射DMA需要的缓冲区
	card->ptr = (void *)buf_rd;
	card->dma_size = count;
	card->dma_dir = wr ? DMA_TO_DEVICE : DMA_FROM_DEVICE;

	card->dma_addr = pci_map_single(DM642_demo_card->pci_device, DM642_demo_card->ptr,
				    DM642_demo_card->dma_size, DM642_demo_card->dma_dir);

	//设置设备

	//检查主机是否发出请求
	card->offset = 0x1C00008 % 0x400000;
	iowrite32(0x1C00008/0x400000, card->io + 8);
	temp = ioread32(card->mmio + card->offset);

	if(((temp & 0x8)>>3) == 1)		//HOSTSW
		{
		if(((temp & 0x40)>>6) == 1)	//MASTEROK
			{
			//清除MASTEROK
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x40, card->mmio + card->offset);
			//清除HOSTSW
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x8, card->mmio + card->offset);
			//使能DSP到PPC的中断:HSR[INTAM]
			temp = ioread32(card->io + 0);
			temp &= ~(0x4);
			iowrite32(temp, card->io + 0);
			//产生中断:RSTSCR[INTREQ]
			card->offset = 0x1C00000 % 0x400000;
			iowrite32(0x1C00000/0x400000, card->io + 8);
			temp = ioread32(card->mmio + card->offset);
			temp &= ~(0x8);
			temp |= 0x8;
			iowrite32(temp, card->mmio + card->offset);
			}
		}
	else if(((temp & 0x8)>>3) == 0)		//!HOSTSW
		{
		//清除HOSTSW
		card->offset = 0x1C00008 % 0x400000;
		iowrite32(0x1C00008/0x400000, card->io + 8);
		iowrite32(0x8, card->mmio + card->offset);
		//读取PCIMA
		}

	return 0;
}

static int proc_read_DM642(struct file *filp, const char *buffer, unsigned long count, void *data)
{
}

static int proc_write_DM642(struct file *filp, const char *buffer, unsigned long count, void *data)
{
}

static loff_t fsl_DM642_llseek(struct file *filp, loff_t off, int whence)
{
	struct DM642_card *card = (struct DM642_card *)filp->private_data;
	loff_t ret = 1;
	int high;

	switch(whence)
		{
		case 0x0:		//SEEK_SET
			{
			high = (off/0x400000) & 0x3FF;
			card->offset = (off%0x400000) & 0x3FFFFF;
			iowrite32(high, card->io + 8);
			break;
			}
		case 0x1:		//SEEK_CUR
			{
			break;
			}
		case 0x2:		//SEEK_END
			{
			break;
			}
		default:
			return -EINVAL;
		}
	return ret;
}

*/

#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm-ppc/mpc83xx.h>

#define MODULE_NAME "DM642"
#define DRV_NAME "pcidev"

#define GPIO30 0
#define GPIO31 1

static struct pci_device_id DM642_ids[] __devinitdata = {
	{0x104c, 0x9065, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{0,}
};

struct DM642_card{
	struct pci_dev *pci_device;
	int pci_id;
	int irq;
	//REG
	void __iomem *io;
	void __iomem *mmio;
	unsigned int offset;
	unsigned int read_len;
	//DMA
	unsigned int dsp_ma;
	unsigned int pci_mc;
	struct semaphore sem;
}*DM642_demo_card;

u32 *buf_rd;
//u8 *buf_wr;
void __iomem *base0, *base1, *base2;

MODULE_DEVICE_TABLE(pci, DM642_ids);


void delay(int time)
{
	int i;
	for(i=0; i<time;i++)
		;
}

static irqreturn_t dm642_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("TMS320DM642 init!\n");
	struct DM642_card *card = DM642_demo_card;
	unsigned int temp;

/*	//释放DMA缓冲区的映射
	pci_unmap_single(DM642_demo_card->pci_device, DM642_demo_card->dma_addr,
			 DM642_demo_card->dma_size, DM642_demo_card->dma_dir);
*/

	//关闭中断DSP_INTA:RSTSCR[INTRST]
	card->offset = 0x1C00000 % 0x400000;
	iowrite32(0x1C00000/0x400000, card->io + 8);
	temp = ioread32(card->mmio + card->offset);
	temp &= ~(0x10);
	temp |= 0x10;
	iowrite32(temp, card->mmio + card->offset);

	//屏蔽DSP到PPC的中断:HSR[INTAM]
	temp = ioread32(card->io + 0);
	temp &= ~(0x4);
	temp |= 0x4;
	iowrite32(temp, card->io + 0);

	return IRQ_HANDLED;
}

//only for gpio30 & gpio31
void fsl_gpio_setpin(unsigned int pin, unsigned int to)
{
	void __iomem *gpio;
	unsigned int *temp;

	if((temp = kmalloc(sizeof(unsigned int), GFP_KERNEL)) == NULL)
		{
		printk("fsl_gpio_setpin kmalloc failed.\n");
		return;
		}
	gpio = ioremap(0xE0000C00, 32);
	//GPIO30
	if(pin == 0)
		{
		//GPDIR
		*temp = in_be32(gpio + 0);
		*temp &= ~(0x2);
		*temp |= 0x2;
		out_be32(gpio + 0, *temp);
		//GPODR
		*temp = in_be32(gpio + 4);
		*temp &= ~(0x2);
		out_be32(gpio + 4, *temp);
		//GPDAT
		*temp = in_be32(gpio + 8);
		*temp &= ~(0x2);
		*temp |= (to*2);
		out_be32(gpio + 8, *temp);
		}
	//GPIO31
	else if(pin == 1)
		{
		//GPDIR
		*temp = in_be32(gpio + 0);
		*temp &= ~(0x1);
		*temp |= 0x1;
		out_be32(gpio + 0, *temp);
		//GPODR
		*temp = in_be32(gpio + 4);
		*temp &= ~(0x1);
		out_be32(gpio + 4, *temp);
		//GPDAT
		*temp = in_be32(gpio + 8);
		*temp &= ~(0x1);
		*temp |= to;
		out_be32(gpio + 8, *temp);
		}
	iounmap(gpio);
	kfree(temp);
	return;
}

static int fsl_DM642_open(struct inode *inode, struct file *filp)
{
	filp->private_data = DM642_demo_card;
	return 0;
}

static int fsl_DM642_release(struct inode *inode, struct file *filp)
{
	filp->private_data = NULL;
	return 0;
}

static int fsl_DM642_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	struct DM642_card *card = (struct DM642_card *)filp->private_data;
	int ret = 0;
	unsigned int temp;

	switch(cmd)
		{
		case 0x0:		//HSR
			{
			iowrite32(arg, card->io + 0);
			break;
			}
		case 0x1:		//HDCR
			{
			iowrite32(arg, card->io + 4);
			break;
			}
		case 0x2:		//DSPP
			{
			temp = arg/0x400000;
			card->offset = arg%0x400000;
			iowrite32(temp, card->io + 8);
			break;
			}
		case 0x3:		//DSP_RESET
			{
			//DSP_nRST
			fsl_gpio_setpin(GPIO30,1);
			delay(300);
			fsl_gpio_setpin(GPIO30,0);
			delay(300);
			fsl_gpio_setpin(GPIO30,1);
			delay(300);
			//DSP_TCK
			fsl_gpio_setpin(GPIO31,1);
			delay(30);
			fsl_gpio_setpin(GPIO31,0);
			delay(30);
			fsl_gpio_setpin(GPIO31,1);
			break;
			}
		case 0x4:		//DSP_START
			{
			temp = ioread32(card->io + 4);
			temp &= ~(0x2);
			temp |= 0x2;
			iowrite32(temp, card->io + 4);
			break;
			}
		case 0x5:		//SDRAM_INIT
			{
			iowrite32(0x6, card->io + 8);
			iowrite32(0x00092078, card->mmio + 0);
			iowrite32(0xF3A88E02, card->mmio + 4);
			iowrite32(0xFFFFFFDE, card->mmio + 8);
			iowrite32(0x22a28a22, card->mmio + 0x10);
			iowrite32(0x22a28a42, card->mmio + 0x14);
			iowrite32(0x53115000, card->mmio + 0x18);
			iowrite32(0x0000081b, card->mmio + 0x1c);
			iowrite32(0x000a8529, card->mmio + 0x20);
			iowrite32(0x00000002, card->mmio + 0x44);
			iowrite32(0x00000002, card->mmio + 0x48);
			iowrite32(0x00000002, card->mmio + 0x50);
			iowrite32(0x00000073, card->mmio + 0x54);
			break;
			}
		case 0x6:		//DSP_INTA
			{
			//使能DSP到PPC的中断:HSR[INTAM]
			temp = ioread32(card->io + 0);
			temp &= ~(0x4);
			iowrite32(temp, card->io + 0);
			//产生中断:RSTSCR[INTREQ]
			card->offset = 0x1C00000 % 0x400000;
			iowrite32(0x1C00000/0x400000, card->io + 8);
			temp = ioread32(card->mmio + card->offset);
			temp &= ~(0x8);
			temp |= 0x8;
			iowrite32(temp, card->mmio + card->offset);
			break;
			}
		case 0x7:		//MASTEROK_EN:PCIIEN[MASTEROK]
			{
			card->offset = 0x1C0000C % 0x400000;
			iowrite32(0x1C0000C/0x400000, card->io + 8);
			temp = ioread32(card->mmio + card->offset);
			temp &= ~(0x40);
			temp |= 0x40;
			iowrite32(temp, card->mmio + card->offset);
			break;
			}
		case 0x8:		//MASTEROK_DIS:PCIIEN[MASTEROK]
			{
			card->offset = 0x1C0000C % 0x400000;
			iowrite32(0x1C0000C/0x400000, card->io + 8);
			temp = ioread32(card->mmio + card->offset);
			temp &= ~(0x40);
			iowrite32(temp, card->mmio + card->offset);
			break;
			}
		case 0x9:		//HOSTSW_CLR:PCIIS[HOSTSW]
			{
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x8, card->mmio + card->offset);
			break;
			}
		case 0xA:		//MASTEROK_CLR:PCIIS[MASTEROK]
			{
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x40, card->mmio + card->offset);
			break;
			}
		case 0xB:		//DSPMA
			{
			card->offset = 0x1C00010 % 0x400000;
			iowrite32(0x1C00010/0x400000, card->io + 8);
			iowrite32(arg, card->mmio + card->offset);
			break;
			}
		case 0xC:		//PCIMA
			{
			unsigned long *virtbus = virt_to_bus(buf_rd);
			card->offset = 0x1C00014 % 0x400000;
			iowrite32(0x1C00014/0x400000, card->io + 8);
			iowrite32(virtbus, card->mmio + card->offset);
			break;
			}
		case 0xD:		//PCIMC
			{
			card->offset = 0x1C00018 % 0x400000;
			iowrite32(0x1C00018/0x400000, card->io + 8);
			iowrite32(arg, card->mmio + card->offset);
			break;
			}
		case 0xE:		//FILE_DSPMA
			{
			card->dsp_ma = arg;
			break;
			}
		case 0xF:		//FILE_PCIMC
			{
			card->pci_mc = arg;
			break;
			}
		default:
			return -EINVAL;
		}
	return ret;
}

static ssize_t fsl_DM642_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	struct DM642_card *card = (struct DM642_card *)filp->private_data;
	ssize_t ret = 0;
	int i;

	//不是4字节的倍数，补齐
	if ((count % 4) != 0)
		count += 4 - (count % 4);
	card->read_len = count/4;

	if(down_interruptible(&card->sem))
		{
		printk("fsl_DM642_read down_interruptible failure!\n");
		return -ERESTARTSYS;	
		}

	//从SDRAM读数据，分别设置DSPMA、PCIMA、PCIMC
	card->offset = 0x1C00010 % 0x400000;
	iowrite32(0x1C00010/0x400000, card->io + 8);
	iowrite32(card->dsp_ma, card->mmio + card->offset);

	unsigned long *virtbus = virt_to_bus(buf_rd);
	card->offset = 0x1C00014 % 0x400000;
	iowrite32(0x1C00014/0x400000, card->io + 8);
	iowrite32(virtbus, card->mmio + card->offset);

	card->offset = 0x1C00018 % 0x400000;
	iowrite32(0x1C00018/0x400000, card->io + 8);
	card->pci_mc &= ~(0x7);
	card->pci_mc |= 0x1;
	iowrite32(card->pci_mc, card->mmio + card->offset);

	//轮询是否DMA传输完毕
	for(;;)
		{
		card->offset = 0x1C00008%0x400000;
		iowrite32(0x1C00008/0x400000, card->io + 8);
		if((((ioread32(card->mmio + card->offset)) & 0x40)>>6) == 1)
			{
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x40, card->mmio + card->offset);
			break;
			}
		}

/*	for(i=0;i<card->read_len;i++)
		{
		buf_rd[i] = ioread32(card->mmio + card->offset + 4*i);
		mb();
		}
*/
	if(copy_to_user(buf, buf_rd, count))
		{
		printk("copy_to_user fault!\n");
		return -EFAULT;
		}

	up(&card->sem);
	ret = count;
	return ret;
}

static ssize_t fsl_DM642_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	struct DM642_card *card = (struct DM642_card *)filp->private_data;
	ssize_t ret = 0;
	int i;

	//不是4字节的倍数，补齐
	if ((count % 4) != 0)
		count += 4 - (count % 4);
	card->read_len = count/4;

	if(down_interruptible(&card->sem))
		{
		printk("fsl_DM642_write down_interruptible failure!\n");
		return -ERESTARTSYS;	
		}
	if((copy_from_user(buf_rd, buf, count)))
		{
		printk("copy_from_user fault!\n");
		ret = -EFAULT;
		goto out;
		}

	//往SDRAM写数据，分别设置DSPMA、PCIMA、PCIMC
	card->offset = 0x1C00010 % 0x400000;
	iowrite32(0x1C00010/0x400000, card->io + 8);
	iowrite32(card->dsp_ma, card->mmio + card->offset);

	unsigned long *virtbus = virt_to_bus(buf_rd);
	card->offset = 0x1C00014 % 0x400000;
	iowrite32(0x1C00014/0x400000, card->io + 8);
	iowrite32(virtbus, card->mmio + card->offset);

	card->offset = 0x1C00018 % 0x400000;
	iowrite32(0x1C00018/0x400000, card->io + 8);
	card->pci_mc &= ~(0x7);
	card->pci_mc |= 0x2;
	iowrite32(card->pci_mc, card->mmio + card->offset);

	//轮询是否DMA传输完毕
	for(;;)
		{
		card->offset = 0x1C00008%0x400000;
		iowrite32(0x1C00008/0x400000, card->io + 8);
		if((((ioread32(card->mmio + card->offset)) & 0x40)>>6) == 1)
			{
			card->offset = 0x1C00008 % 0x400000;
			iowrite32(0x1C00008/0x400000, card->io + 8);
			iowrite32(0x40, card->mmio + card->offset);
			break;
			}
		}

/*	for(i=0;i<card->read_len;i++)
		{
		iowrite32(buf_rd[i], card->mmio + card->offset + 4*i);
		mb();
		}
*/
	ret = count;
out:
	up(&card->sem);
	return ret;
}

static struct file_operations DM642_fops = {
	.owner =	THIS_MODULE,
	//.llseek  = fsl_DM642_llseek,
	.ioctl = fsl_DM642_ioctl,
	.read  = fsl_DM642_read,
	.write = fsl_DM642_write,
	.open  = fsl_DM642_open,
	.release = fsl_DM642_release,
};

static int __devinit DM642_probe(struct pci_dev *pci, const struct pci_device_id *pci_id)
{
	struct proc_dir_entry *proc_dir, *dm;
	unsigned long int base0_start, base0_end, base0_len, base0_flags,
			  base1_start, base1_end, base1_len, base1_flags,
			  base2_start, base2_end, base2_len, base2_flags;

	//u16 vid, did, cmd, sta;
	//unsigned int base00, base10, base20;

	//3.1启动PCI设备
	if(pci_enable_device(pci))
		return -EIO;
	pci_set_master(pci);

	//3.2申请MMIO/IOP资源, to prevent two devices colliding on the same address range
	if(pci_request_regions(pci, "DM642"))
		{
		pci_disable_device(pci);
		return -ENODEV;
		}

	//3.3设备DMA标识
	if(pci_set_dma_mask(pci, DMA_32BIT_MASK))
		{
		printk(KERN_WARNING "pci_set_dma_mask failed\n");
		pci_release_region(pci, 0);
		return -ENODEV;
		}

	//3.6申请IRQ
	if(request_irq(pci->irq, dm642_interrupt, SA_INTERRUPT, "DM642-PCI-init", pci))
		{
		printk(KERN_ERR "IRQ%d already in use\n", pci->irq);
		free_irq(pci->irq, pci);
		pci_release_region(pci, 0);
		pci_disable_device(pci);
		return -ENODEV;
		}

	//在内核空间中动态申请内存
	if((DM642_demo_card = kmalloc(sizeof(struct DM642_card), GFP_KERNEL)) == NULL)
		{
		printk(KERN_ERR "DM642_demo_card: out of memory\n");
		return -ENOMEM;
		}
	memset(DM642_demo_card, 0, sizeof(*DM642_demo_card));

	//开辟读写空间
	buf_rd = (u32 *)__get_free_pages(GFP_KERNEL, 9);
	if(buf_rd == NULL)
		{
		printk("buf_rd get_free_pages failed!\n");
		return 0;
		}

	//获取映射信息
	base0_start = pci_resource_start(pci, 0);
	base0_end = pci_resource_end(pci, 0);
	base0_len = pci_resource_len(pci, 0);
	base0_flags = pci_resource_flags(pci, 0);
/*	printk("base0_start = %lx\n",base0_start);
	printk("base0_end = %lx\n",base0_end);
	printk("base0_len = %lx\n",base0_len);
	printk("base0_flags = %lx\n\n",base0_flags);
*/
	base1_start = pci_resource_start(pci, 1);
	base1_end = pci_resource_end(pci, 1);
	base1_len = pci_resource_len(pci, 1);
	base1_flags = pci_resource_flags(pci, 1);
/*	printk("base1_start = %lx\n",base1_start);
	printk("base1_end = %lx\n",base1_end);
	printk("base1_len = %lx\n",base1_len);
	printk("base1_flags = %lx\n\n",base1_flags);
*/
	base2_start = pci_resource_start(pci, 2);
	base2_end = pci_resource_end(pci, 2);
	base2_len = pci_resource_len(pci, 2);
	base2_flags = pci_resource_flags(pci, 2);
/*	printk("base2_start = %lx\n",base2_start);
	printk("base2_end = %lx\n",base2_end);
	printk("base2_len = %lx\n",base2_len);
	printk("base2_flags = %lx\n\n",base2_flags);
*/
	//4MB addr, prefetchable memory
	base0 = ioremap(base0_start, base0_len);
	//printk("mapped base0 4M prefetchable = %x\n",base0);
	if(!base0)
		{
		printk("Cannot remap prefetchable memory, ABORTING\n");
		pci_disable_device(pci);
		return -EIO;
		}

	//8MB addr, non-prefetchable memory
	base1 = ioremap(base1_start, base1_len);
	//printk("mapped base1 8M non-prefetchable = %x\n",base1);
	if(!base1)
		{
		printk("Cannot remap non-prefetchable memory, ABORTING\n");
		pci_disable_device(pci);
		return -EIO;
		}

	//4 words IO
	base2 = ioport_map(base2_start, base2_len);
	//printk("mapped base2 I/O = %x\n",base2);
	if(!base2)
		{
		printk("Cannot remap IO, ABORTING\n");
		pci_disable_device(pci);
		return -EIO;
		}

	//读取配置信息
	DM642_demo_card->pci_device = pci;
	DM642_demo_card->pci_id = pci_id->device;
	DM642_demo_card->irq = pci->irq;
	DM642_demo_card->io = base2;
	DM642_demo_card->mmio = base0;
	DM642_demo_card->offset = 0;
	DM642_demo_card->read_len = 0;
	DM642_demo_card->dsp_ma = 0;
	DM642_demo_card->pci_mc = 0;

	//初始化信号向量
	init_MUTEX(&DM642_demo_card->sem);

	//访问寄存器
	//Configuration Registers, accessed by pci_read_config_word().
/*	printk("\nConfiguration Registers: \n");
	pci_read_config_word(pci, 0x0, &vid);
	pci_read_config_word(pci, 0x2, &did);
	pci_read_config_word(pci, 0x4, &cmd);
	pci_read_config_word(pci, 0x6, &sta);
	pci_read_config_dword(pci, 0x10, &base00);
	pci_read_config_dword(pci, 0x14, &base10);
	pci_read_config_dword(pci, 0x18, &base20);
	printk("vid = %x\n",vid);
	printk("did = %x\n",did);
	printk("cmd = %x\n",cmd);
	printk("sta = %x\n",sta);
	printk("BAR0 = %x\n",base00);
	printk("BAR1 = %x\n",base10);
	printk("BAR2 = %x\n",base20);

	//IO Registers, accessed by ioread32(base2 + offset) & iowrite32(val, base2 + offset)
	//访问io内存：readl和writel为早期版本，ioread32和iowrite32近期版本。
	printk("\nIO Registers: \n");
	printk("HSR = %x\n",ioread32(base2 + 0));
	printk("HDCR = %x\n",ioread32(base2 + 4));
	printk("DSPP = %x\n",ioread32(base2 + 8));
	//Memory-Mapped Registers, accessed by DSPP=7, ioread(base0 + offset), only used when pci master enabled
	printk("\nMemory-Mapped Registers: \n");
	//Memory, accessed by DSPP=0x200-0x20F, ioread32(base0 + offset) & iowrite32(val, base0 + offset)
	printk("\nMemory:\n");
*/
	//创建proc文件
	proc_dir = proc_mkdir("pcidev", NULL);
	if(proc_dir == NULL)
		return -ENOMEM;
	dm = create_proc_entry("DM642", 0644, proc_dir);
	if(dm == NULL)
		return -ENOMEM;

	dm->proc_fops = &DM642_fops;
	dm->owner = THIS_MODULE;
	//dm->read_proc = proc_read_DM642;
	//dm->write_proc = proc_write_DM642;

	printk("DM642 PCI initialized \n");
	return 0;
}

static void __devexit DM642_remove(struct pci_dev *pci)
{
	//4.2释放IRQ
	free_irq(pci->irq, pci);

	//4.3 Stop all DMA activity

	//4.4 Release DMA buffers
	free_pages((unsigned long)buf_rd,9);
	kfree(DM642_demo_card);

	//4.6 Disable Device from responding to MMIO/IO Port addresses
	iounmap(base0);
	iounmap(base1);
	ioport_unmap(base2);
	pci_disable_device (pci);

	//4.7 Release MMIO/IO Port Resource(s)
	pci_release_regions(pci);

	remove_proc_entry("DM642", NULL);
	printk(KERN_INFO "%s removed\n", MODULE_NAME);
}

static struct pci_driver driver = {
	.name = "DM642",
	.id_table = DM642_ids,
	.probe = DM642_probe,
	.remove = __devexit_p(DM642_remove),
};

static int __init DM642_init(void)
{
	return pci_register_driver(&driver);
}

static void __exit DM642_exit(void)
{
	pci_unregister_driver(&driver);
}

module_init(DM642_init)
module_exit(DM642_exit)
