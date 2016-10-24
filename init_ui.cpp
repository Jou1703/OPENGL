#include "includes.h"
#include "particle.h"
#include "joystick.h"
#include "init_ui.h"
#include "dig_store.h"
#include "tv_ui.h"
#include "game_ui.h"
#include "fun.h"
#include "settings.h"
#include "draw_utils.h"
#include "load3ds.h"
#include "softkey.h"
#include "option.h"
#include "all_path.h"
#include "download.h"
#include "commu.h"
#include "sound.h"
#include <sys/types.h>

#include <unistd.h>

#define false 0
#define true 1

extern TSysData gSysData;

TextureImage button_steamos, button_shimmer, button_TV;
TextureImage shimmer_market1, steamos1, tv1;
TextureImage bg_tex;
TextureImage tip,internet;

TextureImage off_on,settings_on,game_store_on,download_on;
TextureImage shutdown,settings,game_store,download;
TextureImage music,music_on,music_turn_on,music_turn_off;
TextureImage transparent;
TextureImage mark;


GLint fogMode;
GLubyte off_dark = 0;
static int count = 0;
float xcoord, ycoord, width, heigth;

extern char wh[50];
extern float depth;


void EscAnimation(void);
void EnterAnimation(void);
void TipNote();

char xy_pos[128];
static float mouse_x = 0;
static float mouse_y = 0;


void MouseMove(int x, int y)
{
	//mouse_x=x;
	//mouse_y=1080-y;
	mouse_x=(float)x-960;
	mouse_y=540-(float)y;

	//printf("x = %d y = %d\n",mouse_x,mouse_y);
	sprintf(xy_pos,"x:%f y:%f",mouse_x,mouse_y);
}

void BasePicPoad()
{	
	LoadBmpTex(&bg_tex ,get_path("bg.bmp"));
	LoadBmpTex(&button_steamos ,get_path("steamos.bmp"));
	LoadBmpTex(&button_shimmer ,get_path("shm_market.bmp"));
	LoadBmpTex(&button_TV ,get_path("tv.bmp"));	
	LoadBmpTex(&steamos1 ,get_path("steamos1.bmp"));
	LoadBmpTex(&shimmer_market1 ,get_path("shm_market1.bmp"));
	LoadBmpTex(&tv1 ,get_path("tv1.bmp"));
	LoadTgaTex(&tip,get_path("tip.tga"));
	LoadTgaTex(&internet,get_path("internet.tga"));
	LoadTgaTex(&shutdown,get_path("off.tga"));
	
	LoadBmpTex(&off_on ,get_path("off_on.bmp"));
	LoadTgaTex(&settings,get_path("settings.tga"));
	LoadBmpTex(&settings_on ,get_path("settings_on.bmp"));
	LoadTgaTex(&download,get_path("download.tga"));
	LoadBmpTex(&download_on,get_path("download_on.bmp"));
	LoadTgaTex(&game_store,get_path("game_store.tga"));
	LoadBmpTex(&game_store_on ,get_path("game_store_on.bmp"));
	LoadBmpTex(&music_on ,get_path("music_on.bmp"));
	LoadTgaTex(&music,get_path("music.tga"));
	LoadTgaTex(&music_turn_on,get_path("music_turn_off.tga"));
	LoadBmpTex(&music_turn_off ,get_path("music_turn_off.bmp"));
	LoadTgaTex(&transparent ,get_path("bg.tga"));
	LoadTgaTex(&mark ,get_path("yellow_cir.tga"));	
}

char logn[128];
char uid[8];
void BaseInit()
{  
	glEnable(GL_TEXTURE_2D);
	
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //指定混合函数
	

	glEnable(GL_FOG);//启用雾功能
	GLfloat fogColor[4] = {0.3f, 0.3f, 0.3f, 0.3f};                       
	glFogi(GL_FOG_MODE, GL_LINEAR);  
	glFogfv(GL_FOG_COLOR, fogColor);  
	glFogf(GL_FOG_DENSITY, 0.05f);  
	glHint(GL_FOG_HINT, GL_NICEST);  
	glFogf(GL_FOG_START, 1.0f);  
	
	glFogf(GL_FOG_END, 5.7f);
	

	glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);//背景色
	
	//memcpy(logn,getlogin(),10);
	//sprintf(uid,"%d\n",geteuid());
	//struct stat st_tgz,st_app;
	//setuid(1001);
	//printf("longn:%s euid:%d uid:%d\n",getlogin(),geteuid(),getuid());

	load3ds();
} 

