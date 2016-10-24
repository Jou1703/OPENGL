#include "includes.h"
#include <pthread.h>
#include "fun.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <dirent.h>
#include <stdarg.h>
#include "all_path.h"
#include "option.h"
#include "conf.h"



extern TSysData gSysData;

#define JOY_RUN 1
#define JOY_STOP 0

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int joy_status = JOY_RUN;

int GetUid()
{
	return getuid();
}
int GetUUid()
{
	return geteuid();
}

void ThreadMutex()
{
	
	pthread_mutex_lock(&mut);
	while (!joy_status)
	{
		pthread_cond_wait(&cond, &mut);
	}
	pthread_mutex_unlock(&mut);
}
void JoyThreadResume()
{
    if (joy_status == JOY_STOP)
    {   
        pthread_mutex_lock(&mut);
        joy_status = JOY_RUN;
        pthread_cond_signal(&cond);
        printf("pthread run!\n");
        pthread_mutex_unlock(&mut);
    }   
    else
    {   
        printf("pthread run already\n");
    }   
}

void JoyThreadPause()
{
    if (joy_status == JOY_RUN)
    {   
        pthread_mutex_lock(&mut);
        joy_status = JOY_STOP;
        printf("thread stop!\n");
        pthread_mutex_unlock(&mut);
    }   
    else
    {   
        printf("pthread pause already\n");
    }
}
pthread_t* thread_create(void*(*func)(void*),void *arg)
{
	int ret;
	pthread_t *thread;
	thread = (pthread_t *)malloc(sizeof(pthread_t));
	if (thread == NULL)
		return NULL;
	ret = pthread_create(thread,NULL,func,(void*)arg);
	if (ret !=0)
		return NULL;
	return thread;
}
void ThreadExit(void)
{
	pthread_exit(NULL);
}

int SystemEx(const char* cmd_string)
{
	int status = 0;
	int child_pid = -1;
	if (cmd_string == NULL)
		return -1;
	child_pid = vfork();
	if (child_pid == 0)
	{
		execl("/bin/sh","sh","-c",cmd_string,(char*)0);
	}
	else if (child_pid >0)
	{
		waitpid(child_pid, &status, 0);
	}
	else if (child_pid <0)
		return -1;
	return 0;
}

int SystemExSu(const char* cmd_string)
{
	int status = 0;
	int child_pid = -1;
	if (cmd_string == NULL)
		return -1;
	child_pid = vfork();
	if (child_pid == 0)
	{
		printf("uid = %d euid = %d\n",GetUid(),GetUUid());
		setuid(0);
		printf("uid = %d euid = %d\n",GetUid(),GetUUid());
		execl("/bin/sh","sh","-c",cmd_string,(char*)0);
	}
	else if (child_pid >0)
	{
		waitpid(child_pid, &status, 0);
	}
	else if (child_pid <0)
		return -1;
	return 0;
}

int IsFileExist(const char *file_path)
{
	return (!access(file_path,F_OK));
}
int IsDirExist(const char *dir_path)
{
	DIR *dp;
	if ((dp=opendir(dir_path))==NULL)
	{
		LOG_ERR("dir not exist");
		return 0;
	}
	
	closedir(dp);
	return 1;
}

int ChmodFile(const char* file,int mod)
{
	if(chmod(file,mod)==-1)
	{
		LOG_ERR("chmod err");
		return -1;
	}
	return 0;
}
int ChownFile(const char *pathname,uid_t owner,gid_t group)
{
	if( chown(pathname,owner,group)==-1)
	{
		LOG_ERR("chown err");
		return -1;
	}
	return 0;
}


char const *str_xrandr[10]=
{
	"1920x1080",
	"1680x1050",        
	"1600x1200",        
	"1440x900",        
	"1280x1024",       
	"1280x800",   
	"1280x768",  
	"1280x720",      
	"1152x864",       
	"1024x768"       
};

GLuint wxrandr[10]={1920,1680,1600,1440,1280,1280,1280,1280,1152,1024};
GLuint hxrandr[10]={1080,1050,1200,900, 1024,800, 768, 720, 864,  768};

extern char text_xrandr[100];

int GetResolutionPos(int width, int height)
{
	
	char resolution[64] = {0};
	sprintf(resolution,"%dx%d",width,height);
	
	for(int i=0;i<10;++i)
	{
		if (strcmp(resolution,str_xrandr[i]) ==0)
		 return i+1;
	}
	
	return 1;
}

