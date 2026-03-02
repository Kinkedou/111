#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string.h>
#include "disp_manager.h"
#include <stdlib.h>
#include "font_manager.h"
#include "ui.h"


int main(int argc, char** argv)
{

	PDispBuff ptBuffer;
	int error;
	Button tButton;
	Region tRegion;

	if (argc != 2)
	{
		printf("Usage: %s <font_file>\n", argv[0]);
		return -1;
	}
	DisplayInit();//注册显示设备fb，放到显示设备链表头
	SelectDefaultDisplay("fb");//链表里找到fb设备，并设置为默认显示设备
	InitDefaultDisplay();//创建disbuffer，打开fb，获得基地址和渲染区域大小，填充disbuffer
	ptBuffer = GetDispBuffer();//得到前一步的disbuffer，放入本地变量ptBuffer(里面包含显示器的分辨率与色深)

	FontsRegister();//注册字体freetype到字体链表头
	error = SelectAndInitFont("freetype", argv[1]);//选择freetype字体，并初始化freetype库，加载字体文件
	if (error)
	{
		printf("SelectAndInitFont err\n");
		return -1;
	}
	tRegion.iLeftUpX = 200;
	tRegion.iLeftUpY = 100;
	tRegion.iWidth = 300;
	tRegion.iHeight = 100;
	InitButton(&tButton, "test", &tRegion,NULL,NULL);
	tButton.OnDraw(&tButton, ptBuffer);
	while (1)
	{
		tButton.OnPressed(&tButton, ptBuffer,NULL);
		sleep(2);
	}


	return 0;
}