float trans_shimmer;
float trans_steamos;
float trans_mgtv;
//float trans_set;
float trans_font;
float trans_font_off;
static char time_dis=1;
static double tex_part=0.5;

extern int joy_exist;



void BaseRender()
{  
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
	
	glMatrixMode( GL_PROJECTION);  
	glLoadIdentity();  
	glOrtho(-SCREEN_WIDTH/2,SCREEN_WIDTH/2,-SCREEN_HEIGTH/2,SCREEN_HEIGTH/2,-100.0f, 100.0f);
	glMatrixMode( GL_MODELVIEW );  
	glLoadIdentity();   	

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);

	ShowPartPic(0, 0.5, 0, 1.0f, GAME_X, GAME_Y, 0, game_store.width/2, game_store.height, game_store.texID);
	glDisable(GL_BLEND);
	//glDisable(GL_MULTISAMPLE);
	
	if(gSysData.bios_key==1)
	{
		if(gSysData.bios_key_pass==0)
		{
			glEnable(GL_BLEND);
			glColor3ub(255,0,0);
			FontDraw(-225, 0,"运行环境错误 错误代码 01",25);
			glDisable(GL_BLEND);
			glutSwapBuffers(); 
			return;
		}	
	}
	if(gSysData.nv_card==1)
	{
		if(gSysData.nv_card_pass==0)
		{
			glEnable(GL_BLEND);
			glColor3ub(255,0,0);
			FontDraw(-225, 0,"运行环境错误 错误代码 02",25);
			glDisable(GL_BLEND);
			glutSwapBuffers(); 
			return;
		}
	}
	//gSysData.state = TEST;
	
	
	switch(gSysData.state)
	{
	
		case TEST:
		/*
			ShowJpg(BACKGROUND_X, BACKGROUND_Y, BACKGROUND_Z, g_tex.width, g_tex.height,g_tex.texID);
			glEnable(GL_BLEND);
			ShowPic(mouse_x, mouse_y, BACKGROUND_Z, png_tex1.width, png_tex1.height,png_tex1.texID);
			ShowPic(BACKGROUND_X+310, BACKGROUND_Y, BACKGROUND_Z, png_tex2.width, png_tex2.height,png_tex2.texID);
			ShowPic(BACKGROUND_X+310+700, BACKGROUND_Y, BACKGROUND_Z, png_tex3.width, png_tex3.height,png_tex3.texID);
			glDisable(GL_BLEND);
			 */
			break;
			
		case BASE:
			
			ShowPic(BACKGROUND_X, BACKGROUND_Y, BACKGROUND_Z-off_dark, bg_tex.width, bg_tex.height,bg_tex.texID);
			glFogf(GL_FOG_START, 1.0f);  
			glFogf(GL_FOG_END, 80.7f);  
			glDisable(GL_FOG);
			if (off_dark!=4)
			{
				Show3ds(gSysData.particles_num);
			}
			glEnable(GL_FOG);
			glFogf(GL_FOG_START, 1.0f);
			glFogf(GL_FOG_END, 5.7f);
		
			
			if (time_dis)
			{
				TimeDisplay();
				
			}
			TipNote();
			
			
			
			//
		
			{
			ShowPic(SHIMMER_X-trans_shimmer/2.0f, SHIMMER_Y-trans_shimmer/2.0f, 0-off_dark, SHIMMER_WIDTH+trans_shimmer, SHIMMER_HEIGTH+trans_shimmer, button_shimmer.texID);
			ShowPic(STEAMOS_X-trans_steamos/2.0f, STEAMOS_Y-trans_steamos/2.0f, 0-off_dark, STEAMOS_WIDTH+trans_steamos, STEAMOS_HEIGTH+trans_steamos, button_steamos.texID);
			ShowPic(TV_X-trans_mgtv/2.0f, TV_Y-trans_mgtv/2.0f, 0-off_dark, TV_WIDTH+trans_mgtv, TV_HEIGTH+trans_mgtv, button_TV.texID);


			glEnable(GL_BLEND);
			
			ShowPartPic(0, 0.5, 0, 1.0f, GAME_X, GAME_Y, 0, game_store.width/2, game_store.height, game_store.texID);
			ShowPartPic(0.5, tex_part, 0, 1.0f, GAME_X+game_store.width/2, GAME_Y,0, (tex_part-0.5)*game_store.width, game_store.height, game_store.texID);

			ShowPartPic(0, 0.5, 0, 1.0f, SETTINGS_X+trans_font, SETTINGS_Y,0, settings.width/2, settings.height, settings.texID);
			ShowPartPic(0.5, tex_part, 0, 1.0f, SETTINGS_X+settings.width/2+trans_font, SETTINGS_Y,0, (tex_part-0.6)*game_store.width, settings.height, settings.texID);
	
			
			
			ShowPartPic(0, 0.5, 0, 1.0f, DOWNLOAD_X-3*trans_font_off, DOWNLOAD_Y,0, download.width/2, download.height, download.texID);
			ShowPartPic(0.5, tex_part, 0, 1.0f, DOWNLOAD_X-2*trans_font_off, DOWNLOAD_Y,0, (tex_part-0.5)*download.width, download.height, download.texID);


			if (gSysData.music_on)
			{
				ShowPartPic(0, 0.5, 0, 1.0f, INTERNET_X-2*trans_font_off, INTERNET_Y, 0, music.width/2, music.height, music.texID);
				ShowPartPic(0.5, tex_part, 0, 1.0f, INTERNET_X-trans_font_off, INTERNET_Y,0, (tex_part-0.5)*music.width, music.height, music.texID);
			}
			else
			{
				ShowPartPic(0, 0.5, 0, 1.0f, INTERNET_X-2*trans_font_off, INTERNET_Y, 0, music_turn_on.width/2, music_turn_on.height, music_turn_on.texID);
				ShowPartPic(0.5, tex_part, 0, 1.0f, INTERNET_X-trans_font_off, INTERNET_Y,0, (tex_part-0.5)*music_turn_on.width, music_turn_on.height, music_turn_on.texID);
			}

			ShowPartPic(0, 0.5, 0, 1.0f, SHUTDOWN_X-trans_font_off, SHUTDOWN_Y, 0, shutdown.width/2, shutdown.height, shutdown.texID);
			ShowPartPic(0.5, tex_part, 0, 1.0f, SHUTDOWN_X-trans_font_off+shutdown.width/2, SHUTDOWN_Y, 0, (tex_part-0.5)*shutdown.width, shutdown.height, shutdown.texID);

			glDisable(GL_BLEND);

			}

			if (gSysData.set_base.position == 1)
			{
				ShowPic(SHIMMER_X-trans_shimmer/2.0f, SHIMMER_Y-trans_shimmer/2.0f, 0, SHIMMER_WIDTH+trans_shimmer, SHIMMER_HEIGTH+trans_shimmer, shimmer_market1.texID);
				DrawGrWave(SHIMMER_X-trans_shimmer/2.0f, SHIMMER_Y-trans_shimmer/2.0f,  SHIMMER_WIDTH+trans_shimmer, SHIMMER_HEIGTH+trans_shimmer);
				
			
			}
			
			else if (gSysData.set_base.position == 2)
			{		
				ShowPic(TV_X-trans_mgtv/2.0f, TV_Y-trans_mgtv/2.0f, 0, TV_WIDTH+trans_mgtv, TV_HEIGTH+trans_mgtv, tv1.texID);
				DrawRectangle(TV_X-trans_mgtv/2.0f, TV_Y-trans_mgtv/2.0f,  TV_WIDTH+trans_mgtv, TV_HEIGTH+trans_mgtv);			
			}
			
			else if (gSysData.set_base.position == 3)
			{
				ShowPic(STEAMOS_X-trans_steamos/2.0f, STEAMOS_Y-trans_steamos/2.0f, 0, STEAMOS_WIDTH+trans_steamos, STEAMOS_HEIGTH+trans_steamos, steamos1.texID);
				DrawBlWave(STEAMOS_X-trans_steamos/2.0f, STEAMOS_Y-trans_steamos/2.0f,  STEAMOS_WIDTH+trans_steamos, STEAMOS_HEIGTH+trans_steamos);
				
			
			}
			else if (gSysData.set_base.position == 4)
			{
				ShowPartPic(0, 0.5, 0, 1.0f, GAME_X, GAME_Y, 0, 210/2, 82, game_store_on.texID);
				ShowPartPic(0.5, tex_part, 0, 1.0f, GAME_X+210/2, GAME_Y,0, (tex_part-0.5)*210, 82, game_store_on.texID);
				DrawRectangle(GAME_X, GAME_Y,  210/2+(tex_part-0.5)*210, 82);
			}
			else if (gSysData.set_base.position == 5)
			{
				ShowPic(SETTINGS_X+trans_font, SETTINGS_Y, 0, settings_on.width, settings_on.height, settings_on.texID);
				DrawRectangle(SETTINGS_X+trans_font, SETTINGS_Y,  170, 82);
			}
			else if (gSysData.set_base.position == 6)
			{
				ShowPic(DOWNLOAD_X-3*trans_font_off, DOWNLOAD_Y, 0, download_on.width, download_on.height, download_on.texID);
				DrawRectangle(DOWNLOAD_X-3*trans_font_off, DOWNLOAD_Y,  174, 82);
			}
			

			else if(gSysData.set_base.position == 7)
			{
				if (gSysData.music_on)
				{
					ShowPic(INTERNET_X-2*trans_font_off, INTERNET_Y, 0, music_on.width, music_on.height, music_on.texID);
					DrawRectangle(INTERNET_X-2*trans_font_off, INTERNET_Y, music_on.width, music_on.height);
				}
				else
				{
					ShowPic(INTERNET_X-2*trans_font_off, INTERNET_Y, 0, music_turn_off.width, music_turn_off.height, music_turn_off.texID);
					DrawRectangle(INTERNET_X-2*trans_font_off, INTERNET_Y, music_turn_off.width, music_turn_off.height);
				}
			}

		

			else if(gSysData.set_base.position == 8)
			{
				ShowPic(SHUTDOWN_X-trans_font_off, SHUTDOWN_Y, 0, off_on.width, off_on.height, off_on.texID);
				DrawRectangle(SHUTDOWN_X-trans_font_off, SHUTDOWN_Y, 146, 82);
			}
			
			else if (gSysData.set_base.position == 17)
			{

				off_dark=4;

				
				DrawRectangle(0, 0, 220, 170);
				
				DrawYeRect(-240, -150,480, 400);
				DrawWhRect(-170, 100,340, 60);
				DrawWhRect(-170, 100-90,340, 60);
				DrawWhRect(-170, 100-180,340, 60);

				glEnable(GL_BLEND);
				glColor3ub(231,111,1);
				FontDraw(-25, 100+20,"注销",25);
				FontDraw(-25, 100-90+20,"重启",25);
				FontDraw(-25, 100-180+20,"关机",25);
			
				if (gSysData.off == 1)
				{
					ShowPic(-100, 100-90+20, 0, 24, 24, mark.texID);
				}	
				else if (gSysData.off == 2)
				{
					ShowPic(-100, 100-180+20, 0, 24, 24, mark.texID);
					
				}
				glDisable(GL_BLEND);
				
			}
			
			if (gSysData.set_base.position == 8)
			{
				off_dark=0;
			}
			
			if(gSysData.set_Shimmer.esc || gSysData.set_setting.esc || gSysData.set_game.esc ||gSysData.set_tv.esc || gSysData.set_download.esc)
			{
				
				ShowPic(xcoord, ycoord, 0.0f, width,heigth, bg_tex.texID);
			}
			
			break;
		case TV:
			
			//MangoRender();
			break;
		case SHIMMER:

			ShimmerRender();
			
			break;

		//
		case VOICESET:
		case SOFTKEY:
		case IPSET:
		case SETTINGS:
		case RESULOTION_SET:
		case IPSET_WRIED_WRIELESS:
		case IPSET_WRIED_MANUAL_AUTO:
		case IPSET_WRIED_MANUAL:
		case IPSET_WRIELESS:
		case IPSET_WRIELESS_MANUAL_AUTO:
		case IPSET_WRIED_AUTO:
		case IPSET_WRIELESS_AUTO:
		case IPSET_WRIELESS_MANUAL:
		case IPSET_WRIELESS_SSID:
		case IPSET_WRIELESS_KEY:
		case IPSET_AIISC_KEY:
		case WIFI_CONNECTING:
		
		     
			SettingsSystemRender();
			break;
		case GAME:
			
			GameRender();
			break;	
		case DOWNLOAD:
			DownloadRender();
			break;
		case DIG_MARKET:
			DigStoreRender();
			break;
		case UPDATE:
			ShowPic(BACKGROUND_X, BACKGROUND_Y, BACKGROUND_Z-off_dark, bg_tex.width, bg_tex.height,bg_tex.texID);
			glEnable(GL_BLEND);
			glColor3ub(231,111,1);
			FontDraw(-70, 100+20,"系统更新",25);				
			
			if(gSysData.update_way)
			{
				if(gSysData.zongchang>0)
					DrawWhRect(-800,-100,1600, 10);	
				if(gSysData.zongchang>=1024)
				{
					char str[64] = {0};
					glColor3ub(231,111,1);
					FontDraw(770, -130,"%%",20);
					snprintf(str,sizeof(str),"%ld",(gSysData.bpoint*100/1024)/(gSysData.zongchang/1024));
					FontDraw(740, -130,str,20);
					DrawYeRect(-800,-100,1600*(gSysData.bpoint/1024)/(gSysData.zongchang/1024), 10);
				}
			}
			else
			{
				if(gSysData.zongchang>0)
					DrawWhRect(-800,-100,1600, 10);	
				if(gSysData.zongchang>=(8*1024))
				{
					char str[64] = {0};
					glColor3ub(231,111,1);
					FontDraw(770, -130,"%%",20);
					snprintf(str,sizeof(str),"%ld",(gSysData.bpoint*100)/(gSysData.zongchang/(64*1024)));
					FontDraw(740, -130,str,20);
					DrawYeRect(-800,-100,1600*(gSysData.bpoint)/(gSysData.zongchang/(64*1024)), 10);
				}	
			}		
			break;		
		default:break;		
	}
	glEnable(GL_BLEND);
	glColor3ub(231,111,1);

	//FontDraw(-800,500,uid,30);
	glutSwapBuffers(); 
}


