#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "option.h"
#include "search.h"
#include "fun.h"
using namespace std;

extern TSysData gSysData;


vector<SteamLibInfo> game_info;
vector<SteamLibInfo> mobile_disk_game;
vector<TSMGame> gSMLocalGameVec;

int IsCopy(char *name);

int FreadLine(FILE *fp, char *buffer, int *size)
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
int GetLibDir(char* buf,char *dir)
{
	char *b;
	b=buf;
	
	int t=0,d=0;
	while(*b!='\0')
	{
		t++;
		if(*b == '"')
			d++;
		if(d==3)
		{
			break;
		}
		b++;		
	}
	memcpy(dir,buf+t,strlen(buf)-t-1);
	return 0;
}
int ser_local_num;
int ser_move_num;

int GetLibInfo(const char* acf,int no)
{	
	int size = 0;
	char buf[512];
	FILE *sfp; 
	char sbuf[128]={0};
	char date[128]={0};
	char *tmp;
	FILE *fp= fopen(acf,"r");
	if (fp == NULL)
	{
		printf("search fopen err!\n");	
		return 0;
	}	
	SteamLibInfo sli;
	memset(&sli,0,sizeof(struct SteamLibInfo));
	if(no==1)
	{
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			FreadLine(fp, buf, &size);
			if(!size)
				break;
			if(strstr(buf,"name")!=NULL)
			{	
				GetLibDir(buf,sli.name);
							
			}
			if(strstr(buf,"installdir")!=NULL)
			{	
				GetLibDir(buf,sli.dir);			
			}				
		}
		if(IsCopy(sli.dir)<0)
			return 0;
		char buf1[256]={0};
		
		sprintf(buf1,"du -sm %s\"%s\" ",STEAM_GAME_PATH,sli.dir);
		printf("SteamGame:%s\n",buf1);
		
		sfp = popen(buf1, "r");  
		if(NULL == sfp)  
		{  
			perror("popen error");  
			return 0;  
		} 
		
		if(fgets(sbuf, sizeof(sbuf), sfp)!=NULL)
		{
			tmp = sbuf;
			int j=0;
			while(*tmp != '\t')
			{	
				date[j]=*tmp;
				tmp++;
				j++;
			}
			sprintf(sli.dis_size,"%s",date);
		

			memcpy(sli.acf,acf,strlen(acf));
		
			ser_local_num += 1;
			memcpy(&sli.number,&ser_local_num,4);
			sli.showname=1;
			if(sli.number>5)
			{
				sli.showname=0;
			}
			game_info.push_back(sli);
			pclose(sfp);
		}
		else
		{
			pclose(sfp);
			return -1;
		}
		
	}
	else if(no==0)
	{
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			FreadLine(fp, buf, &size);
			if(!size)
				break;
			if(strstr(buf,"name")!=NULL)
			{	
				GetLibDir(buf,sli.name);			
			}
			if(strstr(buf,"installdir")!=NULL)
			{	
				GetLibDir(buf,sli.dir);
		
			}				
		}
		
		char buf1[256]={0};
		sprintf(buf1,"du -sm %s\"%s\" ",DISK_MOUNT_PATH,sli.dir);
		
		sfp = popen(buf1, "r");  
		if(NULL == sfp)  
		{  
			perror("popen error");  
			return 0;  
		} 
		if(fgets(sbuf, sizeof(sbuf), sfp)!=NULL)
		{
			tmp = sbuf;
			
			int j=0;
			
			while(*tmp != '\t')
			{	
				date[j]=*tmp;
				tmp++;
				j++;
			}
			sprintf(sli.dis_size,"%s",date);
		
		
			memcpy(sli.acf,acf,strlen(acf));
		
			ser_move_num += 1;
			memcpy(&sli.number,&ser_move_num,4);
			sli.showname=1;
			if(sli.number>5)
			{
				sli.showname=0;
			}
			mobile_disk_game.push_back(sli);
		}
		else
		{
			pclose(sfp);
			return -1;
		}	
	}
	fclose(fp);
	return 0;
}

void ShowNameDown(int n)
{
	if(game_info.size()==0)
		return;
	if(gSysData.sg_posdown==1)
	{
		game_info[n-4].showname=0;
		game_info[n+1].showname=1;
	}
	else if(gSysData.sg_posup==1)
	{
		game_info[n].showname=1;
		game_info[n+5].showname=0;
	}
	
}

