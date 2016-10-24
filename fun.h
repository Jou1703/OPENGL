#ifndef __FUN_H__
#define __FUN_H__

#include <pthread.h>

enum PRIN_TYPE{
	L_ERR,
	L_INFO,
	L_WARNING,
	L_DEBUG
	};

pthread_t* thread_create(void*(*func)(void*),void *arg);
void ThreadMutex();
void JoyThreadPause();
void JoyThreadResume();
void ThreadExit(void);
int SystemEx(const char* cmd_string);
int SystemExSu(const char* cmd_string);
//void DPrint(const char *name,int n);
int getxrandr(const char* find_str);
int IsFileExist(const char *file_path);
int IsDirExist(const char *dir_path);
int SetXrandr(GLbyte res,int *width, int *height);
int GetResolutionPos(int width, int height);
void InitResolution();

int GetGraphicsCards();
int GetBiosKey();
int GetSndPort(const char *eld);


void Log(PRIN_TYPE type,const char* file,const char* func, int line,const char* format,...);
#define LOG_ERR(format...)  Log(L_ERR,__FILE__,__FUNCTION__, __LINE__,format)
#define LOG_INFO(format...)  Log(L_INFO,__FILE__,__FUNCTION__, __LINE__,format)
#define LOG_WARNING(format...)  Log(L_WARNING,__FILE__,__FUNCTION__, __LINE__,format)
#define LOG_DEBUG(format...)  Log(L_DEBUG,__FILE__,__FUNCTION__, __LINE__,format)




#endif //__FUN_H__
