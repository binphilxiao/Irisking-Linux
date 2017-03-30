#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
	//FILE * fd, *fh;
	int i,fd,fh;
        unsigned long  cc;
	char num[26];
	char abc[]="abcdefghijklmnopqrstuvwxyz";
//	fh = open("/dev/hpi", O_RDWR, S_IRUSR | S_IWUSR);
	fd = open("/dev/adc", O_RDWR, S_IRUSR | S_IWUSR);
/*	if (fh != -1 )
	{
	       if (ioctl(fh, 0 ,cc) < 0)
               {
	                printf("ioctl ok!\n");
	       }
		write(fh,abc, sizeof(char)*27);
		read(fh, num, sizeof(char)*27);
		printf("buffer is %s \n",num);
	}
	else
	{
		printf("Device open failure \n");
	}*/
        if (fd != -1 )
	{
                read(fd, &num, sizeof(char));
                printf("first byte is %d \n",num);
                close(fd);
        }
        else
        {
                printf("Device open failure \n");
        }
	return (0);
}
