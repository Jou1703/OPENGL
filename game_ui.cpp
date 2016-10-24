#include "includes.h"
#include <sys/mount.h>

#include "init_ui.h"
#include "particle.h"
#include "game_ui.h"
#include "draw_utils.h"
#include "option.h"
#include "search.h"
#include <vector>
#include "fun.h"
using namespace std;
extern float xcoord, ycoord, width, heigth;
extern TextureImage bg_tex;
extern vector<SteamLibInfo> game_info; 
extern vector<SteamLibInfo> mobile_disk_game;
extern vector<TSMGame> gSMLocalGameVec;
extern TSysData gSysData;



extern vector<TSMGame> gSMGameVec;


TextureImage gamelib_title, info1,info2,pt_down,yi_down,downing,down_line;
TextureImage cir,warning;
char cping_gamename[256]={0};
void GamePicLoad(void)
{
	LoadBmpTex(&info1 ,get_path("info1.bmp"));
	LoadTgaTex(&info2,get_path("info2.tga"));
	LoadTgaTex(&gamelib_title,get_path("store.tga"));
	LoadTgaTex(&pt_down,get_path("bendicipan.tga"));
	LoadTgaTex(&yi_down,get_path("yidong.tga"));
	LoadTgaTex(&downing,get_path("downing.tga"));
	LoadTgaTex(&down_line,get_path("down_line.tga"));
	LoadTgaTex(&cir,get_path("yellow_cir.tga"));
	LoadTgaTex(&warning,get_path("warning.tga"));
	
}
int get_con;

void *CpLocal2Move(void *arg)
{
	gSysData.cping=1;
	get_con=2;
	char buf[256]={0};
	char dir_buf[256]={0};
	sprintf(dir_buf,"%s%s",DISK_MOUNT_PATH,game_info[gSysData.game_save].dir);
	if(IsDirExist(dir_buf)==0)
	{
		memcpy(cping_gamename,game_info[gSysData.game_save].dir,sizeof(game_info[gSysData.game_save].dir));
		snprintf(buf,sizeof(buf),"/home/steam/.shimmer/cp_steamgame.sh \"%s\" \"%s\" ",game_info[gSysData.game_save].dir,game_info[gSysData.game_save].acf);
		SystemEx(buf);
		printf("Copy %s from  local to Mobile HDD\n",mobile_disk_game[gSysData.game_move].name);
		ScanMobileDisk(&gSysData.mgame_num);	
	}
	gSysData.getsizeing=0;	
	gSysData.cping=0;
	gSysData.sure_copy=0;
	gSysData.game_selected=0;
	return NULL;
}

void *CpMove2Local(void *arg)
{
	gSysData.mcping=1;
	
	get_con=2;
	char buf[256]={0};
	char dir_buf[256]={0};
	sprintf(dir_buf,"%s%s",STEAM_GAME_PATH,mobile_disk_game[gSysData.game_move].dir);
	if(IsDirExist(dir_buf)==0)
	{	
		if(memcpy(cping_gamename,mobile_disk_game[gSysData.game_move].dir,sizeof(mobile_disk_game[gSysData.game_move].dir))==NULL)
			LOG_ERR("memcpy err!\n");
		if(snprintf(buf,sizeof(buf),"/home/steam/.shimmer/mv_steamgame.sh \"%s\" \"%s\" ",mobile_disk_game[gSysData.game_move].dir,mobile_disk_game[gSysData.game_move].acf)<0)
			LOG_ERR("snprintf err!\n");
		
		SystemEx(buf);
		
		printf("Copy %s from Mobile HDD to local\n",mobile_disk_game[gSysData.game_move].name);
		ScanGames(&gSysData.game_num);
		
	}

	gSysData.getmsizeing=0;		
	gSysData.mcping=0;
	gSysData.msure_copy=0;
	gSysData.mgame_selected=1;
	return NULL;
}

