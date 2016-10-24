#include "includes.h"

#include "joystick.h"
#include "init_ui.h"
#include "dig_store.h"
#include "game_ui.h"
#include "settings.h"
#include "fun.h"
#include "sound.h"
#include "tv_ui.h"
#include "net.h"
#include "fun.h"
#include "option.h"
#include "wifi.h"
#include "softkey.h"
#include "conf.h"
#include "all_path.h"
#include "commu.h"
#include "search.h"
#include <vector>

#define UP_BTN 13
#define DOWN_BTN 14
#define LEFT_BTN 11
#define RIGHT_BTN 12
#define ENTER_BTN 0
#define ESC_BTN 1
#define HOME_BTN 8
#define X_BTN 2
#define Y_BTN 3
#define LB_BTN 4
#define RB_BTN 5

extern wifi_info wifi_res[32];


extern char update_flag;
extern TSysData gSysData;
extern char key_pos;

extern GLbyte pos_addr, pos_mask, pos_gw;
extern vector<SteamLibInfo> game_info;
int js_fd=0;
struct js_event event;
int joy_exist = 0;
static int dis_game_line=0;


void JoystickResponse();
void JoystickClose()
{
	close(js_fd);
}
void JoystickInit(void)
{
	static int joyc = 0;
	js_fd = open("/dev/input/js0",O_RDONLY | O_NONBLOCK);
	if (js_fd <0)
	{
		
		close(js_fd);
		if(joyc<1)
			LOG_ERR("js0 open err!");
		joyc++;
	}
	/*js1_fd = open("/dev/input/js1",O_RDONLY | O_NONBLOCK);
	if (js1_fd <0)
	{
		printf("js1 open err!\n");
	}
	js2_fd = open("/dev/input/js2",O_RDONLY | O_NONBLOCK);
	if (js2_fd <0)
	{
		printf("js2 open err!\n");
	}
	*/
	uint8_t num_axis   = 0;
	uint8_t num_button = 0;
	
	ioctl(js_fd, JSIOCGAXES,    &num_axis);
	ioctl(js_fd, JSIOCGBUTTONS, &num_button);

	char name_c_str[1024]={0};
	ioctl(js_fd, JSIOCGNAME(sizeof(name_c_str)), name_c_str);
}

void *JoystickCreateThread(void *arg)
{
	int joy_check_time=0;
	JoystickInit();
	while(1)
	{	
		ThreadMutex();
		
		joy_check_time++;
		if (joy_check_time>=1000)
		{
			joy_check_time = 0;

			if (!joy_exist)
			{                                                                                                                                                                                                                                                                                          
				JoystickInit();		
			}
		}
		JoystickResponse();
		//JoystickResponse(js1_fd);
		//JoystickResponse(js2_fd);
		usleep(1000);
	}
	return NULL;
}

