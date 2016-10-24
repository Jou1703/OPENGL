#include "includes.h"

#include "init_ui.h"
#include "particle.h"
#include "draw_utils.h"
#include "option.h"
#include <vector>
#include <iostream>

using namespace std;

extern float xcoord, ycoord, width, heigth;
extern TextureImage bg_tex;

extern TSysData gSysData;
extern vector<TSMGame> gSMGameVec;
void LoadingList()
{
	float x = -720.0f;
	if(gSysData.download == 1)
	{
		char str[64] = {0};
		FontDraw(x+400, 200,"系统更新",20);
		FontDraw(x+970, 200,"%%",20);
	
		snprintf(str,sizeof(str),"%d",(int)((gSysData.bpoint*100)/(gSysData.zongchang/(64*1024))));
		FontDraw(x+930, 200,str,20);
		DrawWhRect(x+400,150,600, 20);	
		DrawYeRect(x+400,150,600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024)), 20);	
	}
	else if(gSysData.download == 2)
	{
		char str[64] = {0};
		FontDraw(x+400, 200,"游戏",20);
		if(gSysData.down_load_stop)
			FontDraw(x+450, 200,"下载暂停",20);
		else
			FontDraw(x+450, 200,"下载中",20);
		
		for(vector<TSMGame>::iterator it=gSMGameVec.begin(); it!=gSMGameVec.end(); it++)
		{
				if(gSysData.sm_game_pos == (*it).game_num)
					FontDraw(x+600, 200,(*it).game_name,20);
		}
		if(gSysData.zongchang>=1024)
			snprintf(str,sizeof(str),"%d/%dK",gSysData.bpoint*(64*1024-12)/1024,(gSysData.zongchang/1024));
		FontDraw(x+900, 200,str,20);
		DrawWhRect(x+400,150,600, 20);	
		DrawYeRect(x+400, 150,600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024-12)), 20);	
		
	}
	else if(gSysData.download == 3)
	{
	
		char str[64] = {0};
		FontDraw(x+400, 200,"其他",20);
		FontDraw(x+970, 200,"%%",20);
		//printf("SysData.bpoint:%d zongchang:%d\n",gSysData.bpoint,gSysData.zongchang);
		
		snprintf(str,sizeof(str),"%d",(int)((gSysData.bpoint*100)/(gSysData.zongchang/(64*1024))));
		FontDraw(x+930, 200,str,20);
		DrawWhRect(x+400,150,600, 20);	
		DrawYeRect(x+400,150,600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024)), 20);
	}
	else
	{
		FontDraw(x+500, 200,"没有正在进行中的下载",30);
	}
}
void SysUpDownload()
{
	float x = -720.0f;
	if(gSysData.download == 1)
	{
		char str[64] = {0};
		FontDraw(x+400, 200,"系统更新",20);
		FontDraw(x+970, 200,"%%",20);
	
		snprintf(str,sizeof(str),"%d",(int)((gSysData.bpoint*100)/(gSysData.zongchang/(64*1024))));
		FontDraw(x+930, 200,str,20);
		DrawWhRect(x+400,150,600, 20);	
		DrawYeRect(x+400,150,600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024)), 20);
	}
	else
	{	
		FontDraw(x+500, 200,"没有下载中的系统更新",30);
	}
}
void DownloadList()
{
	float x = -720.0f;
	FontDraw(x+500, 200,"没有已下载的内容",30);
}

void GameDownload()
{
	float x = -720.0f;
	FontDraw(x+500, 200,"没有下载中的游戏",30);
}
void OtherDownload()
{
	float x = -720.0f;
	if(gSysData.download == 3)
	{
		char str[64] = {0};
		FontDraw(x+400, 200,"其他内容",20);
		FontDraw(x+970, 200,"%%",20);
	
		snprintf(str,sizeof(str),"%d",(int)((gSysData.bpoint*100)/(gSysData.zongchang/(64*1024))));
		FontDraw(x+930, 200,str,20);
		DrawWhRect(x+400,150,600, 20);	
		DrawYeRect(x+400,150,600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024)), 20);
	}
	else
	{	
		FontDraw(x+500, 200,"没有下载中的其他内容",30);
	}
}

void DownloadRender(void)
{
	ShowPic(xcoord, ycoord, 0.0f, width,heigth,bg_tex.texID);
	
	if (width<1902)
		return;
	TipNote();
	TimeDisplay();
	glEnable(GL_BLEND);
	
	float x = -720.0f,y=200.0f;
	
	glColor3ub(231,111,1);
	FontDraw(-850.0f, 390.0f, "下载",40);
	FontDraw(x-20.0f, y-140, "内容",20);

	DrawYeRect(x, y,200, 50);
	DrawYeRect(x, y-70,200, 50);
	DrawYeRect(x, y-210,200, 50);
	DrawYeRect(x, y-280,200, 50);
	DrawYeRect(x, y-350,200, 50);

	glColor3ub(255,255,255);
	FontDraw(x+10, y+15, "正在下载",20);
	FontDraw(x+10, y-70+15, "已下载",20);
	FontDraw(x+10, y-210+15, "游戏",20);
	FontDraw(x+10, y-280+15, "系统更新",20);
	FontDraw(x+10, y-350+15, "其他",20);
	
	switch(gSysData.download_pos)
	{
		case 0:
		{
			DrawWhRect(x, y,200, 50);
			glColor3ub(231,111,1);
			FontDraw(x+10, y+15, "正在下载",20);
			LoadingList();
		}
			break;
		case 1:
		{
			DrawWhRect(x, y-70,200, 50);
			glColor3ub(231,111,1);
			FontDraw(x+10, y-70+15, "已下载",20);
			DownloadList();
			
		}
			break;
		case 2:
		{	
			DrawWhRect(x, y-210,200, 50);
			glColor3ub(231,111,1);
			FontDraw(x+10, y-210+15, "游戏",20);
			GameDownload();
			
		}
			break;
		case 3:
		{
			DrawWhRect(x, y-280,200, 50);
			glColor3ub(231,111,1);
			FontDraw(x+10, y-280+15, "系统更新",20);
			
			SysUpDownload();
		}
			break;
		case 4:
		{
			DrawWhRect(x, y-350,200, 50);
			glColor3ub(231,111,1);
			FontDraw(x+10, y-350+15, "其他",20);
			OtherDownload();			
		}
			break;
	}
	
	
	
	
	glDisable(GL_BLEND);

}