void *DelMoveGame(void *arg)
{
	char buf[256]={0};
	
	sprintf(buf,"rm %s\"%s\" \"%s\" -rf",DISK_MOUNT_PATH,mobile_disk_game[gSysData.game_move].dir,mobile_disk_game[gSysData.game_move].acf);
	SystemEx(buf);
	LOG_INFO("Delete %s from Mobile HDD",mobile_disk_game[gSysData.game_move].name);
	printf("move = %d\n",gSysData.game_move);
	//ScanMobileDisk(&gSysData.mgame_num);
	gSysData.mgame_num--;
	mobile_disk_game.erase(mobile_disk_game.begin()+gSysData.game_move);
	
	

	for(vector<SteamLibInfo>::iterator it=mobile_disk_game.begin(); it!=mobile_disk_game.end(); it++)
	{
		if((*it).number>gSysData.game_move)
		{
			(*it).number--;
		}
		if((*it).number < 6)
			(*it).showname=1;
		else 
			(*it).showname=0;

		printf("search:\n%d %d\n%s\n%s\n%s\n\n",(*it).number,(*it).showname,(*it).name,(*it).dir,(*it).acf);	
	}
	gSysData.game_move = 0;
	gSysData.mg_pos = 1;
		
	

	gSysData.mgame_selected=1;
	gSysData.sure_rm=0;
	gSysData.msure_copy=0;
	return NULL;
}

int GetgameSize()
{
	FILE *fp=NULL; 
	char buf[128]={0};
	char buf1[256]={0};
	char date[128]={0};
	char *tmp;
	
	
	if(gSysData.sure_copy==1)
	{
		snprintf(buf1,sizeof(buf1),"du -sm %s\"%s\" ",DISK_MOUNT_PATH,game_info[gSysData.game_save].dir);
	}
	else if(gSysData.msure_copy==1)
	{
		snprintf(buf1,sizeof(buf1),"du -sm %s\"%s\" ",DISK_MOUNT_PATH,mobile_disk_game[gSysData.game_move].dir);
	}
	else
		return -1;
	fp = popen(buf1, "r");  
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return 0;  
	} 
	
	if(fgets(buf, sizeof(buf), fp)!=NULL)
	{
		pclose(fp);
		tmp = buf;
		int i=0;
		while(*tmp != '\t' && tmp !=NULL)
		{	
			date[i]=*tmp;
			tmp++;
			i++;
		}
		gSysData.game_size =(int)(atof(date));
		return 0;	
	}
	pclose(fp);	
	return -1;
}



int GetSgameSize()
{
	FILE *fp=NULL; 
	char buf[128]={0};
	char date[128]={0};
	char *tmp=NULL;
	char buf1[256]={0};
	if(gSysData.sure_copy==1)
	{
		snprintf(buf1,sizeof(buf1),"du -sm %s\"%s\" ",STEAM_GAME_PATH,game_info[gSysData.game_save].dir);
	}
	else if(gSysData.msure_copy==1)
	{
		snprintf(buf1,sizeof(buf1),"du -sm %s\"%s\" ",STEAM_GAME_PATH,mobile_disk_game[gSysData.game_move].dir);
	}
	else 
		return -1;
	fp = popen(buf1, "r");  
	if(NULL == fp)  
	{  
		LOG_ERR("popen error\n");  
		return -1;  
	} 
	
	if(fgets(buf, sizeof(buf), fp)!=NULL)
	{
		pclose(fp);
		tmp = buf;
		int i=0;
		while(*tmp != '\t' && tmp !=NULL)
		{	
			date[i]=*tmp;
			tmp++;
			i++;
		}
		gSysData.Sgame_size =(int)(atof(date));
		return 0;
	}
	pclose(fp);
	return -1;
	
}

