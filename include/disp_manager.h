#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

#include "font_manager.h"
#include "common.h"



typedef struct DispBuff {
	int iXres;
	int iYres;
	int iBpp;
	char* buff;
}DispBuff,* PDispBuff;



typedef struct DispOpr {
	char* name;
	int (*DeviceInit)(void);//设备初始化函数
	int (*DeviceExit)(void);//设备退出函数
	int (*GetBuffer)(PDispBuff ptDispBuff);//分辨率和色深（每个像素多少位）
	int (*FlushRegion)(PRegion ptRegion, PDispBuff ptDispBuff);  // 刷新区域,区域信息和数据缓冲区
	struct DispOpr* pNext;// 指向下一个显示设备操作接口
}DispOpr,* PDispOpr;

void RegisterDisplay(PDispOpr ptDispOpr);//注册显示设备

void DisplaySystemRegister(void);//显示设备注册
int SelectDefaultDisplay(char* name);//选择默认显示设备
int InitDefaultDisplay(void);//初始化默认显示设备
int PutPixel(int x, int y, unsigned int dwColor);//在指定位置上输出指定颜色（描点）
int FlushDisplayRegion(PRegion ptRegion, PDispBuff ptDispBuff);//刷新显示区域
PDispBuff GetDispBuffer(void);//获取显示缓冲区信息
void DrawFontBitMap(PFontBitMap ptFontBitMap, unsigned int dwColor);
void DrawRegion(PRegion ptRegion, unsigned int dwColor);
void DrawTextInRegionCentral(char* name, PRegion ptRegion, unsigned int dwColor);

#endif