int GetGraphicsCards()
{
	char card960[128] = "GeForce GTX 960";
	char card730[128] = "GeForce GT 730";
	int card=0;
	FILE *fp; 
	char buf[128]={0};
	fp = popen("/usr/sbin/hwinfo --gfxcard | grep Model", "r");  
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return 0;  
	}  
	while(1)
	{	
		memset(buf,0,sizeof(buf));
		if(fgets(buf, sizeof(buf), fp) != NULL)  
		{
 			if(strstr(buf,card960))
			{
				card++;
				LOG_INFO("card%d:%s found",card,card960);
			}	
			else if(strstr(buf,card730))
			{
				card++;
				LOG_INFO("card%d:%s found",card,card730);
			}
		}
		else
		{
			break;
		}
		
	}
	if(card < 2)
		return -1;
	else
		return 0;
	
	return -1;
}
int GetBiosKey()
{
	char bios_key[128] = "Shimmerfcgame+";
	
	FILE *fp; 

	char buf[128]={0};
	fp = popen("/usr/sbin/dmidecode -t12", "r");  
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return 0;  
	}  
	while(1)
	{	
		memset(buf,0,sizeof(buf));
		if(fgets(buf, sizeof(buf), fp) != NULL)  
		{
 			if(strstr(buf,bios_key))
			{
				LOG_INFO("bios key found");
				return 0;
			}
		}
		else
		{
			break;
		}
	}
	return -1;
}
int SetSndPort(int n)
{
	FILE *fp = NULL;
	char buf[128] = {0};
	char c =  n+48;
	int f=0;
	if((fp = fopen("/usr/share/alsa/alsa.conf","r+"))==NULL)
	{
		LOG_ERR("fopen err");
		return -1;
	}
	while(1)
	{
		memset(buf,0,sizeof(buf));
		if(fgets(buf,sizeof(buf),fp) != NULL)
		{
			if(strstr(buf,"defaults.pcm.device")!=NULL)
			{
				f=1;
				break;
			}
		}
		else 
		{
			fclose(fp);
			return -1;
		}
	}
	fseek(fp, -2, SEEK_CUR);	
	fwrite(&c,1,1,fp);
	fclose(fp);
	return  0;
	
}

int GetSndPort(const char *eld)
{
	FILE *fp; 
	char buf[128]={0};
	char cmd[128]={0};
	
	sprintf(cmd,"cat /proc/asound/card1/%s",eld);
	fp = popen(cmd, "r");
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return 0;  
	} 
	if(fgets(buf, sizeof(buf), fp) != NULL)  
	{
		if(buf[17] == '1')
		{
			LOG_INFO("hw:1,7");
			SetSndPort(7);
			
		}
		else
		{
			LOG_INFO("hw:1,8");
			SetSndPort(8);
		}
			
	}
	return 0;
}

int getxrandr(const char* find_str)
{
	FILE *fp; 
	char buf[128]={0};
	char *result;
	
	fp = popen("xrandr", "r");  
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return 0;  
	}   

	while(1)
	{	
		if(fgets(buf, sizeof(buf), fp) != NULL)  
		{  
			result = strstr(buf,find_str);
			if (result == NULL)
			{
				continue;
			}
			else
			{
				pclose(fp);
				return 1;
			}
		}
		else
			break;
	} 
	pclose(fp);
	
	return 0;
}
void InitResolution()
{
	char cmd[128]={0};
	ReadConfStr("resolution",gSysData.set_resolution);
	sprintf(cmd,"xrandr -s %s",gSysData.set_resolution);
	SystemEx(cmd);
	
}

int SetXrandr(GLbyte res,int *width, int *height)
{
	char cmd[128]={0};
	*width = wxrandr[res];
	*height = hxrandr[res];
	
	if (!getxrandr(str_xrandr[res]))
	{
		LOG_ERR("no res found");
		return 0;	
	}	
	
	sprintf(cmd,"xrandr -s %s",str_xrandr[res]);
	
	LOG_INFO("set xrandr:%s",cmd);
	SystemEx(cmd);
	memcpy(gSysData.set_resolution,str_xrandr[res],sizeof(str_xrandr[res])+1);
	WriteConfStr("resolution",gSysData.set_resolution);
	execl("/bin/sh","sh","-c",STEAMUI_PATH,(char*)0);
	return 1;
}

void WriteLog(const char* string)
{
	FILE *log_fp;
	struct stat statbuf;
	char run_file[60] = RUN_PATH;
	
	stat(run_file,&statbuf);
	if (statbuf.st_size <1024*1024*20)
	{
		log_fp = fopen(run_file,"at+");
		if(log_fp!= NULL)
		{
			fwrite(string,1,strlen(string),log_fp);
		}
		fclose(log_fp);
	}
	else
	{
		log_fp = fopen(run_file,"at+");
		if(log_fp!= NULL)
		{
			int fd = fileno(log_fp);
			ftruncate(fd,0);
		}
		fclose(log_fp);
	}
}
void GetTimeNow(char *time_str)
{
	time_t tm;
	struct tm *ptm;
	time(&tm);
	ptm=localtime(&tm);
	sprintf(time_str,"%04d%02d%02d-%02d%02d%02d",1900+ptm->tm_year,1+ptm->tm_mon,ptm->tm_mday,
		ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
}

void Log(PRIN_TYPE type,const char* file,const char* func, int line,const char* format,...)
{
	char log_str[128] = {0};
	char log[256] = {0};
	va_list va;
    va_start(va, format);
    vsnprintf(log_str,128,format,va);
    va_end(va);
  
    sprintf(log,"%s %s %d %s\n",file,func,line,log_str);
    printf(log);
    if(type == L_ERR | type == L_INFO)
    {
		char tm_str[16] = {0};
		GetTimeNow(tm_str);
		WriteLog(tm_str);
		WriteLog(log);
	}
}



