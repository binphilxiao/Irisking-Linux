#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int MiniGUIMain (int argc,const char * argv[])
{
       int fh,fb;
       int i;
       unsigned long framebuf[153600];
  
       MessageBox(HWND_DESKTOP,"hello MiniGUI!","Hello",MB_OK);  
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
	       lseek(fh,0x80000000,0);
	       printf("Write data to dm642");
	       write(fh,framebuf,sizeof(char)*614400);
	       close(fh);
       }
       else
       {
	       printf("Device open failure \n");
       }
       return (0);
}
#include <minigui/dti.c>