void *MountDisk(void *arg)
{
	gSysData.mounting=1;
	static int flat;
	FILE * f;
	char name[64];
	char dev_name[64]={0};
	char line[256];
	int major, minor;
	int blocks;
	char * frc;
	f = fopen("/proc/partitions", "r");
	if (!f) 
	{
		LOG_ERR("partitions error");
		return NULL;
	}
	frc = fgets(line, sizeof(line), f);
	while(frc)
	{
		
		if (sscanf(line, "%d %d %d %s", &major, &minor, &blocks, name) != 4) 
		{
			line[strlen(line) - 1] = '\0';	
		}
		if ((blocks>458383360) && (memcmp(name, "sda", 3)!=0))
		{
			memcpy(dev_name,name,sizeof(name));
			flat=1;
			break;	
		}
		else
		{
			flat=2;
		}
		frc = fgets(line, sizeof(line), f);
		
	}
	LOG_INFO(dev_name);
	if(flat==2)
	{
		gSysData.mounting=2;	
	}
	else if(flat==1)
	{
		
		char devbuf[256];
		LOG_INFO("MobileDev name:%s\n",dev_name);
		
		setuid(0);
		//setreuid(1001, 0);

		//printf("uid = %d\n",getuid());
		//printf("euid = %d\n",geteuid());
		SystemEx("umount /media/usb0");
		snprintf(devbuf,sizeof(devbuf),"mount -t ntfs-3g /dev/%s1 /media/usb0",dev_name);
		LOG_INFO(devbuf);
		SystemEx(devbuf);
		setuid(1001);

		if(gSysData.steam_game_cp==1)
		{
			ScanMobileDisk(&gSysData.mgame_num);
			gSysData.mounted=0;
			gSysData.mounting=0;	
		}	
		gSysData.conlture=1;
	}
}
void DrawCpGame()
{
	char str[24]={0};
	float base_pos_x=-250.0f;
	float base_pos_y=20.0f;
	char cping_game[256];
	
	glColor3ub(231,111,1);
	FontDraw(base_pos_x+300, base_pos_y-60,"已完成...",20);
	//FontDraw(base_pos_x+450, base_pos_y-60,"%%",20);
	FontDraw(base_pos_x+80, base_pos_y+130,"请勿离开此界面",25);
	sprintf(cping_game,"正在拷贝 %s",cping_gamename);
	FontDraw(base_pos_x, base_pos_y+28,cping_game,20);
	ShowPic(base_pos_x+20, base_pos_y+130, 0.0f, warning.width, warning.height, warning.texID);
	
	if(gSysData.cping==1)
	{	
		
		snprintf(str,sizeof(str),"%d/%d",gSysData.game_size,gSysData.Sgame_size);
		FontDraw(base_pos_x+415, base_pos_y-60,str,20);
		ShowPic(base_pos_x, base_pos_y, 0.0f, 505, 20, down_line.texID);
		ShowPic(base_pos_x-5,base_pos_y, 0.0f, 20, 20, cir.texID);
		if(gSysData.Sgame_size>0)
		{
			ShowPic(base_pos_x-8+(500/100)*((gSysData.game_size*100)/(gSysData.Sgame_size)),base_pos_y, 0.0f, 20, 20, cir.texID);
			DrawYeRect(base_pos_x, base_pos_y,((gSysData.game_size*500)/(gSysData.Sgame_size)), 20);
		}
	}
	else if(gSysData.mcping==1)
	{
		
		snprintf(str,sizeof(str),"%d/%d",gSysData.Sgame_size,gSysData.game_size);
		FontDraw(base_pos_x+415, base_pos_y-60,str,20);
		ShowPic(base_pos_x, base_pos_y, 0.0f, 500, 20, down_line.texID);
		ShowPic(base_pos_x-5,base_pos_y, 0.0f, 20, 20, cir.texID);
		if(gSysData.game_size>0)
		{	
			ShowPic(base_pos_x-8+(500/100)*((gSysData.Sgame_size*100)/(gSysData.game_size)),base_pos_y, 0.0f, 20, 20, cir.texID);
			DrawYeRect(base_pos_x, base_pos_y,((gSysData.Sgame_size*500)/(gSysData.game_size)), 20);
		}
	}	
}


