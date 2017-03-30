#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*
static FILE * fpp,*fp;
static int ff;
static struct stat file_out;
*/

int make_file(const char *src_file, const char *dst_file)
{
	char sz_line[1024];
	FILE *fpSrc = fopen(src_file, "r");
	FILE *fpDst = fopen(dst_file, "w");
	if (!fpSrc || !fpDst)
	{
		fclose(fpSrc);
		fclose(fpDst);
		return -1;
	}
	
	while(!feof(fpSrc))
	{
		char c= fgetc(fpSrc);
		if (c == ' ' || c == '\r' || c == '\n')
			continue;
		else if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
		{
			unsigned int val;
			unsigned char chr;
			sz_line[0] = c;
			sz_line[1] = fgetc(fpSrc);
			sz_line[2] = '\0';
			sscanf(sz_line, "%02X", &val);
			//printf("%s\t%X\n", sz_line, val);
			chr = val;
			fwrite(&chr, 1, 1, fpDst);
		}
		else
		{
				while(c != '\n' && !feof(fpSrc))
				{
					c = fgetc(fpSrc);
				}
		}
	}
	
	fclose(fpSrc);
	fclose(fpDst);
	return 0;
}
int main(int argc, char *argv[])
{
	make_file(argv[1], argv[2]);
}
/*
{
        int i,j,k;
	 char temp;
	 ff=creat("/root/arm/nfs/usr/loaddsp/app.bin",O_RDWR | O_CREAT | O_TRUNC);
         ff=open("/root/arm/nfs/usr/loaddsp/app.hex",O_RDWR, S_IRUSR | S_IWUSR);
	 i=fstat(ff,&file_out);
	 printf("hex file size %d\n",file_out.st_size);
	 close(ff);
	 fp =fopen("/root/arm/nfs/usr/loaddsp/app.bin", "r+");
	 fpp=fopen("/root/arm/nfs/usr/loaddsp/app.hex", "r");
	for (i=0;i<=file_out.st_size/3;i++)
	{
		fscanf(fpp,"%x",&temp);
		printf("%c ",temp);
		putc(temp,fp);
        }
}
*/
