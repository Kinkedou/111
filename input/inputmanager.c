#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "input_manager.h"

static PInputDevice g_InputDevs = NULL;

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

#define BUFFER_LEN 20
static int g_iRead = 0;
static int g_iWrite = 0;
static InputEvent g_atInputEvents[BUFFER_LEN];

static int isInputBufferFull(void)
{
	return ((g_iWrite + 1) % BUFFER_LEN) == g_iRead;
}

static int isInputBufferEmpty(void)
{
	return g_iWrite == g_iRead;
}

static void PutInputEventToBuffer(PInputEvent ptInputEvent)
{
	if (!isInputBufferFull())
	{
		g_atInputEvents[g_iWrite] = *ptInputEvent;
		g_iWrite = (g_iWrite + 1) % BUFFER_LEN;
	}
}

static int GetInputEventFromBuffer(PInputEvent ptInputEvent)
{
	if (!isInputBufferEmpty())
	{
		*ptInputEvent = g_atInputEvents[g_iRead];
		g_iRead = (g_iRead + 1) % BUFFER_LEN;
		return 1;
	}
	else
		return 0;
}

void RegisterInputDevice(PInputDevice ptInputDev)
{
	ptInputDev->ptNext = g_InputDevs;
	g_InputDevs = ptInputDev;
}

void InputSystemRegister(void)//注册触摸屏和网络输入设备
{
	extern void TouchscreenRegister(void);
	TouchscreenRegister();
	extern void NetInputRegister(void);
	NetInputRegister();
}


static void* input_recv_thread_func(void* data)
{
	PInputDevice ptInputDev = (PInputDevice)data;
	InputEvent tEvent;
	int ret;
	while (1)
	{
		ret=ptInputDev->GetInputEvent(&tEvent);
		if (!ret)
		{
			pthread_mutex_lock(&g_tMutex);
			PutInputEventToBuffer(&tEvent);
			pthread_cond_signal(&g_tConVar); //这里发送条件变量信号，通知等待的线程
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

void  InputDeviceInit(void)
{
	PInputDevice ptTmp = g_InputDevs;
	while (ptTmp)
	{
		int ret;
		pthread_t tid;
		ret=ptTmp->DeviceInit();
		if (!ret)
		{
			
			ret = pthread_create(&tid, NULL, input_recv_thread_func, ptTmp);
		}
		ptTmp = ptTmp->ptNext;
	}
}

int GetInputEvent(PInputEvent ptInputEvent)
{
	InputEvent tEvent;
	int ret;
	pthread_mutex_lock(&g_tMutex);
	if (GetInputEventFromBuffer(&tEvent))
	{
		*ptInputEvent = tEvent;
		pthread_mutex_unlock(&g_tMutex);
		return 0;
	}
	else
	{
		pthread_cond_wait(&g_tConVar, &g_tMutex);	//这一步会释放互斥锁并阻塞等待条件变量信号
		if (GetInputEventFromBuffer(&tEvent))
			{
			*ptInputEvent = tEvent;
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		pthread_mutex_unlock(&g_tMutex);
	}
	return ret; 
}