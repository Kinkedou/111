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
#include "input_manager.h"




int main(int argc, char** argv)
{
	int ret;
	InputEvent event;
	InputInit();
	InputDeviceInit();
	while (1)
	{
		ret = GetInputEvent(&event);
		if (ret)
		{
			printf("GetInputEvent err!\n");
			return -1;
		}
		else
		{
			if (event.iType == INPUT_TYPE_TOUCH)
			{
				printf("Type :%d\n", event.iType);
				printf("iX :%d\n", event.iX);
				printf("iY :%d\n", event.iY);
				printf("iPressure :%d\n", event.iPressure);
			}
			else if (event.iType == INPUT_TYPE_NET)
			{
				printf("Type :%d\n", event.iType);
				printf("str :%s\n", event.str);
			}
		}
	}
	return 0;
}