extern char key_flash;
static int joy_time=0;
static int key_time;

static int heart_time;

char depth_flag;

void BaseTimerFun(int value)
{
	
	if (gSysData.state == BASE)
	{
		if (gSysData.set_base.position == 1)
		{
			if (trans_shimmer<20.0)
				trans_shimmer+=2.0f;
			if (trans_mgtv>0.0)
				trans_mgtv-=2.0f;
			if (trans_steamos>0.0)
				trans_steamos-=2.0f;
			if (trans_font>0.0)
				trans_font-=10.0f;
			if (trans_font_off>0.0)
				trans_font_off-=10.0f;
			if (trans_font==0)
				time_dis=1;	
			if (tex_part>0.5f)
				tex_part-=0.1f;
		}
		else if (gSysData.base_trans == SHIMMER_MGTV)
		{
			if (trans_shimmer>0.0)
				trans_shimmer-=2.0f;
			if (trans_mgtv<20.0)
				trans_mgtv+=2.0f;
		}
		else if (gSysData.base_trans == MGTV_STEAMOS)
		{
			if (trans_mgtv>0.0)
				trans_mgtv-=2.0f;
			if (trans_steamos<20.0)
				trans_steamos+=2.0f;
		}
		else if (gSysData.base_trans == STEAMOS_SHIMMER)
		{
			
			if (trans_shimmer<20.0)
				trans_shimmer+=2.0f;
		}
		else if (gSysData.base_trans == STEAMOS_MGTV)
		{
			if (trans_steamos>0.0)
				trans_steamos-=2.0f;
			if (trans_mgtv<20.0)
				trans_mgtv+=2.0f;
		}
		else if (gSysData.base_trans == MGTV_SHIMMER)
		{
			if (trans_mgtv>0.0)
				trans_mgtv-=2.0f;
			if (trans_shimmer<20.0)
				trans_shimmer+=2.0f;
		}
		else if (gSysData.base_trans == SHIMMER_STEAMOS)
		{
			if (trans_shimmer>0.0)
				trans_shimmer-=2.0f;
			if (trans_steamos<20.0)
				trans_steamos+=2.0f;
		}
		
		else if (gSysData.base_trans == SHIMMER_GAME)
		{
			if (trans_shimmer>0.0)
				trans_shimmer-=2.0f;
			if (trans_font<120.0)
				trans_font+=10.0f;
			if (trans_font_off<80.0)
				trans_font_off+=10.0f;
			time_dis=0;	
			if (tex_part<1.0f)
				tex_part+=0.1f;
			
		}
		else if (gSysData.base_trans == MGTV_GAME)
		{
			if (trans_mgtv>0.0)
				trans_mgtv-=2.0f;
			if (trans_font<120.0)
				trans_font+=10.0f;
			if (trans_font_off<80.0)
				trans_font_off+=10.0f;
			time_dis=0;	
			if (tex_part<1.0f)
				tex_part+=0.1f;	
		}
		else if (gSysData.base_trans == STEAMOS_GAME)
		{
			if (trans_steamos>0.0)
			{	
				trans_steamos-=2.0f;
			}
			if (trans_font<120.0)
				trans_font+=10.0f;
			if (trans_font_off<80.0)
				trans_font_off+=10.0f;
			time_dis=0;	
			if (tex_part<1.0f)
				tex_part+=0.1f;
			
		}
		else if (gSysData.base_trans == GAME_SHIMMER)
		{
			/*
			if (trans_shimmer<20.0)
				trans_shimmer+=2.0f;
			if (trans_font>0.0)
				trans_font-=5.0f;
			*/
		}
		else if (gSysData.base_trans == SET_SHIMMER)
		{
			
			//if (trans_shimmer<20.0)
			//	trans_shimmer+=2.0f;
			
		}
		else if(gSysData.base_trans == SHUTDOWN_SHIMMER)
		{
			//if (trans_shimmer<20.0)
			//	trans_shimmer+=2.0f;
		}

	}
	else if (gSysData.state == SETTINGS)
	{
		
		if (gSysData.set_setting.position==1)
		{
			if (gSysData.trans_update<20.0f)
				gSysData.trans_update+=2.0f;
		}	
		if (gSysData.set_trans == UPDATE_IPSET)
		{
			if (gSysData.trans_update>0.0f)
				gSysData.trans_update-=2.0f;
			if (gSysData.trans_ipset<20.0f)
				gSysData.trans_ipset+=2.0f;
		}
		else if (gSysData.set_trans == IPSET_VOICE)
		{
			if (gSysData.trans_ipset>0.0f)
				gSysData.trans_ipset-=2.0f;
			if (gSysData.trans_voice<20.0f)
				gSysData.trans_voice+=2.0f;
		}
		else if (gSysData.set_trans == VOICE_BLUETOOTH)
		{
			if (gSysData.trans_voice>0.0f)
				gSysData.trans_voice-=2.0f;
			if (gSysData.trans_bluetooth<20.0f)
				gSysData.trans_bluetooth+=2.0f;
		}
		else if (gSysData.set_trans == BLUETOOTH_VOICE)
		{
			if (gSysData.trans_bluetooth>0.0f)
				gSysData.trans_bluetooth-=2.0f;
			if (gSysData.trans_voice<20.0f)
				gSysData.trans_voice+=2.0f;
		}
		else if (gSysData.set_trans == VOICE_IPSET)
		{
			if (gSysData.trans_voice>0.0f)
				gSysData.trans_voice-=2.0f;
			if (gSysData.trans_ipset<20.0f)
				gSysData.trans_ipset+=2.0f;
		}
		else if (gSysData.set_trans == IPSET_UPDATE)
		{
			if (gSysData.trans_ipset>0.0f)
				gSysData.trans_ipset-=2.0f;
			if (gSysData.trans_update<20.0f)
				gSysData.trans_update+=2.0f;
		}	
		
	}
	else if (gSysData.state == SHIMMER)
	{
		if (gSysData.set_Shimmer.position==2)
		{
			if (gSysData.trans_dig<20.0f)
				gSysData.trans_dig+=2.0f;
		}
		if (gSysData.shm_trans == DIG_WEB)
		{
			if (gSysData.trans_dig>0.0f)
				gSysData.trans_dig-=2.0f;
			if (gSysData.trans_web<20.0f)
				gSysData.trans_web+=2.0f;
		}
		else if (gSysData.shm_trans == WEB_STORE)
		{
			if (gSysData.trans_web>0.0f)
				gSysData.trans_web-=2.0f;
			if (gSysData.trans_store<20.0f)
				gSysData.trans_store+=2.0f;
		}
		else if (gSysData.shm_trans == STORE_WEB)
		{
			if (gSysData.trans_store>0.0f)
				gSysData.trans_store-=2.0f;
			if (gSysData.trans_web<20.0f)
				gSysData.trans_web+=2.0f;
		}
		else if (gSysData.shm_trans == WEB_DIG)
		{
			if (gSysData.trans_web>0.0f)
				gSysData.trans_web-=2.0f;
			if (gSysData.trans_dig<20.0f)
				gSysData.trans_dig+=2.0f;
		}
		
	}
	
	heart_time++;
	if(heart_time == 5)
	{
		heart_time = 0;
		if(depth_flag)
		{
			depth += 0.4f;
			if(depth > 0.0f)
				depth_flag = 0;
		}						
		else
		{
			depth -= 0.4f;
			if(depth < -6.0f)
				depth_flag = 1;
		}
	}		
	glutTimerFunc(5,BaseTimerFun,1);
}