void ShowNameUp(int n)
{
	if(mobile_disk_game.size()==0)
		return;
	if(gSysData.mg_posdown==1)
	{
		mobile_disk_game[n-4].showname=0;
		mobile_disk_game[n+1].showname=1;
	}
	else if(gSysData.mg_posup==1)
	{
		
		mobile_disk_game[n].showname=1;
		mobile_disk_game[n+5].showname=0;
	}
	printf("move = %d num:%d game:%s\n",gSysData.game_move,mobile_disk_game[gSysData.game_move].number,mobile_disk_game[gSysData.game_move].name);
	
}
void ShownameRm(int n)
{
	if(mobile_disk_game.size()==0)
		return;
	if(n==gSysData.mgame_num)
	{
		mobile_disk_game[n].showname=0;
		mobile_disk_game[n-5].showname=1;
	}	
	else
		mobile_disk_game[n+1].showname=1;
}

void Clearlist()
{
	for(vector<SteamLibInfo>::iterator it=game_info.begin(); it!=game_info.end(); it++)
	{
		if((*it).number < 6)
			(*it).showname=1;
		else 
			(*it).showname=0;	
	}
	for(vector<SteamLibInfo>::iterator it=mobile_disk_game.begin(); it!=mobile_disk_game.end(); it++)
	{
		if((*it).number < 6)
			(*it).showname=1;
		else 
			(*it).showname=0;	
	}
		
}

void ScanMobileDisk(int *num)
{
	DIR *dirp;
	struct dirent* dt;
	dirp = opendir(DISK_MOUNT_PATH);
	
	if(dirp == NULL)
	{
		printf("opendir error");
		return ;
	}	
	ser_move_num=0;
	mobile_disk_game.clear();
	while((dt=readdir(dirp))!=NULL)
	{
		char name[256];	
		if(strstr((dt->d_name),".acf")!=NULL)
		{
			memset(name,0,sizeof(name));
			sprintf(name,"%s%s",DISK_MOUNT_PATH,dt->d_name);
			
			GetLibInfo(name,0);			
		}
	}
	
	closedir(dirp);
	*num=mobile_disk_game.size();
	/*
	for(vector<SteamLibInfo>::iterator it=mobile_disk_game.begin(); it!=mobile_disk_game.end(); it++)
	{
		
		printf("search:\n%s\n%s\n%s\n%d\n\n",(*it).name,(*it).dir,(*it).acf,(*it).number);
	}
	*/
	
	
}

int IsCopy(char *name)
{
	DIR *dirp;
	struct dirent* dt;

	dirp = opendir(STEAM_GAME_PATH);
	if(dirp == NULL)
	{
		printf("opendir error");
		return -1;
	}	 
	while((dt=readdir(dirp))!=NULL)
	{
		if(strcmp(dt->d_name,name)==0)
		{
			return 0;
		}
	}
	closedir(dirp);
	return -1;
	
}
void ScanGames(int *num)
{
	DIR *dirp;
	struct dirent* dt;
	dirp = opendir(STEAM_GAMEACF_PATH);
	
	if(dirp == NULL)
	{
		LOG_ERR("opendir error");
		return ;
	}
	ser_local_num=0;
	game_info.clear();
	while((dt=readdir(dirp))!=NULL)
	{
		char name[256];	
		if(strstr((dt->d_name),".acf")!=NULL)
		{
			memset(name,0,sizeof(name));
			sprintf(name,"%s%s",STEAM_GAMEACF_PATH,dt->d_name);
			GetLibInfo(name,1);			
		}
	}
	closedir(dirp);
	*num=game_info.size();
	
	/*
	for(vector<SteamLibInfo>::iterator it=game_info.begin(); it!=game_info.end(); it++)
	{
		
		printf("GameInfo:%s %s %s %d %s\n",(*it).name,(*it).dir,(*it).acf,(*it).number,(*it).dis_size);
	}
	*/
		
		
}
void ScanSMGames()
{
	DIR *dirp;
	struct dirent* dt;
	dirp = opendir("/home/steam/.shimmer/smgame/common");
	
	if(dirp == NULL)
	{
		LOG_ERR("opendir error");
		return ;
	}
	while((dt=readdir(dirp))!=NULL)
	{
		TSMGame smgame_local;
		if(dt->d_name[0] != '.')
		{
			snprintf(smgame_local.game_name,sizeof(smgame_local.game_name),"%s",dt->d_name);
			snprintf(smgame_local.game_img,sizeof(smgame_local.game_img),"/home/steam/.shimmer/smgame/common/%s/%s.bmp",dt->d_name,dt->d_name);
			smgame_local.img_load = true;
			gSMLocalGameVec.push_back(smgame_local);
		}
	}
	closedir(dirp);
	
}







