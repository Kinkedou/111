
#ifndef _COMMON_H
#define _COMMON_H


typedef struct Region {
	int iLeftUpX;
	int iLeftUpY;
	int iWidth;
	int iHeight;
}Region, * PRegion;

typedef struct RegionCartesian {
	int iLeftUpX;
	int iLeftUpY;
	int iWidth;
	int iHeight;
}RegionCartesian, * PRegionCartesian;

#ifndef NULL
#define NULL (void *)0
#endif // !NULL

#endif

