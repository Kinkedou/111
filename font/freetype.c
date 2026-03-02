#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include "font_manager.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

static 	FT_Face g_tFace;
static int g_iDefaultFontSize = 12;

static int FreetypeGetStringRegionCar(char* str, PRegionCartesian ptRegionCar)
{
    int i;
    int error;
    FT_BBox bbox;
    FT_BBox glyph_bbox;
    FT_Vector pen;
    FT_Glyph  glyph;
    FT_GlyphSlot slot = g_tFace->glyph;

    /* 初始化 */
    bbox.xMin = bbox.yMin = 32000;
    bbox.xMax = bbox.yMax = -32000;

    /* 指定原点为(0, 0) */
    pen.x = 0;
    pen.y = 0;

    /* 计算每个字符的bounding box */
    /* 先translate, 再load char, 就可以得到它的外框了 */
    for (i = 0; i < strlen(str); i++)
    {
        /* 转换：transformation */
        FT_Set_Transform(g_tFace, 0, &pen);

        /* 加载位图: load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(g_tFace, str[i], FT_LOAD_RENDER);
        if (error)
        {
            printf("FT_Load_Char error\n");
            return -1;
        }

        /* 取出glyph */
        error = FT_Get_Glyph(g_tFace->glyph, &glyph);
        if (error)
        {
            printf("FT_Get_Glyph error!\n");
            return -1;
        }

        /* 从glyph得到外框: bbox */
        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &glyph_bbox);

        /* 更新外框 */
        if (glyph_bbox.xMin < bbox.xMin)
            bbox.xMin = glyph_bbox.xMin;

        if (glyph_bbox.yMin < bbox.yMin)
            bbox.yMin = glyph_bbox.yMin;

        if (glyph_bbox.xMax > bbox.xMax)
            bbox.xMax = glyph_bbox.xMax;

        if (glyph_bbox.yMax > bbox.yMax)
            bbox.yMax = glyph_bbox.yMax;

        /* 计算下一个字符的原点: increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    /* return string bbox */
	ptRegionCar->iLeftUpX = bbox.xMin;
	ptRegionCar->iLeftUpY = bbox.yMax;
	ptRegionCar->iWidth = (bbox.xMax - bbox.xMin+1);
    ptRegionCar->iHeight = (bbox.yMax - bbox.yMin+1);
	return 0;
}

static int FreeTypeFontInit(char* aFileName)
{
	FT_Library    library;
	int error;
	error = FT_Init_FreeType(&library);//这里初始化FreeType库
	if (error)
	{
		printf("FT_Init_FreeType err\n");
		return -1;
	}
	error = FT_New_Face(library, aFileName, 0, &g_tFace); //这里创建一个字体对象，g_tFace
	if (error)
	{
		printf("FT_New_Face err\n");
		return -1;
	}
	FT_Set_Pixel_Sizes(g_tFace, g_iDefaultFontSize, 0);
	return 0;
}

static int FreeTypeSetFontSize(int iFontSize)
{
	FT_Set_Pixel_Sizes(g_tFace, iFontSize, 0);
	return 0;
}

static int FreeTypeGetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
	int error;
	FT_Vector pen; //pen是一个向量对象，有x和y两个成员，指定字形原点的位置
	FT_GlyphSlot slot = g_tFace->glyph;//glyph slot是一个具体的字形对象

	pen.x = ptFontBitMap->iCurOriginX * 64; //单位: 1/64像素 ，pen.x和 pen.y 指定了字形在位图中的位置
	pen.y = ptFontBitMap->iCurOriginY * 64; /* 单位: 1/64像素 */
	FT_Set_Transform(g_tFace, 0, &pen);//设置字形放置位置，0表示没有旋转

	/* 加载位图: load glyph image into the slot (erase previous one) */
	error = FT_Load_Char(g_tFace, dwCode, FT_LOAD_RENDER); //加载字符dwCode对应的字形，并进行渲染（即生成位图）。FT_LOAD_RENDER标志表示同时渲染字形（即生成位图数据）。结果存储在字形槽中。
	if (error)
	{
		printf("FT_Load_Char error\n");
		return -1;
	}


	ptFontBitMap->pucBuffer = slot->bitmap.buffer;//位图缓冲区的作用是存储字形的像素数据
	ptFontBitMap->tRegion.iLeftUpX = slot->bitmap_left;//字形左上角相对于原点的水平偏移量
	ptFontBitMap->tRegion.iLeftUpY = ptFontBitMap->iCurOriginY * 2 - slot->bitmap_top;//字形左上角相对于原点的垂直偏移量
	ptFontBitMap->tRegion.iWidth = slot->bitmap.width;//字形位图的宽度
	ptFontBitMap->tRegion.iHeight = slot->bitmap.rows;//字形位图的高度
	ptFontBitMap->iNextOriginX = ptFontBitMap->iCurOriginX + slot->advance.x / 64;
	ptFontBitMap->iNextOriginY = ptFontBitMap->iCurOriginY;
	return 0;
}

static FontOpr g_tFreeTypeOpr={
	.name = "freetype",
	.FontInit = FreeTypeFontInit,
	.SetFontSize = FreeTypeSetFontSize,
	.GetFontBitMap = FreeTypeGetFontBitMap,
	.GetStringRegionCar = FreetypeGetStringRegionCar,
};

void FreetypeRegister(void)
{
	RegisterFont(&g_tFreeTypeOpr);
}