void TimerFun(int value)
{
	if (gSysData.state==IPSET_WRIED_MANUAL || gSysData.state==SOFTKEY)
	{
		key_time++;
		if (key_time>50)
		{
			key_flash =1;
			if (key_time>=100)		
				key_time=0;
		}
		else
		{
			key_flash =0;
		}	
	}

	joy_time+=1;
	if (joy_time>=500)
	{
		joy_exist = IsFileExist(JS0_PATH);
		joy_time=0;
	}
	
	if(gSysData.getsizeing==1)
	{
		GetgameSize();	
	}
	else if(gSysData.getmsizeing==1)
	{
		GetSgameSize();	
	}
	if(gSysData.update_way)
	{
		GetPackSize();
	}
	
	if (gSysData.state == SHIMMER || gSysData.state == TV|| gSysData.state == SETTINGS || gSysData.state == GAME || gSysData.state == DOWNLOAD)
	{
		EnterAnimation();
	}
	else if ( gSysData.state == BASE )
	{
		EscAnimation();
	}
	BaseRender();
	
	if(gSysData.shm_tex_load==1)
	{
		ShimmerPicLoad();
		gSysData.shm_tex_load++;
	}
	if(gSysData.set_tex_load==1)
	{
		SettingsPicLoad();
		gSysData.set_tex_load++;
	}
	if(gSysData.game_tex_load==1)
	{
		GamePicLoad();
		gSysData.game_tex_load++;
	}
	glutTimerFunc(5,TimerFun,1);
}

