#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char abc[]="abcdefghijklmnopqrstuvwxyz";
char ac[27];
int main(int argc,char *argv[])
{
        int fh,fb,fadc;
        int i;
	unsigned long framebuf[153600];
        fadc = open("/dev/adc", O_RDWR, S_IRUSR | S_IWUSR);
	if (fadc != -1 )
	{
		read(fadc, &i, sizeof(char)*2);
		i=i & 0x3ff;
		printf("read value is 0x%x\n",i);
		printf("voltage value is %5.3f\n",3.3*i/1024);
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
		for (i=0;i<10;i++)
		{
			lseek(fh,0x80000000+i*0xf000,0);
                	write(fh,framebuf+i*0x3c00,0xf000);
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
