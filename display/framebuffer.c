
#include "disp_manager.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

static int fd_fb;
static struct fb_var_screeninfo var;	/* Current var */
static int screen_size;
static unsigned char* fb_base;
static unsigned int line_width;
static unsigned int pixel_width;

static int FbDeviceInit(void)//设备初始化，打开framebuffer设备，映射内存，获取分辨率等信息
{
	fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0)
	{
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can't get var\n");
		return -1;
	}

	line_width = var.xres * var.bits_per_pixel / 8;//一行的长度等于分辨率X乘以每个像素的字节数
	pixel_width = var.bits_per_pixel / 8;//一个像素的字节数等于色深除以8
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;//屏幕大小等于分辨率X乘以分辨率Y乘以每个像素的字节数
	fb_base = (unsigned char*)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);//映射内存，PROT_READ|PROT_WRITE是可读可写，MAP_SHARED是共享映射
	if (fb_base == (unsigned char*)-1)
	{
		printf("can't mmap\n");
		return -1;
	}
	memset(fb_base, 0, screen_size);
	return 0;
}

static int FbDeviceExit(void)//设备退出，取消映射内存，关闭framebuffer设备
{
	munmap(fb_base, screen_size);
	close(fd_fb);
	return 0;
}

/*可以返回LCD的framebuffer，以后上层APP可以直接操作LCD，不用FlushRegion
也可以malloc返回一块无关的buffer，要使用FbFlushRegion
*/
static int FbGetBuffer(PDispBuff ptDispBuff)//分辨率和色深（每个像素多少位）
{
	ptDispBuff->iXres = var.xres;//分辨率X
	ptDispBuff->iYres = var.yres;//分辨率Y
	ptDispBuff->iBpp = var.bits_per_pixel;//色深
	ptDispBuff->buff=(char*)fb_base;//返回framebuffer地址
	return 0;
}
static int FbFlushRegion(PRegion ptRegion, PDispBuff ptDispBuff)  // 刷新区域,区域信息和数据缓冲区
{
	return 0;
}

static DispOpr g_tFrameBufferOpr = {
.name="fb",
.DeviceInit=FbDeviceInit,
.DeviceExit = FbDeviceExit,
.GetBuffer=FbGetBuffer,
.FlushRegion=FbFlushRegion,
};

void FrameBufferRegister(void)//注册framebuffer显示设备
{
	RegisterDisplay(&g_tFrameBufferOpr);
}