void KeyEvent(int n)
{
	int i;
	
	switch(n)
	{
		case UP_BTN:
			
			if(gSysData.web_open==1)
				break;
			if(gSysData.state == SOFTKEY)
			{
				if (key_pos>=7 && key_pos<=14)
					key_pos-=4;
				else if (key_pos==3)
					key_pos=0;
				else if (key_pos==4 || key_pos==5)
					key_pos=1;
				else if (key_pos==6)
					key_pos=2;
				
			}
			else if (gSysData.state == BASE)
			{
				if (gSysData.set_base.position ==1)
				{
					gSysData.set_base.position = 4;
					gSysData.set_base.up=1;
					gSysData.base_trans = SHIMMER_GAME;
					
				}
				else if ( gSysData.set_base.position==2)
				{
					gSysData.set_base.position = 4;
					gSysData.set_base.up=1;
					gSysData.base_trans = MGTV_GAME;
				}
				else if ( gSysData.set_base.position==3)
				{
					gSysData.set_base.position = 4;
					gSysData.set_base.up=1;
					gSysData.base_trans = STEAMOS_GAME;
				}
				else if (gSysData.set_base.position==17)
				{
					gSysData.off = 1;
				}
			}
			
			else if (gSysData.state == SETTINGS)
			{
				if (gSysData.set_setting.position > 1)
				{
					gSysData.set_setting.position--;
				}
				if (gSysData.set_setting.position==3)
				{
					gSysData.set_trans = BLUETOOTH_VOICE;
				}
				else if (gSysData.set_setting.position==2)
				{
					gSysData.set_trans = VOICE_IPSET;
				}
				else if (gSysData.set_setting.position==1)
				{
					gSysData.set_trans = IPSET_UPDATE;
				}
				
			}
			
			else if (gSysData.state == RESULOTION_SET)
			{	
				if (gSysData.resolution_pos>1)
					gSysData.resolution_pos--;
			}

			switch(gSysData.state)
			{
				case IPSET_WRIED_WRIELESS:
					{
						gSysData.net_way =0;
					}
					break;
				case IPSET_WRIED_MANUAL_AUTO:
					{
						gSysData.net_way = 2;
					}
					break;
				case IPSET_WRIELESS_MANUAL_AUTO:
					{
						gSysData.net_way = 4;
							
					}
					break;
				case IPSET_WRIED_MANUAL:
					{					
						if(gSysData.ip_set_pos > 1)
							gSysData.ip_set_pos--;
					}
					break;
				case IPSET_WRIELESS:
					
					{	if(gSysData.wifi_group==0 && gSysData.wifi_ssid >0)
						{
								gSysData.wifi_ssid--;
						}	
						if(gSysData.wifi_group==1 && gSysData.wifi_ssid >10)
						{
								gSysData.wifi_ssid--;
						}			
					
					}
					break;
				
				case IPSET_WRIELESS_KEY:
					{					
						if(gSysData.wifi_other)
						{						
							if(gSysData.wifi_con >0)
								gSysData.wifi_con--;
						}
						else
						{	
							if(gSysData.wifi_con >1)
								gSysData.wifi_con--;
						}
					}
					break;
				case  IPSET_AIISC_KEY:
					{
						if (gSysData.key_soft_pos>12 && gSysData.key_soft_pos<16)
							gSysData.key_soft_pos-=12;
						else if (gSysData.key_soft_pos==16)
							gSysData.key_soft_pos = 5;
						else if (gSysData.key_soft_pos>16 && gSysData.key_soft_pos<20)
							gSysData.key_soft_pos-=11;
						else if (gSysData.key_soft_pos>=19 && gSysData.key_soft_pos<35)
							gSysData.key_soft_pos-=10;
						else if (gSysData.key_soft_pos>=35 && gSysData.key_soft_pos<39)
							gSysData.key_soft_pos-=9;
						else if (gSysData.key_soft_pos>=39 && gSysData.key_soft_pos<43)
							gSysData.key_soft_pos-=10;
						
					}
					break;
				case GAME:
				{
					
					gSysData.rming=0;
					if(gSysData.steam_game_cp==1)
					{
						if(gSysData.game_selected==1 ||gSysData.mgame_selected==2)
							break;
						if(gSysData.cping == 1 ||gSysData.mcping == 1)
							break;
						
						else
						{
							if(gSysData.conl==0)
							{
						
								if(gSysData.sg_pos >1)
								{
									gSysData.sg_pos--;
									gSysData.sg_posup=0;
									gSysData.sg_posdown=0;
								}
								else if(gSysData.sg_pos==1)
								{
									gSysData.sg_posup=1;
								}

								if(gSysData.game_save>0)
								{
									gSysData.game_save--;
								}
								ShowNameDown(gSysData.game_save);
								//if (gSysData.game_local>2)
								//gSysData.game_local-=2;
							}
					
							else if(gSysData.conl==1)
							{
								if(gSysData.conlture==1)
								{
								
									if(gSysData.mg_pos >1)
									{
										gSysData.mg_pos--;
										gSysData.mg_posup=0;
										gSysData.mg_posdown=0;
									}
									else if(gSysData.mg_pos==1)
									{
										gSysData.mg_posup=1;
									}

									if(gSysData.game_move>0)
									{
										gSysData.game_move--;
									}
									ShowNameUp(gSysData.game_move);
						
							
								}
							}
						}
					}
					break;
				}	
				case DOWNLOAD:
				{
					if(gSysData.download_pos>0)
						gSysData.download_pos--;
					break;
				}
					
				case DIG_MARKET:
				{
					if(gSysData.sm_game_pos>2)
						gSysData.sm_game_pos-=2;
					break;
				}
					

				default:break;
				
			}
					
			break;
		case DOWN_BTN:
			if(gSysData.web_open==1)
				break;
			if(gSysData.state == SOFTKEY)
			{
				if (key_pos>=3 && key_pos<=10)
					key_pos+=4;
				else if (key_pos==0)
					key_pos=3;
				else if (key_pos==1)
					key_pos=4;
				else if (key_pos==2)
					key_pos=6;

			}
			
			else if (gSysData.state == BASE)
			{
				if (gSysData.set_base.position == 8)
				{
					gSysData.set_base.position = 1;
					gSysData.set_base.up=0;
					gSysData.base_trans = SHUTDOWN_SHIMMER;
				}
				else if (gSysData.set_base.position == 7)
				{
					gSysData.set_base.position = 1;
					gSysData.set_base.up=0;
					gSysData.base_trans = SHUTDOWN_SHIMMER;
				}
				else if (gSysData.set_base.position == 6)
				{
					gSysData.set_base.position = 1;
					gSysData.set_base.up=0;
					gSysData.base_trans = SHUTDOWN_SHIMMER;
				}
				else if (gSysData.set_base.position == 5)
				{
					gSysData.set_base.position = 1;
					gSysData.set_base.up=0;
					gSysData.base_trans = SET_SHIMMER;
				}
				else if (gSysData.set_base.position == 4)
				{
					gSysData.set_base.position = 1;
					gSysData.set_base.up=0;
					gSysData.base_trans = GAME_SHIMMER;
				}
				else if (gSysData.set_base.position==17)
				{
					gSysData.off = 2;
					
				}
				
			}
			else if (gSysData.state == SHIMMER)
			{
				gSysData.set_Shimmer.position = 2;
			}
			else if (gSysData.state == SETTINGS)
			{
				
				if (gSysData.set_setting.position < 4)
					gSysData.set_setting.position++;
				if (gSysData.set_setting.position==2)
				{
					gSysData.set_trans = UPDATE_IPSET;
				}
				else if (gSysData.set_setting.position==3)
				{
					gSysData.set_trans = IPSET_VOICE;
				}
				else if (gSysData.set_setting.position==4)
				{
					gSysData.set_trans = VOICE_BLUETOOTH;

					gSysData.resolution_pos = gSysData.resolution;


				}

				if(gSysData.set_setting.position == 2)
				{
					gSysData.ip_set_pos = 0;
				}
			}
			
			else if (gSysData.state == RESULOTION_SET)
			{	
				if (gSysData.resolution_pos<10)
					gSysData.resolution_pos++;
			}

			switch(gSysData.state)
			{
				case IPSET_WRIED_WRIELESS:
					{
						if(gSysData.wifi_device == 1)
						gSysData.net_way =1;
					}
					break;
				case IPSET_WRIED_MANUAL_AUTO:
					{
						gSysData.net_way = 3;
					}
					break;
				case IPSET_WRIELESS_MANUAL_AUTO:
					{
						
						gSysData.net_way = 5;
						
					}
					break;
				case IPSET_WRIED_MANUAL:
					{					
						if(gSysData.ip_set_pos < 5)
							gSysData.ip_set_pos++;
					}
					break;
				case IPSET_WRIELESS:
					{
						if (gSysData.wifi_scan_num<10 && gSysData.wifi_ssid<gSysData.wifi_scan_num)
						{
								gSysData.wifi_ssid++;
						}
						else if(gSysData.wifi_scan_num>=10 && gSysData.wifi_ssid<9)
						{
							gSysData.wifi_ssid++;
						}
						else if(gSysData.wifi_group==1 && gSysData.wifi_ssid <gSysData.wifi_scan_num)
						{
								gSysData.wifi_ssid++;
						}	
						
					
					}
					break;
				case IPSET_WRIELESS_SSID:
					{
						gSysData.wifi_con=1;
						
					}
					break;
				case IPSET_WRIELESS_KEY:
					{					
						if(gSysData.wifi_con <3)
							gSysData.wifi_con++;
					}
					break;
				case  IPSET_AIISC_KEY:
					{
						if (gSysData.key_soft_pos<5)
							gSysData.key_soft_pos+=12;
						else if (gSysData.key_soft_pos>4 && gSysData.key_soft_pos<12)
							gSysData.key_soft_pos+=11;
						else if (gSysData.key_soft_pos>=12 && gSysData.key_soft_pos<24)
							gSysData.key_soft_pos+=10;
						else if (gSysData.key_soft_pos==24|| gSysData.key_soft_pos==25)
							gSysData.key_soft_pos = 34;
						else if (gSysData.key_soft_pos>25 && gSysData.key_soft_pos<34)
							gSysData.key_soft_pos +=9;
					}
					break;
				case GAME:
				{
					
					gSysData.rming=0;
					if(gSysData.steam_game_cp==1)
					{
						if(gSysData.game_selected==1 ||gSysData.mgame_selected==2)
							break;
						if(gSysData.cping == 1 ||gSysData.mcping == 1)
							break;
						else
						{
							if(gSysData.conl==0)
							{
							
								if(gSysData.sg_pos < 5 )
								{
									if(gSysData.sg_pos<gSysData.game_num)
									{
									gSysData.sg_pos++;
									gSysData.sg_posdown=0;
									gSysData.sg_posup=0;
									}
									else if(gSysData.sg_pos==gSysData.game_num)							
			gSysData.sg_posdown=1;					}
								else if(gSysData.sg_pos==5)
								{
									gSysData.sg_posdown=1;
								}

								if(gSysData.game_save<gSysData.game_num-1)
								{
									gSysData.game_save++;
								}
							
								ShowNameDown(gSysData.game_save-1);
								//if (gSysData.game_local<3)
									//gSysData.game_local+=2;
							}
					
							else if(gSysData.conl==1)
							{
								if(gSysData.conlture==1)
								{
								
									if(gSysData.mg_pos < 5)
									{
										if(gSysData.mg_pos<gSysData.mgame_num)
										{
											gSysData.mg_pos++;
											gSysData.mg_posdown=0;
											gSysData.mg_posup=0;
										}
										else if(gSysData.mg_pos==gSysData.game_num)							
											gSysData.mg_posdown=1;	
		
									}
									else if(gSysData.mg_pos==5)
									{
										gSysData.mg_posdown=1;
									}

									if(gSysData.game_move<gSysData.mgame_num-1)
									{
										gSysData.game_move++;
										
									}
						
									ShowNameUp(gSysData.game_move-1);
									
								
								}
							}
						}
					}	
					break;
				}
				case DOWNLOAD:
				{
					if(gSysData.download_pos<4)
						gSysData.download_pos++;
					
				}
				break;
				case DIG_MARKET:
				{
					if(gSysData.sm_game_pos+1<7)
						gSysData.sm_game_pos+=2;
					break;
				}
				

				default:break;
				
			}
			break;
		case LEFT_BTN:
			if(gSysData.web_open==1)
				break;
			if(gSysData.state == SOFTKEY)
			{
				if (key_pos > 0)
					key_pos-=1;
				
			}
			switch(gSysData.state)
			{
				case BASE:	
					{
						if (gSysData.set_base.up == 0)
						{
							gSysData.set_base.position--;
							if (gSysData.set_base.position == 1)
							{
								gSysData.base_trans = MGTV_SHIMMER;
							}
							else if (gSysData.set_base.position == 2)
							{
								gSysData.base_trans = STEAMOS_MGTV;
							}
							else if (gSysData.set_base.position == 0)
							{
								gSysData.set_base.position = 3;
								gSysData.base_trans = SHIMMER_STEAMOS;
							}
						}
						else if(gSysData.set_base.up == 1)
						{
							if(gSysData.set_base.position!=17)							
								gSysData.set_base.position--;
							
							if(gSysData.set_base.position == 3)
							{
								gSysData.set_base.position=8;
							}
							
						}	
						
					}
					
					break;
				case SHIMMER:
					{
						if (gSysData.set_Shimmer.position > 2)
						{
					
							gSysData.set_Shimmer.position--;
						}
						if (gSysData.set_Shimmer.position == 3)
						{
							gSysData.shm_trans = STORE_WEB;
						}
						else if (gSysData.set_Shimmer.position == 2)
						{
							gSysData.shm_trans = WEB_DIG;
						}
					}
					break;
				case SETTINGS:
					{
						
					}
					break;
				case IPSET:
					if (gSysData.set_setting.position == 2)
					{
						gSysData.ip_set_pos = 0;
						gSysData.net_opt = 0;
						gSysData.net_state=0;
						gSysData.net_l=0;
						gSysData.state = SETTINGS;
					}	
					break;			
				
				case VOICESET:
					if(gSysData.voice_pos == 1)
					{
						gSysData.voice_pos = 0;
						gSysData.state = SETTINGS;
					}
					else if(gSysData.voice_pos == 2)
						gSysData.voice_pos = 1;
					break;
				
			
				case RESULOTION_SET:
					gSysData.state = SETTINGS;
					gSysData.resolution_pos = gSysData.resolution;
					
					break;

				case IPSET_WRIELESS:
					{					
						if(gSysData.wifi_group >0)
						{
							gSysData.wifi_group--;
							gSysData.wifi_ssid=0;
						}
						
							
						
					}
					break;
				case  IPSET_AIISC_KEY:
					{
						if (gSysData.key_soft_pos>1)
							gSysData.key_soft_pos--;
						
					}
					break;
				case GAME:
				{
					if(gSysData.steam_game_cp==1)
					{
						if(gSysData.cping == 1 ||gSysData.mcping == 1)
							break;

						else if(gSysData.mgame_selected==2||gSysData.mgame_selected==3)
						{
							gSysData.sure_rm=0;
							gSysData.msure_copy=0;
							gSysData.mgame_selected=1;
						}
						else
						{
							gSysData.conl=0;
							gSysData.game_selected=0;
						}
					}
					break;
				}
				case DIG_MARKET:
				{
					if(gSysData.sm_game_pos%2==0)
						gSysData.sm_game_pos-=1;
					break;
				}
				
				
					default: break;
			}
			break;
		case RIGHT_BTN:		
			if(gSysData.web_open==1)
				break;
			if(gSysData.state == SOFTKEY)
			{
				if (key_pos<14)
				key_pos+=1;
			}
			switch(gSysData.state)
			{
				case BASE:
					{	
						
						if (gSysData.set_base.up == 0)
						{
							gSysData.set_base.position++;
							if (gSysData.set_base.position == 2)
							{
								gSysData.base_trans = SHIMMER_MGTV;
							}
							else if (gSysData.set_base.position == 3)
							{
								gSysData.base_trans = MGTV_STEAMOS;
							}
							else if (gSysData.set_base.position == 4)
							{
								gSysData.set_base.position = 1;
								gSysData.base_trans = STEAMOS_SHIMMER;
							}
						}
						else if(gSysData.set_base.up == 1)
						{
							if(gSysData.set_base.position!=17)
								gSysData.set_base.position++;
							
							if(gSysData.set_base.position == 9)
							{
								gSysData.set_base.position =4;
							}
					
						}
						
					}
					break;
				case SHIMMER:
					{
						if (gSysData.set_Shimmer.position < 4)
							gSysData.set_Shimmer.position++;
						if (gSysData.set_Shimmer.position == 3)
						{
							gSysData.shm_trans = DIG_WEB;
						}
						else if (gSysData.set_Shimmer.position == 4)
						{
							gSysData.shm_trans = WEB_STORE;
						}
						
					}
					break;
				case SETTINGS:
					if (gSysData.set_setting.position == 1)
					{
						//gSysData.update_pos = 1;
						//gSysData.state = UPDATE;
					}
					else if (gSysData.set_setting.position == 2)
					{
						gSysData.ip_set_pos = 1;
						gSysData.net_opt=1;
						gSysData.state = IPSET;
					}	
					else if (gSysData.set_setting.position == 3)
					{
						gSysData.voice_pos = 1;
						gSysData.state = VOICESET;
					}
					else if (gSysData.set_setting.position == 4)
					{
						gSysData.resolution_pos = gSysData.resolution;
						gSysData.state = RESULOTION_SET;
					}			
					break;
				
				case VOICESET:
					gSysData.voice_pos = 2;
					break;

				case RESULOTION_SET:
					//gSysData.resolution_pos = gSysData.resolution;
					break;
				case IPSET_WRIELESS:
					{					
						if(gSysData.wifi_group <1 && gSysData.wifi_scan_num>=10)
							gSysData.wifi_group++;
						if(gSysData.wifi_group==1)
							gSysData.wifi_ssid=10;
						
					}
					break;
				case  IPSET_AIISC_KEY:
					{
						if (gSysData.key_soft_pos<42)
							gSysData.key_soft_pos++;
						
					}
					break;
				case GAME:
				{
					if(gSysData.steam_game_cp==1)
					{
						
						if(gSysData.cping == 1 ||gSysData.mcping == 1)
							break;
						
						else if(gSysData.mgame_selected==2||gSysData.mgame_selected==3)
						{
							gSysData.sure_rm=1;
							gSysData.game_selected=0;
						}
						else
							gSysData.conl=1;

					}
						//if (gSysData.game_local<4)
						//gSysData.game_local+=1;
					break;
				}
				case DIG_MARKET:
				{
					if(gSysData.sm_game_pos == 0)
						gSysData.sm_game_pos=1;
					else if(gSysData.sm_game_pos%2==1 && gSysData.sm_game_pos<7)
						gSysData.sm_game_pos+=1;
					break;
				}
				
			}
			break;
		case ENTER_BTN://enter
			
			if(gSysData.cping==1 ||gSysData.mcping==1 ||gSysData.mounting==1)
				break;
			
			if(gSysData.web_open==1)
				break;
			switch(gSysData.state)
			{
					
				case BASE:
					ResetCoord();
					if (gSysData.set_base.position==1)
					{
						gSysData.state = SHIMMER;
						if(gSysData.shm_tex_load==0)
							gSysData.shm_tex_load++;
					}
					else if (gSysData.set_base.position==3)
					{
						
						//if(gSysData.download==0)
						
							exit(0);
					}
					else if (gSysData.set_base.position==2)
					{
							//gSysData.state = TV;
							//gSysData.mg_tv = 1;
							JoyThreadPause();
							JoystickClose();
							SndStop();
							LOG_ERR("state: TV");
							SystemEx("/home/steam/.shimmer/mango.sh");
							SndResume();
							JoystickInit();
							JoyThreadResume();
											
					}
					else if (gSysData.set_base.position==5)
					{
						gSysData.state = SETTINGS;
						if(gSysData.set_tex_load==0)
							gSysData.set_tex_load++;
						LOG_ERR("state: SETTINGS");
						//SetIfUp(gSysData.eth_name);
						//SetIfUp(gSysData.wlan_name);
						if (GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.eth_name))
						{
							
							GetDNS(gSysData.DNS);
							
							GetGateway(gSysData.gateway);
							if (gSysData.DNS[0] == 0)
								memcpy(gSysData.DNS,gSysData.gateway,sizeof(gSysData.gateway));

							gSysData.getip_state = 1;
						}
						else if(GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.wlan_name))
						{
							GetDNS(gSysData.DNS);
							
							GetGateway(gSysData.gateway);
							if (gSysData.DNS[0] == 0)
								memcpy(gSysData.DNS,gSysData.gateway,sizeof(gSysData.gateway));

							gSysData.getip_state = 1;
						}
						else
						{
							gSysData.getip_state = 0;
						}						
					}
					else if (gSysData.set_base.position==4)
					{
						LOG_ERR("state: GAME");
						if(gSysData.game_tex_load==0)
							gSysData.game_tex_load++;
						Clearlist();
						
						if(dis_game_line==1)
						{
							gSysData.conlture=1;
							gSysData.conl=1;
						}
						else
						{
							gSysData.conlture=0;
							gSysData.conl=0;
						}
						gSysData.state = GAME;
						gSysData.game_local=1;
						gSysData.game_save=0;
						gSysData.game_move=0;
						gSysData.mounted=1;
						gSysData.mounting=0;
						gSysData.sg_pos=1;
						gSysData.mg_pos=1;
						gSysData.cping=0;
						gSysData.getsizeing=0;
						gSysData.mcping=0;
						gSysData.getmsizeing=0;
						gSysData.game_selected=0;
						gSysData.sure_copy=0;
						gSysData.mgame_selected=1;
						gSysData.msure_copy=0;
						
						
					}
					else if(gSysData.set_base.position==6)
					{
						gSysData.state = DOWNLOAD;
						LOG_ERR("state: DOWNLOAD");
					}
					else if (gSysData.set_base.position==7)
					{
						LOG_ERR("music stop/resume");
						if (gSysData.music_on)
							SndStop();
						else
							SndResume();
						
						gSysData.music_on = !gSysData.music_on;
						
					}	
					else if (gSysData.set_base.position==8)// OFF or REBOOT
					{
						gSysData.set_base.position=17;
						gSysData.off=1;

					}
					else if (gSysData.set_base.position==17)
					{
						if (gSysData.off==1)
						{	
							setuid(1001);
							//printf("uid = %d\n",getuid());	
							//printf("euid = %d\n",geteuid());
							SystemEx("systemctl reboot");
						}	
						else if (gSysData.off==2)
						{
							setuid(1001);
							//printf("uid = %d\n",getuid());	
							//printf("euid = %d\n",geteuid());
							SystemEx("systemctl poweroff");					
						}					
					}
					break;
					
				case SHIMMER:
					
					if(gSysData.set_Shimmer.position == 2)
					{
						thread_create(GetGameInfo,NULL);
						gSysData.state = DIG_MARKET;
					}
					else if (gSysData.set_Shimmer.position == 3)// iceweasel -new-table www.baidu.com
					{	
						gSysData.web_open=1;
						SystemEx("/home/steam/.shimmer/open_icewseal.sh www.shimmerfc.com");
					}
					else if (gSysData.set_Shimmer.position == 4)// iceweasel -new-window www.baidu.com
					{
						gSysData.web_open=1;
						//SystemEx("iceweasel -new-window www.sammei.cn &");
						SystemEx("/home/steam/.shimmer/open_icewseal.sh www.sammei.cn");
					}
					break;
				
				case STEAMOS:
					
					//if(gSysData.ter == 0)
						//exit (0);
					break;
				case SETTINGS:
					if (gSysData.set_setting.position == 1 && gSysData.update_start == 1)
					{		
						thread_create(UpdateUSBThread,NULL);
					}
					break;
				case DOWNLOAD:
				{
					//if(gSysData.download_pos==4 && gSysData.download==0)
					//	thread_create(DownloadCN,NULL);
						gSysData.down_load_stop = false;
						thread_create(GetGameTgz,NULL);
						break;
					
				}
					break;
				case IPSET:
					{
						LOG_ERR("state: IPSET");
						gSysData.state = IPSET_WRIED_WRIELESS;
						gSysData.net_way =0;
						
					}
					break;
				case IPSET_WRIED_WRIELESS:
					{
						LOG_ERR("state: IPSET_WRIED_WRIELESS");
						if (gSysData.net_way == 0)
						{
							gSysData.state = IPSET_WRIED_MANUAL_AUTO;
							gSysData.net_way = 2;
						}
						else if(gSysData.net_way==1)
						{
							wifi_on();
							gSysData.state = IPSET_WRIELESS;
							GetWifiInfo();
						}
						gSysData.wifi_group=0;
						gSysData.wifi_ssid=0;
						
					}
					break;
				case IPSET_WRIELESS:
					{
						memset(gSysData.wifi_ssid_tmp,0,sizeof(gSysData.wifi_ssid_tmp));
						memset(gSysData.wifi_key_tmp,0,sizeof(gSysData.wifi_key_tmp));
						gSysData.wifi_other=0;
						gSysData.wifi_con=1;
						if(gSysData.wifi_ssid == gSysData.wifi_scan_num)
						{
							gSysData.wifi_con=0;
							gSysData.wifi_other=1;
						}						
						gSysData.state = IPSET_WRIELESS_KEY;
						memcpy(gSysData.wifi_ssid_tmp,wifi_res[gSysData.wifi_ssid].ssid_str,64);
						ReadConfStr("wifi_key",gSysData.wifi_key_str);
						memcpy(gSysData.wifi_key_tmp,gSysData.wifi_key_str,64);
						
						
						SoftKeyInit();
						

					}		
					break;
				
				
				case IPSET_WRIELESS_KEY:
					{
						if(gSysData.wifi_con == 0)
						{
							gSysData.key_soft_pos = 1;
							gSysData.wifi_ssid_write=1;
							gSysData.wifi_key_write=0;
							gSysData.state = IPSET_AIISC_KEY;
							gSysData.key_soft_pos=1;
							gSysData.wifi_show_keypad = 1;
						}
						else if(gSysData.wifi_con == 1)
						{
							gSysData.key_soft_pos = 1;
							gSysData.wifi_ssid_write=0;
							gSysData.wifi_key_write=1;
							gSysData.state = IPSET_AIISC_KEY;
							gSysData.key_soft_pos=1;
							gSysData.wifi_show_keypad = 1;
							
						}
						else if(gSysData.wifi_con == 2)
						{
							gSysData.wifi_hide_key=!gSysData.wifi_hide_key;
						}
						else if (gSysData.wifi_con == 3)
						{
							memcpy(gSysData.wifi_ssid_str,gSysData.wifi_ssid_tmp,sizeof(gSysData.wifi_ssid_str));
							
							
							WriteConfStr("wifi_ssid",gSysData.wifi_ssid_tmp);
							WriteConfStr("wifi_key",gSysData.wifi_key_tmp);
							gSysData.state = WIFI_CONNECTING;
							char netstr[64]= {0};						
							sprintf(netstr,"nmcli device disconnect %s",gSysData.eth_name);
							SystemEx(netstr);
							WifiConnectThread();			
						}
					}		
					break;
				case  IPSET_AIISC_KEY:
					{
						gSysData.assic_key_down=1;
					}
					break;
				
				case IPSET_WRIED_MANUAL_AUTO:
					{
						if (gSysData.net_way == 2)
						{
							WiredConnectThread();
							gSysData.state = IPSET_WRIED_AUTO;
							
						}
						else if (gSysData.net_way == 3)
						{
							gSysData.state = IPSET_WRIED_MANUAL;
							gSysData.ip_set_pos=1;
							gSysData.ip_check=0;
							KeyInit();
						}
						
					}
					break;
				case IPSET_WRIELESS_MANUAL_AUTO:
					{
						if (gSysData.net_way == 4)
						{
							gSysData.state = IPSET_WRIELESS_AUTO;
							
						}						
						else if (gSysData.net_way == 5)
						{
							gSysData.state = IPSET_WRIELESS_MANUAL;
							
							
						}
						LOG_ERR("eth auto");
					}
					break;
				case IPSET_WRIED_AUTO:
					{
							
						//gSysData.state = IPSET_WRIED;
					}
					break;
				case IPSET_WRIED_MANUAL:
					{
						if (gSysData.ip_set_pos != 5)
							gSysData.state = SOFTKEY;
						else
						{
							gSysData.ip_check=0;
							char c1[8],c2[8],c3[8],c4[8];
							ParseIp(gSysData.ip_set,c1,c2,c3,c4);
							LOG_INFO("ip_set = %s %s.%s.%s.%s",gSysData.ip_set,c1,c2,c3,c4);
							//gSysData.note_time=30;
							if(CheckIpStr(c1,c2,c3,c4,1)==0)
							{	
								gSysData.ip_check=1;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
								
								break;
							}
							ParseIp(gSysData.mask_set,c1,c2,c3,c4);
							if(CheckIpStr(c1,c2,c3,c4,0)==0)
							{	
								gSysData.ip_check=1;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
			
								break;
							}	
							ParseIp(gSysData.gateway_set,c1,c2,c3,c4);
							if(CheckIpStr(c1,c2,c3,c4,0)==0)
							{	
								gSysData.ip_check=1;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
								
								break;
							}
							ParseIp(gSysData.DNS_set,c1,c2,c3,c4);
							if(CheckIpStr(c1,c2,c3,c4,0)==0)
							{	
								gSysData.ip_check=1;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
								
								break;
							}
							
							
 							if(SetIp(gSysData.eth_name, gSysData.ip_set, 					    gSysData.mask_set,gSysData.gateway_set,gSysData.DNS_set)<0)
							{
								gSysData.ip_check=1;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
								
							}	
							else
							{
								gSysData.ip_check = 2;
								LOG_ERR("ip_check=%d",gSysData.ip_check);
							}	
						}
					}
					break;
				case IPSET_WRIELESS_AUTO:
					{
							
						//gSysData.state = IPSET_WRIED;
					}
					break;
				case IPSET_WRIELESS_MANUAL:
					{
							
						gSysData.state = SOFTKEY;
					}
					break;
				
				case SOFTKEY:
					gSysData.key_down=1;
					break;
				case VOICESET:
					
					if(gSysData.voice_pos == 1 && gSysData.snd_vol > 0)
					{
						
						gSysData.snd_vol-=5;
					}
					else if(gSysData.voice_pos == 2 && gSysData.snd_vol < 100)
					{
						gSysData.snd_vol+=5;
					}
					SndSet(gSysData.snd_vol);
					WriteConfInt("sys_snd_vol",gSysData.snd_vol);
					
					break;
				case RESULOTION_SET:
					{
						LOG_ERR("set resultion");
						SetXrandr(gSysData.resolution_pos-1, &gSysData.width, &gSysData.height);
					}
					break;
				case GAME:
				{	
					//printf("run game\n");
					//SystemEx("/home/steam/.shimmer/exgame.sh");
					if(gSysData.steam_game_cp==1)
					{
						
						if(gSysData.conl==0)
						{
							if(gSysData.conlture==0)
							{
								break;
							}
							if(gSysData.game_selected<2)
							{
								gSysData.game_selected+=1;
								
							}
							if(gSysData.game_selected==2)
								gSysData.sure_copy++;
							else 
								gSysData.sure_copy=0;
								
							if(gSysData.sure_copy==1)
							{			
								gSysData.game_size=0;	
								GetSgameSize();				
								thread_create(CpLocal2Move,NULL);
								
							}
						}
						else if(gSysData.conl==1)
						{
							if(gSysData.mounted==1)
							{
								LOG_INFO("Mount Disk");
								thread_create(MountDisk,NULL);
							}
							if(gSysData.conlture==1)
							{	dis_game_line=1;
							
								if(gSysData.mgame_selected<3)
									gSysData.mgame_selected+=1;
								
								if(gSysData.mgame_selected==3)
								{
									gSysData.msure_copy++;		
								}								
								else
									gSysData.msure_copy=0;
								
								if(gSysData.msure_copy==1 && gSysData.sure_rm==0)
								{	
									
									
									gSysData.Sgame_size=0;	
									GetgameSize();	
									thread_create(CpMove2Local,NULL);
									
								}
								if(gSysData.sure_rm==1)
								{
									thread_create(DelMoveGame,NULL);
										
									
									
									
								}
							}
						}
					}						
					/*SndStop();
					if (gSysData.game_local==1)
					{
						//SystemEx("/home/steam/.shimmer/rungame.sh TheLastTinker TheLastTinker.x86");
					}
					else if(gSysData.game_local==2)
					{
						//SystemEx("/home/steam/.shimmer/rungame.sh Guacamelee game-bin");
					}
					else if(gSysData.game_local==3)
					{
						//SystemEx("/home/steam/.shimmer/rungame.sh JoeDanger2TheMovie/BIN Joeanger2TheMovie");
					}
					else if(gSysData.game_local==4)
					{
						SystemEx("/home/steam/.shimmer/rungame.sh Teslagrad_linux64_fullversion_1_2_2 Teslagrad");
					}
					*/
					break;
					case DIG_MARKET:
					{
						if(gSysData.sm_game_pos>0)
						{
							gSysData.state = DOWNLOAD;
							thread_create(GetGameTgz,NULL);
							//gSysData.sm_game_pos=0;
						}
						break;
					}
					
				}
				default:
					break;

			}
			break;
		case ESC_BTN://esc
			
			if(gSysData.cping==1 ||gSysData.mcping==1 )
				break;
			
			if (gSysData.set_base.position==17)
					gSysData.set_base.position=8;
			
			switch(gSysData.state)
			{
				case SHIMMER:
				{	
					if (gSysData.web_open==1)
					{
						SystemEx("killall iceweasel");
						gSysData.web_open=0;
						//gSysData.state = BASE;
					}
					else
					{
						gSysData.set_tv.esc = 0;
						gSysData.set_setting.esc = 0;
						gSysData.set_game.esc = 0;
						gSysData.set_download.esc = 0;
						gSysData.set_Shimmer.esc = 1;
						gSysData.state = BASE;
					}
					
				}
				break;
				case STEAMOS:
				{
					;
				}
				break;
				case TV:
				{	
					gSysData.set_setting.esc = 0;
					gSysData.set_game.esc = 0;
					gSysData.set_Shimmer.esc = 0;
					gSysData.set_download.esc = 0;
					gSysData.set_tv.esc = 1;
					
					gSysData.state = BASE;
				}
				break;
				case SETTINGS:
				{
					gSysData.set_tv.esc = 0;
					gSysData.set_setting.esc = 1;
					gSysData.set_game.esc = 0;
					gSysData.set_Shimmer.esc = 0;
					gSysData.set_download.esc = 0;
					gSysData.state = BASE;
				}
				break;
				case DOWNLOAD:
				{
					gSysData.set_tv.esc = 0;
					gSysData.set_setting.esc = 0;
					gSysData.set_game.esc = 0;
					gSysData.set_Shimmer.esc = 0;
					gSysData.set_download.esc = 1;
					gSysData.state = BASE;
				}
				break;
				case  GAME:
				{
					
					gSysData.mounted=0;
					if(gSysData.game_selected==1)
					{
						gSysData.state = GAME;
						gSysData.game_selected=0;
			
					}
					else if(gSysData.mgame_selected==2)
					{
						gSysData.state = GAME;
						gSysData.mgame_selected=1;
						gSysData.sure_rm=0;
			
					}
					else
					{
						gSysData.set_tv.esc = 0;
						gSysData.set_setting.esc = 0;
						gSysData.set_download.esc = 0;
						gSysData.set_game.esc = 1;
						gSysData.set_Shimmer.esc = 0;
						gSysData.state = BASE;
					}
				}
				break;
				
				case IPSET:
				{
					gSysData.state = SETTINGS;
					gSysData.ip_set_pos = 0;
					gSysData.net_state=0;
					gSysData.net_opt = 0;
					gSysData.net_l=0;
					gSysData.key_state=1;

					/*
					for(i=0; i<20; i++)
					{
						gSysData.ipaddr[i] = 0;
						gSysData.mask[i] = 0;
						gSysData.gateway[i] = 0;
						gSysData.macaddr[i] = 0;
					}

					pos_addr = 0;
					pos_mask = 0;
					pos_gw = 0;

					
					if(GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.eth_name))
					{
						GetGateway(gSysData.gateway);
						GetDNS(gSysData.DNS);
					}
					else if(GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.wlan_name))
					{
						GetGateway(gSysData.gateway);
						GetDNS(gSysData.DNS);
						
					}
					*/
				
					}
					break;
				case VOICESET:
				{
					gSysData.state = SETTINGS;
					gSysData.voice_pos=0;
				}
				break;
					case RESULOTION_SET:
				{
					gSysData.state = SETTINGS;
					gSysData.resolution_pos = gSysData.resolution;
					//gSysData.resolution_pos = 0;
				}
				break;
				case SOFTKEY:
				{
					gSysData.key_state=1;
					gSysData.state = IPSET_WRIED_MANUAL;

				}
				break;
					case DIG_MARKET:
				{
					gSysData.state = SHIMMER;
				}
				break;
			


				case  IPSET_AIISC_KEY:
				{
					
					gSysData.state = IPSET_WRIELESS_KEY;
					gSysData.wifi_show_keypad = 0;
					break;
				}
				
				
				case  IPSET_WRIELESS_KEY:
				{					
					
					gSysData.state = IPSET_WRIELESS;
					break;
				}
					
				case  IPSET_WRIELESS:
				{
					gSysData.state = IPSET_WRIED_WRIELESS;
					break;
				}
					
				case  IPSET_WRIED_WRIELESS:
				{
					gSysData.state = IPSET;
					break;
				}
			
				
				
				case IPSET_WRIED_AUTO:
				case IPSET_WRIED_MANUAL:
					{
							
						gSysData.state = IPSET; //IPSET_WRIED_MANUAL_AUTO;
						gSysData.net_way=0;
						if(GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.eth_name))
						{
							GetGateway(gSysData.gateway);
							GetDNS(gSysData.DNS);
						}
					}
					break;
				case IPSET_WRIED_MANUAL_AUTO:
					{
						gSysData.net_way=0;
						gSysData.state =IPSET_WRIED_WRIELESS;
						break;
					}
					
				case  WIFI_CONNECTING:
					if(GetIpinfo(gSysData.ipaddr, gSysData.mask, gSysData.macaddr,gSysData.wlan_name))
					{
						GetDNS(gSysData.DNS);
						GetGateway(gSysData.gateway);
						if (gSysData.DNS[0] == 0)
							memcpy(gSysData.DNS,gSysData.gateway,sizeof(gSysData.gateway));
						gSysData.getip_state = 1;
					}
					
					if(gSysData.getip_state == 1)		
						gSysData.state = IPSET;
					else
						gSysData.state = IPSET_WRIELESS;
					gSysData.wifi_connecting=0;
					break;
			
					
				default:break;
			}

		case LB_BTN:
			{
				if(gSysData.state == SOFTKEY)
				
					gSysData.key_down=2;
				if(gSysData.state == IPSET_AIISC_KEY)
					gSysData.assic_key_down = 2;	
				break;
			}
		case X_BTN:
		{
			
			switch(gSysData.state)
			{
				case SOFTKEY:
				{
					gSysData.key_down=3;
					break;
				}
				case DOWNLOAD:
				{
					gSysData.down_load_stop = true;
					break;
				}
					
			}
			
			//if(gSysData.state == GAME)
			//SystemEx("/home/steam/.shimmer/cp_steamgame Trine 2");
			break;
		}
		case HOME_BTN:
		{
			gSysData.state=BASE;
			break;
		
		}
		case RB_BTN:
		{
			gSysData.assic_key_down = 3;
			break;
		}
			
			break;
		default:
			break;
	}
	
}
static int axis_long[4];	
//static int axis_stat[4];
//static char axis_flag[4];
static int axis_time;

