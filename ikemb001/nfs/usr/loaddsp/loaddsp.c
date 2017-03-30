#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct arch_section
{
        unsigned long size;
	unsigned long pointer;
	unsigned long * buf;
};

static struct stat file_out;
static int fp,fh;
static struct arch_section load[40];
static int seccount;
static off_t filesize; 
static unsigned long entry;
/*const unsigned long memcon[12]={
	0x00092078,
	0xF3A88E02,
	0xFFFFFFDE,
	
	0x22a28a22,
	0x22a28a42,
	0x57115000,
	0x0000081b,
	0x000a8529,
	
	0x00000002,
	0x00000002,
	
	0x00000000,
	0x00000000
};*/

static unsigned int memcon[12] = {0x00092078,        // GCTL     - 0x01800000
	0xF3A88E02,        // CE1      - 0x01800004
	0xFFFFFFDE,        // CE0      - 0x01800008
	// Address split 3
	0x22a28a22,        // CE2      - 0x01800010
	0x22a28a42,        // CE3      - 0x01800014
	0x57115000,        // SDRAMCTL - 0x01800018
	0x0000081b,        // SDRAMTIM - 0x0180001c
	0x000a8529,        // SDRAMEXT - 0x01800020
	// Address split 9
	0x00000002,        // CE1ECCTL - 0x01800044
	0x00000002,        // CE0ECCTL - 0x01800048
	// Address split 
	0x00000002,        // CE2ECCTL - 0x01800050
	0x00000073         // CE3ECCTL - 0x01800054
};

/*const unsigned long bootloader[16]={
        0x00800028,
        0x0080c468,
        0x0080002a,
        0x0080006a,
        0x00840276,
        0x01008028,
        0x01000068,
        0x01080266,
        0x00080362,
        0x00008000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
};*/
void dump(unsigned int addr)
{
	unsigned int i, j;
	lseek(fh,addr,0);
	for (i=0;i<4;i++)
	{
		read(fh,&j,4);
		printf("mem %p is %p\n",addr + i * 4, j);
	}
}