void GameRender(void)
{
	
	ShowPic(xcoord, ycoord, 0, width,heigth, bg_tex.texID);
	if (width<1920)
	return;
	TipNote();
	TimeDisplay();
	glEnable(GL_BLEND);
	ShowPartPic(0.0,1.0,0.3,1.0,-860, 350.0f,0.0f,gamelib_title.width, gamelib_title.height*0.6, gamelib_title.texID);
	FontDraw(-850, 280.0f,"游戏库",40);
	
	if(gSysData.steam_game_cp==1)
	{
		if(gSysData.mounting==1)
		{
			glColor3ub(231,123,1);
			FontDraw(220,70, "正在打开移动设备,请稍等...",20);
		}
		else if(gSysData.mounting==2)
		{
			glColor3ub(231,123,1);
			FontDraw(190,70, "打开移动设备失败",20);
			FontDraw(190,40, "没有找到移动硬盘",20);
		}

		glColor3ub(231,123,1);
		FontDraw(-633,360, "以下项目安装在您的本地硬盘驱动器中.您可以自行进行拷贝到您的移动设备中,拷贝过程中,请勿离开此页面,否则有可能拷贝失败,",15);
		FontDraw(-633,340, "拷贝成功后您可以到Steam游戏中心删除本地内容,释放磁盘空间,需要时可以从移动设备恢复.",15);
		FontDraw(-633,207, "本地游戏列表",25);
		FontDraw(189,207, "移动设备",25);
		ShowPic(-633,-150, 0.0f, pt_down.width, pt_down.height, pt_down.texID);

		if(gSysData.conl==0)
		{
			if(gSysData.sg_pos !=1)
			DrawYeRect(-633, 123,668, 50);		
			DrawYeRect(-633, (123-50-12),668, 50);
			DrawYeRect(-633, (123-100-24),668, 50);
			DrawYeRect(-633, (123-150-36),668, 50);
			DrawYeRect(-633, (123-200-48),668, 50);
			DrawYeRect(-633, (123-250-48-110),303, 60);
			
			DrawWhRect(-633, (123-(50*(gSysData.sg_pos-1))-(12*(gSysData.sg_pos-1))),668, 50);
			
			if(gSysData.game_selected==1 ||gSysData.game_selected==2)
			{
				DrawWhRect(-633, (123-250-48-110),303, 60);
				glColor3ub(231,123,1);
				FontDraw(-600,(140-250-48-110), "拷贝到移动设备",20);
				DrawRectangle(-633,(123-250-48-110), 303, 60);
				
			}
			else
				FontDraw(-600,(140-250-48-110), "拷贝到移动设备",20);
			DrawYeRect(189, 123,454, 50);
			glColor3ub(255,255,255);
			glEnable(GL_BLEND);
			FontDraw(220,140, "打开移动设备:shimmer",20);
			
		}	

		else if(gSysData.conl==1)
		{
			DrawWhRect(189, 123,454, 50);
			glColor3ub(231,123,1);
			FontDraw(220,140, "打开移动设备:shimmer",20);
			DrawYeRect(-633, 123,668, 50);		
			DrawYeRect(-633, (123-50-12),668, 50);
			DrawYeRect(-633, (123-100-24),668, 50);
			DrawYeRect(-633, (123-150-36),668, 50);
			DrawYeRect(-633, (123-200-48),668, 50);
			DrawYeRect(-633, (123-250-48-110),303, 60);
			glEnable(GL_BLEND);
			glColor3ub(255,255,255);
			FontDraw(-600,(140-250-48-110), "拷贝到移动设备",20);
		
		}
		if(gSysData.conlture==1)
		{	
			if(gSysData.mg_pos !=1)
				DrawYeRect(189, 123,454, 50);		
			DrawYeRect(189, (123-50-12),454, 50);
			DrawYeRect(189, (123-100-24),454, 50);
			DrawYeRect(189, (123-150-36),454, 50);
			DrawYeRect(189, (123-200-48),454, 50);
			DrawYeRect(189, (123-250-48-110),303, 60);
			DrawYeRect(520, (123-250-48-110),100, 60);
			
			DrawWhRect(189, (123-(50*(gSysData.mg_pos-1))-(12*(gSysData.mg_pos-1))),454, 50);
		
			
			if(gSysData.mgame_selected==2||gSysData.mgame_selected==3)
			{
				if(gSysData.sure_rm ==0)
				{
					DrawWhRect(189, (123-250-48-110),303, 60);
					glColor3ub(231,123,1);
					FontDraw(210,(140-250-48-110), "拷贝到本地磁盘",20);
					DrawRectangle(189,(123-250-48-110), 303, 60);
					glEnable(GL_BLEND);
					glColor3ub(255,255,255);
					FontDraw(535,(140-250-48-110), "删除",20);
				}					
				else if(gSysData.sure_rm==1)
				{
					DrawWhRect(520, (123-250-48-110),100, 60);
					glColor3ub(231,123,1);
					FontDraw(535,(140-250-48-110), "删除",20);
					DrawRectangle(520,(123-250-48-110), 100, 60);
					glEnable(GL_BLEND);
					glColor3ub(255,255,255);
					FontDraw(210,(140-250-48-110), "拷贝到本地磁盘",20);
				}
			}
			else
			{
				glEnable(GL_BLEND);
				FontDraw(210,(140-250-48-110), "拷贝到本地磁盘",20);
				FontDraw(535,(140-250-48-110), "删除",20);
			}
			if(gSysData.conl==0)
			{
				DrawYeRect(189, (123-(50*(gSysData.mg_pos-1))-(12*(gSysData.mg_pos-1))),454, 50);		
			}
			glEnable(GL_BLEND);
			ShowPic(189,-150, 0.0f, yi_down.width, yi_down.height, yi_down.texID);
			int n=1;
			for(vector<SteamLibInfo>::iterator it=mobile_disk_game.begin(); it!=mobile_disk_game.end(); it++)
			{
				if((*it).showname==1)
				{					
					glColor3ub(255,255,255);
					
					if((gSysData.game_move+1)==(*it).number)
					{
						glColor3ub(231,123,1);	
					}	
					
					
					FontDraw(200,(202-(50*n)-(12*n)),(*it).name ,15);
					FontDraw(570,(202-(50*n)-(12*n)),(*it).dis_size ,15);
											
					if(gSysData.conl==0)
					{
						glColor3ub(255,255,255);
						FontDraw(200,(202-(50*n)-(12*n)),(*it).name ,15);
						FontDraw(570,(202-(50*n)-(12*n)),(*it).dis_size ,15);
					}
					n++;	
				}
			}
		}
		
		int t=1;	
	
		for(vector<SteamLibInfo>::iterator it=game_info.begin(); it!=game_info.end(); it++)
		{
			if((*it).showname==1)
			{
				glColor3ub(255,255,255);
				if((gSysData.game_save+1)==(*it).number)
				{
					glColor3ub(231,123,1);	
				}
				FontDraw(-610,(202-(50*t)-(12*t)),(*it).name ,15);
				FontDraw(-40,(202-(50*t)-(12*t)),(*it).dis_size ,15);

				if(gSysData.conl==1)
				{
					glColor3ub(255,255,255);
					FontDraw(-610,(202-(50*t)-(12*t)),(*it).name ,15);
					FontDraw(-40,(202-(50*t)-(12*t)),(*it).dis_size ,15);
				}
				t++;	
			}
		}
		if(get_con==2)
		{
			if(gSysData.cping==1)
			{
				gSysData.getsizeing=1;
				get_con=1;
			}	
			else if(gSysData.mcping==1)
			{	
				gSysData.getmsizeing=1;
				get_con=1;
			}
		}
		
		if(gSysData.cping==1 ||gSysData.mcping==1 )
		{
			ShowPic(xcoord,ycoord, 0, 1920, 1080, downing.texID);
			DrawCpGame();
		}
		
	}
	else
	{
		glColor3ub(231,111,1);
		FontDraw(-250, -20.0f, "芒果玩加游戏暂未上线,敬请期待!",30);
		
		/*
		int i=0;
		for(vector<TSMGame>::iterator it=gSMLocalGameVec.begin(); it!=gSMLocalGameVec.end(); it++)
		{
				if((*it).img_load)
				{
					LoadBmpTex(&(*it).game_tex,(*it).game_img); 
					(*it).img_load = false;
				}
				FontDraw(-200.0f, 300.0f,(*it).game_name,25);
				ShowPic(-200, 100, 0, 200, 100, (*it).game_tex.texID);	
				i++;
		}
		*/
		glColor3ub(231,111,1);
	}
	glDisable(GL_BLEND);
}