void ResetCoord(void)
{
	count = 0;
}

void EnterAnimation(void)
{
	float x,y;	
	count++;
	if(count >= MAX_INCREASE) 
		count = MAX_INCREASE;
	
	if(gSysData.state == SHIMMER)
	{
		x = SHIMMER_X;
		y = SHIMMER_Y;

		width = SHIMMER_WIDTH + (SCREEN_WIDTH-SHIMMER_WIDTH)/MAX_INCREASE*count;
		heigth = SHIMMER_HEIGTH + (SCREEN_HEIGTH-SHIMMER_HEIGTH)/MAX_INCREASE*count;	
	}
	if(gSysData.state == TV)
	{
		x = TV_X;
		y = TV_Y;

		width = TV_WIDTH + (SCREEN_WIDTH-TV_WIDTH)/MAX_INCREASE*count;
		heigth = TV_HEIGTH + (SCREEN_HEIGTH-TV_HEIGTH)/MAX_INCREASE*count;	
	}
	if(gSysData.state == SETTINGS)
	{
		x = SETTINGS_X+160;
		y = SETTINGS_Y;

		width = SETTINGS_WIDTH + (SCREEN_WIDTH-SETTINGS_WIDTH)/MAX_INCREASE*count;
		heigth = SETTINGS_HEIGTH + (SCREEN_HEIGTH-SETTINGS_HEIGTH)/MAX_INCREASE*count;
	}
	if(gSysData.state == GAME)
	{
		x = GAME_X;
		y = GAME_Y;

		width = GAME_WIDTH + (SCREEN_WIDTH-GAME_WIDTH)/MAX_INCREASE*count;
		heigth = GAME_HEIGTH + (SCREEN_HEIGTH-GAME_HEIGTH)/MAX_INCREASE*count;
	}
	if(gSysData.state == DOWNLOAD)
	{
		x = DOWNLOAD_X-160;
		y = DOWNLOAD_Y;

		width = DOWNLOAD_WIDTH + (SCREEN_WIDTH-DOWNLOAD_WIDTH)/MAX_INCREASE*count;
		heigth = DOWNLOAD_WIDTH + (SCREEN_HEIGTH-DOWNLOAD_WIDTH)/MAX_INCREASE*count;
	}
	

		
	if (xcoord>-960.0f)
		xcoord = x-((x+960)/10.0f)*count;
	if(ycoord>-540.0f)
		ycoord = y-((y+540)/10.0f)*count;
	
}


