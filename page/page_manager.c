
#include "page_manager.h"
#include "common.h"
#include <string.h>

static PPageAction g_ptPages = NULL;

void PageRegister(PPageAction ptPageAction)
{
	ptPageAction->ptNext = g_ptPages;
	g_ptPages = ptPageAction;
}

PPageAction Page(char* name)
{
	PPageAction ptCurrent = g_ptPages;
	
	while(ptCurrent != NULL)
	{
		if(strcmp(ptCurrent->name, name) == 0)
		{
			return ptCurrent;
		}
		ptCurrent = ptCurrent->ptNext;
	}
	return NULL;
}

void PageSystemRegister(void)
{
	extern void MainPageRegister(void);
	MainPageRegister();
}

