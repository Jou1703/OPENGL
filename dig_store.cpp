#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <vector>

#include "includes.h"
#include "init_ui.h"
#include "dig_store.h"
#include "particle.h"
#include "draw_utils.h"
#include "option.h"

using namespace std;

extern TextureImage bg_tex;
extern GLuint confirm, ret;
extern float xcoord, ycoord, width, heigth;
extern char leftkey_flag, rightkey_flag;

extern TSysData gSysData;
extern GLuint downdown;
vector <TSMGame> gSMGameVec;


TextureImage shm_market,dig_info,dig_info2;
TextureImage happy,dig_market,market,dig_info1;

GLuint shimmerUI[SHIMMERUI_MAXPOS]; 

void ShimmerPicLoad(void)
{
	LoadTgaTex(&shm_market,get_path("shm_market.tga"));
	LoadTgaTex(&dig_info,get_path("dig_info.tga"));
	LoadTgaTex(&dig_info2,get_path("dig_info2.tga"));
	LoadBmpTex(&happy,get_path("happy.bmp"));
	LoadBmpTex(&dig_market,get_path("dig_market.bmp"));
	LoadBmpTex(&market,get_path("market.bmp"));
	LoadBmpTex(&dig_info1,get_path("dig_info1.bmp")); 
}
void DelShimmerPicTex()
{
	DelTexID(&shm_market);
	DelTexID(&dig_info);
	DelTexID(&dig_info2);
	DelTexID(&happy);
	DelTexID(&dig_market);
	DelTexID(&market);
}

void ShimmerRender(void)
{
	ShowPic(xcoord, ycoord, 0.0f, width,heigth, bg_tex.texID);
	TipNote();
	TimeDisplay();
	if (width<1920)
		return;
	
	glEnable(GL_BLEND);
	ShowPic(SHIMMER_MARKET_X, SHIMMER_MARKET_Y, SHIMMER_MARKET_Z, shm_market.width, shm_market.height, shm_market.texID);
	glDisable(GL_BLEND);

	ShowPic(DIG_MARKET_X-gSysData.trans_dig/2.0f, DIG_MARKET_Y-gSysData.trans_dig/2.0f, 0, DIG_MARKET_WIDTH+gSysData.trans_dig, DIG_MARKET_HEIGTH+gSysData.trans_dig, dig_market.texID);
	ShowPic(HAPPY_X-gSysData.trans_web/2.0f, HAPPY_Y-gSysData.trans_web/2.0f, 0, HAPPY_WIDTH+gSysData.trans_web, HAPPY_HEIGTH+gSysData.trans_web, happy.texID);
	ShowPic(MARKET_X-gSysData.trans_store/2.0f, MARKET_Y-gSysData.trans_store/2.0f, 0, MARKET_WIDTH+gSysData.trans_store, MARKET_HEIGTH+gSysData.trans_store, market.texID);

	
	switch(gSysData.set_Shimmer.position)
	{
		case 1:
			
			break;
		case 2:
			
			DrawRectangle(DIG_MARKET_X-gSysData.trans_dig/2.0f, DIG_MARKET_Y-gSysData.trans_dig/2.0f,  DIG_MARKET_WIDTH+gSysData.trans_dig, DIG_MARKET_HEIGTH+gSysData.trans_dig);
			break;
		case 3:
			
			DrawRectangle(HAPPY_X-gSysData.trans_web/2.0f, HAPPY_Y-gSysData.trans_web/2.0f , HAPPY_WIDTH+gSysData.trans_web, HAPPY_HEIGTH+gSysData.trans_web);
			break;
		case 4:
		
			DrawRectangle(MARKET_X-gSysData.trans_store/2.0f, MARKET_Y-gSysData.trans_store/2.0f,  MARKET_WIDTH+gSysData.trans_store, MARKET_HEIGTH+gSysData.trans_store);
			break;
		default:
			break;
	}
}


void DigStoreRender(void)
{
	ShowPic(-960.0f, -540.0f, 0.0f, 1920.0f,1080.0f, bg_tex.texID);
	//ShowPic(-270.0f, 100.0f, 0.0f, 500.0f, 100.0f, dig_info1.texID);
	TipNote();
	TimeDisplay();

	glEnable(GL_BLEND);

	glColor3ub(152,152,152);
	FontDraw(-850, 380.0f,"游戏",30);
	FontDraw(-850, 310.0f,"数字商店",40);

	/*
	DrawYeRect(-515.0f, 290.0f,200, 50);
	glColor3ub(255,255,255);
	FontDraw(-480.0f, 300.0f, "游戏列表",25);
	if(gSysData.smsv_game_sum==0)
	{
		glColor3ub(231,111,1);
		FontDraw(-200.0f, 300.0f, "无",25);
	}
	else
	{
		int i=0;
		for(vector<TSMGame>::iterator it=gSMGameVec.begin(); it!=gSMGameVec.end(); it++)
		{
				//FontDraw(-200.0f, 300.0f-i*120,(*it).game_name,25);
				if((*it).img_load)
				{
					LoadBmpTex(&(*it).game_tex,(*it).game_img); 
					(*it).img_load = false;
					printf("load img!\n");
				}
				if(gSysData.sm_game_pos == (*it).game_num)
				{
					ShowPic(-200+(i%2)*300-20, 230-(i/2)*120-10, 0, 200+40, 100+20, (*it).game_tex.texID);
				}
				else
				ShowPic(-200+(i%2)*300, 230-(i/2)*120, 0, 200, 100, (*it).game_tex.texID);
				i++;	
		}
	}
	*/
		
	FontDraw(-280.0f, 0.0f, "有兴趣的开发者可与闪美官方联系,并参与",20);
	FontDraw(-280.0f, -30.0f,"芒果玩加游戏开发社区,进行交流.",20);
	glDisable(GL_BLEND);
	
}





