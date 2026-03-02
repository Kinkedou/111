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
#include "input_manager.h"
#include "page_manager.h"



int main(int argc, char** argv)
{

	//PDispBuff ptBuffer;
	int error;
	
	if(argc!=2)
	{
		printf("Usage: %s <font_file>\n", argv[0]);
	return -1;
	}

	DisplaySystemRegister();//注册显示设备fb，放到显示设备链表头
	SelectDefaultDisplay("fb");//链表里找到fb设备，并设置为默认显示设备
	InitDefaultDisplay();//创建disbuffer，打开fb，获得基地址和渲染区域大小，填充disbuffer
	//ptBuffer = GetDispBuffer();//得到前一步的disbuffer，放入本地变量ptBuffer(里面包含显示器的分辨率与色深)
	InputSystemRegister();//注册输入设备触摸屏和网络输入，放到输入设备链表头
	InputDeviceInit();//为所有输入设备创建线程
	FontSystemRegister();//注册字体freetype到字体链表头
	error = SelectAndInitFont("freetype", argv[1]);//选择freetype字体，并初始化freetype库，加载字体文件
	if (error)
	{
		printf("SelectAndInitFont err\n");
		return -1;
	}
	PageSystemRegister();//注册页面main到页面链表头
	Page("main")->Run(NULL);//选择main页面并运行对应的run函数


	return 0;
}