void JoystickResponse()
{
	static int time_last=0;
	static int btn_last=0;
	
	ssize_t len = read(js_fd, &event, sizeof(event));
	if (len == sizeof(event))
	{    
		event.type &= ~JS_EVENT_INIT;
		
		
		
		if (event.type & JS_EVENT_AXIS)      //axis   轴
		{	
			if(time_last>0)
				time_last--;
			
			if (event.number == 0)
			{	
				if ((int)event.value == 32767 && time_last ==0)
				{
					
					KeyEvent(RIGHT_BTN);				
					axis_time = event.time;
					axis_long[3]=1;
					SndMove();	
					time_last=3;
				}
				else if ((int)event.value == -32767 && time_last ==0)
				{
					KeyEvent(LEFT_BTN);
					axis_time = event.time;
					axis_long[2]=1;
					SndMove();	
					time_last=3;
				}	
			}
			else if (event.number == 1)
			{
				if (((int)event.value == 32767) && time_last ==0)//down
				{
					KeyEvent(DOWN_BTN);
					axis_time = event.time;
					axis_long[1]=1;
					SndMove();	
					time_last=3;

					
				}
				else if (((int)event.value == -32767)&& time_last ==0)//up
				{
					KeyEvent(UP_BTN);
					axis_time = event.time;
					axis_long[0]=1;
					SndMove();	
					time_last=3;
				}
								
			}
			else if (event.number == 3)
			{

			}
			else if (event.number == 4)
			{
			}

			else if (event.number == 5)
			{

			}
			else if (event.number == 6)
			{
				if(event.value == 32767)
				{
					KeyEvent(RIGHT_BTN);
					SndMove();	
				}
				else if(event.value == -32767)
				{
					KeyEvent(LEFT_BTN);
					SndMove();	
				}
			}
			else if (event.number == 7)
			{
				
				if(event.value == 32767)
				{
					KeyEvent(DOWN_BTN);
					SndMove();	
				}
				else if(event.value == -32767)
				{
					KeyEvent(UP_BTN);
					SndMove();	
				}
			}
			
	
			
			/*if (event.number == 3)
			{
				if (((int)event.value == 32767))//down
				{printf("num3 = %d\n",event.value);
				}			
			}
			if (event.number == 4)
			{if (((int)event.value == 32767))//down
				{printf("num4 = %d\n",event.value);
				}	}
			else if (axis_long[3])
			{
				axis_stat[3]++;
				
				if (axis_stat[3]>50)
				{
					axis_flag[3]=1;
					axis_stat[3]=0;	
				}
				if(event.time-axis_time>100 && axis_flag[3])
				{
					axis_time = event.time;
					KeyEvent(RIGHT_BTN);		
				}
			}
			else if (axis_long[2])
			{
				axis_stat[2]++;
				if (axis_stat[2]>50)
				{
					axis_flag[2]=1;
					axis_stat[2]=0;	
				}
				if(event.time-axis_time>100 && axis_flag[2])
				{
					axis_time = event.time;
					KeyEvent(LEFT_BTN);		
				}
			}
			else if (axis_long[1])
			{
				axis_stat[1]++;
				if (axis_stat[1]>50)
				{
					axis_flag[1]=1;
					axis_stat[1]=0;	
				}
				if(event.time-axis_time>100 && axis_flag[1])
				{
					axis_time = event.time;
					KeyEvent(DOWN_BTN);		
				}
			}
			else if (axis_long[0])
			{
				axis_stat[0]++;
				if (axis_stat[0]>50)
				{
					axis_flag[0]=1;
					axis_stat[0]=0;	
				}
				if(event.time-axis_time>100 && axis_flag[0])
				{
					axis_time = event.time;
					KeyEvent(UP_BTN);		
				}
			}
			*/
							
		}
		else if ((event.type & JS_EVENT_BUTTON))   //an  xia  song  kai  dou  hui  jin  ru
		{
			if(btn_last>0)
				btn_last--;
			if (event.value && btn_last==0) 
			{
				KeyEvent(event.number);
				SndMove();		
				btn_last = 2;
			}
		}

	}
 
}


void KeyBoard ( unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:		//Esc键
			KeyEvent(ESC_BTN);
			SndMove();
			break;
		case 13:		//Enter键
			KeyEvent(ENTER_BTN);
			SndMove();
			break;
		case 'r':
			break;
		default:
			break;
	}
	
	
} 

void processSpecialKeys(int key, int x, int y) 
{
    switch(key) 
	{
		case  GLUT_KEY_LEFT:
			KeyEvent(LEFT_BTN);
			SndMove();
			break;                                      
		case  GLUT_KEY_RIGHT:    
			KeyEvent(RIGHT_BTN);
			SndMove();
			break;   
		case  GLUT_KEY_UP  :
			KeyEvent(UP_BTN);
			SndMove();
			break;              
		case  GLUT_KEY_DOWN:
			KeyEvent(DOWN_BTN);
			SndMove();
			break;
	
	}
	
 }