int main(int argc,char *argv[])
{
        unsigned int i,j,k;
	unsigned long temp,tem;

	printf("--------------------loading dm642 program ---------------------\n");
	fp =open("/usr/loaddsp/app.bin", O_RDWR, S_IRUSR | S_IWUSR);
	fh=open("/dev/hpi", O_RDWR, S_IRUSR | S_IWUSR);
        if (fp != -1 )
        {
		filesize=fstat(fp,&file_out);
		printf("opened framework.bin size %d\n",file_out.st_size);
        }
        else
              printf(".bin File open failure \n");
        if (fh == -1 )
               printf("Device open failure \n");

ioctl(fh, 6 ,i);
	
	lseek(fh, 0x01800000, 0);
	write(fh, &memcon[0], 4*3);
	lseek(fh, 0x01800010, 0);
	write(fh, &memcon[3], 4*5);
	lseek(fh, 0x01800048, 0);
	write(fh, &memcon[8], 4*2);
	lseek(fh, 0x01800050, 0);
	write(fh, &memcon[10], 4*2);

	
dump(0x01800000);
	
	lseek(fp,0,0);
	read(fp,&entry,4);
	i=4;
	j=0;
	seccount=0;

        while(1)
	 {
	        if (i>=file_out.st_size)
			break;
	        lseek(fp,i,0);
		read(fp,&(load[j].size),4);
		i=i+4;
       	 	lseek(fp,i,0);
		read(fp,&(load[j].pointer),4);
		i=i+4;
		if (load[j].size%4!=0)
			load[j].size=load[j].size+4-(load[j].size%4);
		printf("--------------------------------------------\n");
		printf("section size 0x%x\n",(int)load[j].size);
		printf("dm642 load address 0x%x\n",load[j].pointer);
                for(k=0;k<load[j].size;)
                {
			lseek(fp,i,4);
			read(fp,&tem,4);
                	lseek(fh,load[j].pointer+k,0);
                        write(fh,&tem,4);
			lseek(fh,load[j].pointer+k,0);
			read(fh,&temp,4);
			if (temp==tem)
			{
				i=i+4;
				k=k+4;
			}
			else
				printf("hpi load error %d---0x%x---0x%x\n",j,temp,*(load[j].buf+k));
                }
		seccount++;
		j++;
	 }
	dump(0x01800000);
	dump(0x01800010);
	dump(0x01800048);
	dump(0x01800050);
	lseek(fh,20,0);
	read(fh,&temp,4);
	read(fh,&tem,4);
	printf("program entry is 0x%x\n",entry);
	printf("-----0x%x-----0x%x\n",temp,tem);
	temp=(temp & 0xff80007f)|(((entry &0x0000ffff)<<7) & 0x7fff80);
	tem=(tem & 0xff80007f)|(((entry &0xffff0000)>>9) & 0x7fff80);
        lseek(fh,20,0);
	write(fh,&temp,4);
        write(fh,&tem,4);
	printf("-----0x%x-----0x%x\n",temp,tem);
        printf("%d sections loaded\n",seccount);
	//lseek(fh,0x01880000,0);
	//write(fh,0x00020002,4);
	//lseek(fh,0x01880000,0);
	  //      read(fh,&temp,4);
	//	        printf("hpi ctl is 0x%x\n",temp);
	printf("start the program\n");
	ioctl(fh, 7 ,i);
	close(fp);
	close(fh);
        return (0);
	

	//lseek(fh, 0x01800000, 0);
	//write(fh, &memcon[0], 3 * 4);

	//lseek(fh, 0x01800010, 0);
	//write(fh, &memcon[3], 5 * 4);

	//lseek(fh, 0x01800048, 0);
	//write(fh, &memcon[8], 2 * 4);

	//lseek(fh, 0x01800050, 0);
	//write(fh, &memcon[10], 2 * 4);

	
	//dump(0x0);
	//dump(0x01800000);
	//dump(0x80000000);

	//lseek(fh,0,0);
        //lseek(fh,0x01800000,0);
	//for (i=0;i<4;i++)
	//{
	//	read(fh,&j,4);
	//	printf("12312 memcon is %p\n",j);
	//}
	/*usleep(1000);

	{
		printf("mem test\n");
		unsigned int i;
		lseek(fh, 0x80000000, 0);
		for (i = 0x80000000; i<0x82000000; i += 4)
		{
			write(fh, &i, 4);
		}

		
		lseek(fh, 0x80000000, 0);
		for (i = 0x80000000; i<0x82000000; i += 4)
		{
			unsigned int j;
			read(fh, &j, 4);
			if (i != j)
				printf("error at %08x\n", i);
		}
	}*/
	/*for (i=0;i<12;i++)
		printf("------------------------0x%x\n",memcon[i]);
        if (fh != -1 )
        {
		printf("copy program to sdram\n");
	        lseek(fh, 0x01800000, 0);
		write(fh, &memcon[0], 4*3);
		lseek(fh, 0x01800010, 0);
		write(fh, &memcon[3], 4*5);
		lseek(fh, 0x01800048, 0);
		write(fh, &memcon[8], 4*2);
		lseek(fh, 0x01800050, 0);
		write(fh, &memcon[10], 4*2);
        	for (i=0;i<seccount;i++)
        	{
        	     for (j=0;j<load[i].size;)
		     {
			lseek(fh,load[i].pointer+j,0);
                        write(fh,load[i].buf+j,4);
			lseek(fh,load[i].pointer+j,0);
			read(fh,&temp,4);
			if (temp==*(load[i].buf+j))
				j=j+4;
			else
			   printf("hpi load error %d---0x%x---0x%x\n",j,temp,*(load[i].buf+j));
		     }
        	}
        }
        else
         {
               printf("Device open failure \n");
        }*/
}

