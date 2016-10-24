#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include "includes.h"


int ReadConfStr(const char* name,char* res);
int ReadConfInt(const char* name,int* res);
int WriteConfStr(const char* name,char* val);
int WriteConfInt(const char* name,int val);

void *SysTimeUpdate(void *arg);

#endif //__INCLUDES_H__

