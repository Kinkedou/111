
#include "ui.h"
#include "page_manager.h"
#include "config.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define X_GAP 5
#define Y_GAP 5

static Button g_tButtons[ITEMCFG_MAX_NUM];
static int g_tButtonCnt;

static int MainPageOnPressed(struct Button* ptButton, PDispBuff ptDispBuff, PInputEvent ptInputEvent)
{
	unsigned int dwColor = BUTTON_DEFAULT_COLOR;
	char name[100];
	char status[100];
	char* strButton=ptButton->name;
	char* command_status[3] = {"error","ok","percent" };
	int command_status_index = 0;
	char command[1000];
	PItemCfg ptItemCfg;

	if (ptInputEvent->iType == INPUT_TYPE_TOUCH)
	{
		if (GetItemCfgByName(ptButton->name)->bCanBeTouched == 0)
			return -1;
		ptButton->status = !ptButton->status;
		if (ptButton->status)
		{
			dwColor = BUTTON_PRESSED_COLOR;
			command_status_index = 1;
		}
	}
	else if(ptInputEvent->iType == INPUT_TYPE_NET)
	{
		sscanf(ptInputEvent->str, "%s %s", name,status);
		if (strcmp(status, "ok") == 0)
		{
			dwColor = BUTTON_PRESSED_COLOR;
			command_status_index = 1;
		}
		else if (strcmp(status, "error") == 0)
		{
			dwColor = BUTTON_DEFAULT_COLOR;
			command_status_index = 0;
		}
		else if (status[0] >= '0' && status[0] <= '9')
		{
			dwColor = BUTTON_PERCENT_COLOR;
			strButton = status;
			command_status_index = 2;
		}

		else
			return -1;

	}
	else
		return -1;
	
	DrawRegion(&ptButton->tRegion, dwColor);
	if(dwColor!= BUTTON_PRESSED_COLOR)
		DrawTextInRegionCentral(strButton, &ptButton->tRegion, BUTTON_TEXT_COLOR);
	else
		DrawTextInRegionCentral(strButton, &ptButton->tRegion, BUTTON_TEXTED_COLOR);
	FlushDisplayRegion(&ptButton->tRegion, ptDispBuff);

	ptItemCfg=GetItemCfgByName(ptButton->name);
	if(ptItemCfg->command[0]!='\0')
	{
		sprintf(command, "%s %s",ptItemCfg->command, command_status[command_status_index]);
		system(command);
	}
	return 0;
}

static int GetFontSizeForAllButton(void)
{
	int i;
	int max_len = -1;
	int max_index = -1;
	int len;
	RegionCartesian tRegionCar;
	float k, kx, ky;

	for (i = 0; i < g_tButtonCnt; i++)
	{
		len = strlen(g_tButtons[i].name);
		if (len > max_len)
		{
			max_len = len;
			max_index = i;
		}
	}
	SetFontSize(100);
	GetStringRegionCar(g_tButtons[max_index].name,&tRegionCar);

	kx = g_tButtons[max_index].tRegion.iWidth * 1.0 / tRegionCar.iWidth;
	ky = g_tButtons[max_index].tRegion.iHeight * 1.0 / tRegionCar.iHeight;
	k = (kx < ky) ? kx : ky;
	return k * 100*0.8;
}

static void GenerateButtons()
{
	int width, height, n_per_line, n, xres, yres;
	int start_x, start_y;
	int pre_start_x, pre_start_y;
	int row, rows;
	int col;
	int i=0;
	int iFontSize;
	PButton pButton;
	PDispBuff pDispBuff;
	g_tButtonCnt= n = GetItemCfgCount();
	pDispBuff= GetDispBuffer();
	xres = pDispBuff->iXres;
	yres = pDispBuff->iYres;
	width = sqrt(1.0 / 0.618 * xres * yres / n);
	n_per_line = xres / width+1;
	width = xres / n_per_line;
	height = width * 0.618;

	start_x = (xres - n_per_line * width) / 2;
	rows = n / n_per_line;
	if (rows * n_per_line < n)
		rows++;
	start_y=(yres-rows*height)/2;
	for (row = 0; (row < rows) && (i < n); row++)
	{
		pre_start_y = start_y + row * height;
		pre_start_x = start_x - width;
		for (col = 0; (col < n_per_line)&&(i<n); col++)
		{
			pButton = &g_tButtons[i];
			pButton->tRegion.iLeftUpX = pre_start_x + width;
			pButton->tRegion.iLeftUpY = pre_start_y;
			pButton->tRegion.iWidth = width-X_GAP;
			pButton->tRegion.iHeight = height - Y_GAP;
			pre_start_x = pButton->tRegion.iLeftUpX;

			InitButton(pButton, GetItemCfgByIndex(i)->name,NULL,NULL, MainPageOnPressed);
			i++;
		}
	}

	iFontSize = GetFontSizeForAllButton();
	//SetFontSize(iFontSize);
	for (i = 0; i < n; i++)
	{
		g_tButtons[i].iFontSize = iFontSize;
		g_tButtons[i].OnDraw(&g_tButtons[i],pDispBuff);
	}
	
}

static int isTouchPointInRegion(int iX, int iY, PRegion ptRegion)
{
	if (iX < ptRegion->iLeftUpX || iX >= ptRegion->iLeftUpX + ptRegion->iWidth)
		return 0;
	if (iY < ptRegion->iLeftUpY || iY >= ptRegion->iLeftUpY + ptRegion->iHeight)
		return 0;

	return 1;
}


static PButton GetButtonByName(char* name)
{
	int i;

	for (i = 0; i < g_tButtonCnt; i++)
	{
		if (strcmp(name, g_tButtons[i].name) == 0)
			return &g_tButtons[i];
	}
	return NULL;
}

static PButton GetButtonByInputEvent(PInputEvent ptInputEvent)
{
	int i;
	char name[100];
	if (ptInputEvent->iType == INPUT_TYPE_TOUCH&& ptInputEvent->iPressure>0)
	{
		for (i = 0; i < g_tButtonCnt; i++)
		{
			if (isTouchPointInRegion(ptInputEvent->iX, ptInputEvent->iY,&g_tButtons[i].tRegion))
				return &g_tButtons[i];
		}
		
	}
	else if (ptInputEvent->iType == INPUT_TYPE_NET)
	{
		sscanf(ptInputEvent->str, "%s", name);
		return GetButtonByName(name);
	}
	else
		return NULL;
	return NULL;

}



static void MainPageRun(void* pParams)
{
	int error;
	InputEvent tInputEvent;
	PButton ptButton;
	PDispBuff ptDispBuff = GetDispBuffer();

	error = ParseConfigFile();
	if(error)
	{
		printf("ParseConfigFile err\n");
		return;
	}
	GenerateButtons();
	while (1)
	{
		error=GetInputEvent(&tInputEvent);
		if (error)
			continue;

		ptButton = GetButtonByInputEvent(&tInputEvent);
		if (!ptButton)
			continue;
		ptButton->OnPressed(ptButton, ptDispBuff, &tInputEvent);
	}
}

static PageAction g_tMainPage = {
.name = "main",
.Run = MainPageRun,
};


void MainPageRegister(void)
{
	PageRegister(&g_tMainPage);
}