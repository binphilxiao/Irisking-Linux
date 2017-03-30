#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char abc[]="abcdefghijklmnopqrstuvwxyz";
char ac[27];
static char *b1,*b2,*b3,*b4;
int main(int argc,char *argv[])
{
        int fh,fb,fadc;
        int i,j;
	unsigned long framebuf[153600];
        fadc = open("/dev/adc", O_RDWR, S_IRUSR | S_IWUSR);
	if (fadc != -1 )
	{
		read(fadc, &i, sizeof(char)*2);
		printf("first byte is %d \n",i);
		close(fadc);
	}
	else
	{
		printf("Device open failure \n");
	}
	fb =open("/dev/fb/0",O_RDWR, S_IRUSR | S_IWUSR);
	printf("Framebuffer test program\n");
        if (fb != -1 )
	{
		lseek(fb,0,0);
		printf("Read data from framebuffer\n");
		read(fb,framebuf,614400);
		close(fb);
	}
        else
        {
                printf("File open failure \n");
        }
	b1=malloc(0xf000);
	b2=malloc(0xf000);
	b3=malloc(0xf000);
	b4=malloc(0xf000);
	for (i=0;i<0xf000;i++)
	{
		*(b1+i)=0x00;
		*(b2+i)=0x55;
		*(b3+i)=0xff;
		*(b4+i)=0xaa;
	}
	fh=open("/dev/hpi", O_RDWR, S_IRUSR | S_IWUSR);
        if (fh != -1 )
        {
		//lseek(fh,0x0000000,0);
                //printf("Write abc to dm642");
               // write(fh,abc,26);
		//puts("\n");
		//lseek(fh,0x0000000,0);
		//read(fh,ac,26);
		//ac[26] = 0;
		//printf("%s\n",ac);
		printf("Write data to dm642\n");
		while(1)
		{
			for (i=0;i<10;i++)
			{
				lseek(fh,0x80000000+i*0xf000,0);
				if (j%4==0)
         	        		write(fh,b1,0xf000);
				if (j%4==1)
   	                                write(fh,b2,0xf000);
				if (j%4==2)
					write(fh,b3,0xf000);
				if (j%4==3)
					write(fh,b4,0xf000);
			}
			j++;
		}
		//lseek(fh,0x80000000+0x90000,0);
		//write(fh,framebuf+i*0x24000,0x6000);
		//lseek(fh,0x80000000+0x90000,0);
		//write(fh,framebuf+i*0x24000,0x6000);
		close(fh);
        }
        else
        {
                printf("Device open failure \n");
        }
        return (0);
}
