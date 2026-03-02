#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H

typedef struct PageAction{
	char* name;
	void (*Run)(void* pParams);
	struct PageAction* ptNext;
} PageAction, *PPageAction;

void PageRegister(PPageAction ptPageAction);
PPageAction Page(char* name);
void PageSystemRegister(void);

#endif