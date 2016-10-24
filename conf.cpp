#include "includes.h"
#include "conf.h"
#include "fun.h"
#include "chmod_opt.h"
#include <sys/types.h> 
#include <sys/stat.h>

int FreadOneLine(FILE *fp, char *buffer, int *size);
void SplitByChar(char *buffer, char *name, char * value, char DeliChar);

int ReadConfStr(const char* name,char* res)
{
		char buf[64]={0};
		char *str=NULL;
		int size = 0;

		FILE *fp= fopen("/home/steam/.shimmer/opt.conf","r+");
		if (fp == NULL)
		{
			LOG_ERR("fopen err!\n");
			return 0;
		}
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			 FreadOneLine(fp, buf, &size);
			if(!size)
				break;
			if (strncmp(buf,name,strlen(name))==0)
			{
				str = buf+strlen(name)+1;
				memcpy(res,str,64);
				break;
			}				
		}
		fclose(fp);
		return 1;
}


int ReadConfInt(const char* name,int* res)
{
		
		char buf[64]={0};
		char *str=NULL;
		int size = 0;

		FILE *fp= fopen("/home/steam/.shimmer/opt.conf","r");
		if (fp == NULL)
		{
			LOG_ERR("fopen err!\n");
			return 0;
		}
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			 FreadOneLine(fp, buf, &size);
			if(!size)
				break;
			if (strncmp(buf,name,strlen(name))==0 && buf[strlen(name)]== '=')
			{
				str = buf+strlen(name)+1;
				*res = atoi(str);
				break;
			}				
		}
		fclose(fp);
		return 1;
}

int WriteConfInt(const char* name,int val)
{
	char buf[128] ={0};
	sprintf(buf,"%d",val);
	WriteConfStr(name,buf);
	return 0;
}
int WriteConfStr(const char* name,char* val)
{
		
		char str_val[32]={0};
		int size = 0;
		char buf[128];
		//char find=0;


		FILE *fp= fopen("/home/steam/.shimmer/opt.conf","r+");
		FILE *fp_tmp = fopen("/home/steam/.shimmer/opt.conf.tmp","w+");
		if (fp == NULL)
		{
			LOG_ERR("fopen err!\n");
			
			return 0;
		}
		 if (fp_tmp == NULL)
		{
			LOG_ERR("fopen err!\n");
			
			return 0;
		}

		sprintf(str_val, "%s=%s",name,val);
		
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			FreadOneLine(fp, buf, &size);
			if(!size)
				break;
			
			if (strncmp(buf,name,strlen(name))==0)
			{
				strcat(str_val,"\n");
				fwrite(str_val, strlen(str_val),1,fp_tmp);
			}
			else
			{
				strcat(buf,"\n");
				fwrite(buf, strlen(buf),1,fp_tmp);
			}
			
		}
		//if(find)
		//	fwrite(str_val, strlen(str_val),1,fp);
			
		fclose(fp);	
		fclose(fp_tmp);

		system("rm /home/steam/.shimmer/opt.conf");
		system("mv /home/steam/.shimmer/opt.conf.tmp /home/steam/.shimmer/opt.conf");
		
		
		return 1;
}


int FreadOneLine(FILE *fp, char *buffer, int *size)
{
	char c;
	*size = 0;
	while(1)
	{
		if (fread(&c, 1, 1, fp))
		{
			if (c=='\r' || c== '\n')
			{
					if (*size == 0)
							continue;
						else
							break;
			}
			buffer[*size] = c;
			(*size)++;
		}
		else
			break;
		
	}
	if (*size>0)
		buffer[*size] = '\0';
	return (*size > 0);

}

