#include "includes.h"
#include "particle.h"
#include "joystick.h"
#include "init_ui.h"
#include "dig_store.h"
#include "settings.h"
#include "tv_ui.h"
#include "game_ui.h"
#include "fun.h"
#include "option.h"
#include "net.h"
#include "base64.h"
#include "draw_utils.h"
#include "sound.h"
#include <xcb/xcb.h>
#include "wifi.h"
#include "conf.h"
#include "commu.h"
#include "search.h"
#include "chmod_opt.h"

extern GLuint bubble_image;
extern TSysData gSysData;

extern char xy_pos[128];
/**************************
20160706 
1,support png jpeg
2,add lib /home/steam/.shimmer/lib /etc/ld.so.conf.d/shimmer_lib 
3,fix joystick(mangotv) 
4,add start_cmd.sh
**************************/

char Version[64] = "Ver1.0 Beta";
char UpdateTime[64] ="2016/07/06";
char UpCkeck[64]="20160706";

char wh[50];
char text_xrandr[100];

void MouseClick(int button, int state, int x, int y)
{
	switch(state)
	{
	case GLUT_LEFT_BUTTON:
		sprintf(xy_pos,"x:%f",(float)x-960.0f);//1080.0f-(float)y-540.0f);
		
		break;
	}
}

void GetXcb(int *width, int *height)
{
	int i, screenNum;
	xcb_connection_t *connection = xcb_connect (NULL, &screenNum);
	const xcb_setup_t *setup = xcb_get_setup (connection);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);  

	for (i = 0; i < screenNum; ++i) 
	{
		xcb_screen_next (&iter);
	}
	xcb_screen_t *screen = iter.data;
	*width  = screen->width_in_pixels;
	*height = screen->height_in_pixels;

	gSysData.resolution = GetResolutionPos(screen->width_in_pixels, screen->height_in_pixels);

	if(0)
	{
		printf ("\n");
		printf ("Informations of screen %ld:\n", screen->root);
		printf ("width.........: %d\n", screen->width_in_pixels);
		printf ("height........: %d\n", screen->height_in_pixels);
		printf ("white pixel...: %ld\n", screen->white_pixel);
		printf ("black pixel...: %ld\n", screen->black_pixel);
		printf ("\n");
	}
}



void SysInit()
{
	SystemEx("/home/steam/.shimmer/start_cmd.sh");
	memcpy(gSysData.version,Version,sizeof(Version));
	memcpy(gSysData.update_time,UpdateTime,sizeof(UpdateTime));
	memcpy(gSysData.serial_num,UpCkeck,sizeof(UpCkeck));
	LOG_INFO("update ver:%s",gSysData.serial_num);
	strcpy(gSysData.wlan_name,"wlan");
	strcpy(gSysData.eth_name,"eth");
	gSysData.wifi_device = 1;
	if(GetCardName(gSysData.wlan_name,4)<0)
	{
		gSysData.wifi_device = 0;
	}
	GetCardName(gSysData.wlan_name,4);
	GetCardName(gSysData.eth_name,3);
	LOG_INFO("wlan_name:%s",gSysData.wlan_name);
	LOG_INFO("eth_name:%s",gSysData.eth_name);

	ReadConfInt("sys_snd",&gSysData.sys_snd);
	if(gSysData.sys_snd==1)
		LOG_INFO("snd open");
	ReadConfInt("sys_snd_vol",&gSysData.snd_vol);
	ReadConfInt("bios_key",&gSysData.bios_key);
	ReadConfInt("nv_card",&gSysData.nv_card);
	ReadConfInt("particles_num",&gSysData.particles_num);
	ReadConfInt("steam_game_cp",&gSysData.steam_game_cp);
	ReadConfInt("first_run",&gSysData.first_run);
	ReadConfInt("sys_time_update",&gSysData.sys_time_update);
	ReadConfInt("net_wired",&gSysData.net_wired);

	if(gSysData.bios_key==1)
	{
		if(GetBiosKey()==0)
		{
			gSysData.bios_key_pass=1;
			LOG_INFO("bios_key pass");
		}
		else
		{		
			LOG_ERR("bios_key fail");
			gSysData.sys_snd=0;
		}
	}
	if(gSysData.nv_card==1)
	{
		if(GetGraphicsCards()==0)
		{
			gSysData.nv_card_pass=1;
			LOG_INFO("nv_card pass");
		}	
		else
		{
			LOG_ERR("nv_card fail");
			gSysData.sys_snd=0;
		}
	}
	
	gSysData.state = UPDATE;
	LOG_ERR("state: UPDATE");
	gSysData.set_base.position = 1;
	gSysData.set_game.position = 3;
	gSysData.set_Shimmer.position = 2;
	gSysData.ip_set_pos = 0;
	gSysData.set_setting.position=1;
	gSysData.rate = 1;
	gSysData.off=1;
	gSysData.music_on=1;
	gSysData.set_base.up=0;
	
	char netstr[64]= {0};	
	if(gSysData.net_wired)
	{
							
		sprintf(netstr,"nmcli device disconnect %s",gSysData.wlan_name);
		SystemEx(netstr);
	}
	else
	{					
		sprintf(netstr,"nmcli device disconnect %s",gSysData.eth_name);
		SystemEx(netstr);
	}
	

	//GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.eth_name);
	LOG_INFO("ip:%s",gSysData.ipaddr);
	GetSndPort("eld#0.1");	
}

int main(int argc,char* argv[]) 
{  
	
	SysInit();
	InitResolution();
	GetXcb(&gSysData.width, &gSysData.height);
	thread_create(JoystickCreateThread,NULL);	
	
	if(gSysData.steam_game_cp==1)
	{
		ScanGames(&gSysData.game_num);	
	}
	ScanSMGames();
					
	glutInit(&argc,argv);  
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(gSysData.width, gSysData.height);

	glutCreateWindow(NULL);
	
	if(0)
	{
		const GLubyte* name = glGetString(GL_VENDOR);
		const GLubyte* rend = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* gluVersion = gluGetString(GL_SHADING_LANGUAGE_VERSION);
		printf("Manufacture:%s\n",name);
		printf("rend: %s\n",rend);
		printf("Opengl version:%s\n",version);
		printf("glu :%s\n",gluVersion);
	}

	BaseInit();
	SoundInit();
	BitLoad();
	BasePicPoad();	
	MgtvPicLoad();
	
	SndPlay();
	FontInit();
	UpdateThread();
	glutFullScreen();
	
	glutKeyboardFunc ( KeyBoard );
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(MouseClick);
	glutPassiveMotionFunc(MouseMove);

	glutDisplayFunc(BaseRender);
	glutTimerFunc(5,TimerFun,1);
	glutTimerFunc(5,BaseTimerFun,1);
	//glutReshapeFunc(ReSizeWindow);
	glutMainLoop(); 

	return 0;
}
