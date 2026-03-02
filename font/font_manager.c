#include "font_manager.h"
#include <string.h>

static PFontOpr g_ptFonts = NULL;
static PFontOpr g_ptDefaultFontOpr = NULL;

void RegisterFont(PFontOpr ptFontOpr)
{
	ptFontOpr->ptNext = g_ptFonts;
	g_ptFonts = ptFontOpr;
}

void FontSystemRegister(void)
{
	extern void FreetypeRegister(void);
	FreetypeRegister();
}

int SelectAndInitFont(char* aFontOprName,char* aFileName)
{
	PFontOpr ptTmp = g_ptFonts;
	while (ptTmp)
	{
		if (strcmp(ptTmp->name,aFontOprName)==0)
			break;
		ptTmp = ptTmp->ptNext;
	}
	if (!ptTmp)
		return -1;

	g_ptDefaultFontOpr = ptTmp;
	return ptTmp->FontInit(aFileName);
}

int SetFontSize(int iFontSize)
{
	return g_ptDefaultFontOpr->SetFontSize(iFontSize);
}

int GetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
	return g_ptDefaultFontOpr->GetFontBitMap(dwCode, ptFontBitMap);
}

int GetStringRegionCar(char* str, PRegionCartesian ptRegionCar)
{
	return g_ptDefaultFontOpr->GetStringRegionCar(str, ptRegionCar);
}