void EscAnimation(void)
{
	float x,y;	
	count--;
	if(count <= 0) 
		count = 0;
	
	if(gSysData.set_setting.esc)
	{
		x = SETTINGS_X+160;
		y = SETTINGS_Y;
		width =  SETTINGS_WIDTH + (SCREEN_WIDTH-SETTINGS_HEIGTH)/MAX_INCREASE*count;
		heigth = SETTINGS_HEIGTH + (SCREEN_HEIGTH-SETTINGS_HEIGTH)/MAX_INCREASE*count;
		if (width == SETTINGS_WIDTH)
			width=0;

		if (xcoord<SETTINGS_X+160)
			xcoord = -960.0f+((x+960)/10.0f)*(10-count);
		if(ycoord<SETTINGS_Y)
			ycoord = -540.0f+((y+540)/10.0f)*(10-count);
				
	}
	else if(gSysData.set_Shimmer.esc)
	{
		x = SHIMMER_X;
		y = SHIMMER_Y;

		width =  SHIMMER_WIDTH + (SCREEN_WIDTH-SHIMMER_HEIGTH)/MAX_INCREASE*count;
		heigth = SHIMMER_HEIGTH + (SCREEN_HEIGTH-SHIMMER_HEIGTH)/MAX_INCREASE*count;
		if (width == SHIMMER_WIDTH)
			width=0;
		if (xcoord<SHIMMER_X)
			xcoord = -960.0f+((x+960)/10.0f)*(10-count);
		if(ycoord<SHIMMER_Y)
			ycoord = -540.0f+((y+540)/10.0f)*(10-count);
	}
	else if(gSysData.set_tv.esc)
	{
		x = TV_X;
		y = TV_Y;

		width =  TV_WIDTH + (SCREEN_WIDTH-TV_HEIGTH)/MAX_INCREASE*count;
		heigth = TV_HEIGTH + (SCREEN_HEIGTH-TV_HEIGTH)/MAX_INCREASE*count;
		if (width == TV_WIDTH)
			width=0;
		if (xcoord<TV_X)
			xcoord = -960.0f+((x+960)/10.0f)*(10-count);
		if(ycoord<TV_Y)
			ycoord = -540.0f+((y+540)/10.0f)*(10-count);
	}
	else if(gSysData.set_game.esc)
	{
		x = GAME_X;
		y = GAME_Y;

		width =  GAME_WIDTH + (SCREEN_WIDTH-GAME_HEIGTH)/MAX_INCREASE*count;
		heigth = GAME_HEIGTH + (SCREEN_HEIGTH-GAME_HEIGTH)/MAX_INCREASE*count;
		if (width == GAME_WIDTH)
			width=0;
		if (xcoord<GAME_X)
			xcoord = -960.0f+((x+960)/10.0f)*(10-count);
		if(ycoord<GAME_Y)
			ycoord = -540.0f+((y+540)/10.0f)*(10-count);
	}
	else if(gSysData.set_download.esc)
	{
		x = DOWNLOAD_X-160;
		y = DOWNLOAD_Y;

		width =  DOWNLOAD_WIDTH + (SCREEN_WIDTH-DOWNLOAD_WIDTH)/MAX_INCREASE*count;
		heigth = DOWNLOAD_HEIGTH + (SCREEN_HEIGTH-DOWNLOAD_HEIGTH)/MAX_INCREASE*count;
		if (width == DOWNLOAD_WIDTH)
			width=0;
		if (xcoord<DOWNLOAD_X-160)
			xcoord = -960.0f+((x+960)/10.0f)*(10-count);
		if(ycoord<DOWNLOAD_Y)
			ycoord = -540.0f+((y+540)/10.0f)*(10-count);
	}
}

void TimeDisplay(void)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	char *s = asctime(timenow);  //Thu Jul 30 14:52:03 2015

	float x;
	float y = 415.0f;
	char time_str[6];
		x=480.0f;
      
	if(gSysData.state == BASE)
	 ;
	else
	{
                x = 680.0f;		
	}
	 memcpy(time_str,s+10,6);
	glEnable(GL_BLEND);
	glColor3ub(152,152,152);
	FontDraw(x, y,time_str,40);
	glDisable(GL_BLEND);
}

void TipNote()
{
	float y = -470.0f;
	glEnable(GL_BLEND);
	ShowPartPic(0,1.0f, 0.5f, 1.0f, -870.0f, y, 0, tip.width, tip.height/2, tip.texID);
	ShowPartPic(0,1.0f, 0.0f, 0.5f, 720.0f,  y, 0, tip.width, tip.height/2, tip.texID);
	
	glDisable(GL_BLEND);
}















