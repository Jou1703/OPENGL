#include "includes.h"

#include "init_ui.h"
#include "settings.h"
#include "particle.h"
#include "net.h"
#include "draw_utils.h"
#include "fun.h"
#include "base64.h"
#include "option.h"
#include "sound.h"
#include "wifi.h"
#include "softkey.h"

extern TextureImage bg_tex;
extern float xcoord, ycoord, width, heigth;
extern const char* str_xrandr[10];

extern TSysData gSysData;
GLubyte key_flash;

GLubyte voice_value[11][4] = { {"0"}, {"10"}, {"20"}, {"30"}, {"40"},{"50"}, {"60"}, {"70"}, {"80"}, {"90"}, {"100"}};
GLubyte asc[15] = {'.',' ',' ','0','1',' ',' ','2','3','4','5','6','7','8','9'};
GLubyte key_pos = 0;

GLuint key_select[15], key_bak;

TextureImage sys_update_on,ipset_on,voice_set_on,bluetooth_set_on;
TextureImage inc, dec, inc_voice, dec_voice;
TextureImage program_update,voice_set,IP_set,  bluetooth_set;
TextureImage set,ip_rect,set_2;
TextureImage resolution_set,resolution_set_on;
TextureImage snd_line_ye,snd_line_gr,white_cir,yellow_cir;

GLbyte pos_addr;
GLbyte pos_mask;
GLbyte pos_gw;
GLbyte pos_dns;


void SettingsPicLoad(void)
{
	LoadBmpTex(&program_update ,get_path("sys_info.bmp"));
	LoadBmpTex(&voice_set ,get_path("voice_set.bmp"));
	LoadBmpTex(&IP_set ,get_path("ip_set.bmp"));
	LoadBmpTex(&bluetooth_set ,get_path("bluetooth_set.bmp"));

	LoadBmpTex(&inc ,get_path("snd_up.bmp"));
	LoadBmpTex(&dec ,get_path("snd_down.bmp"));
	LoadBmpTex(&inc_voice ,get_path("snd_up_on.bmp"));
	LoadBmpTex(&dec_voice ,get_path("snd_down_on.bmp"));
	
	LoadTgaTex(&snd_line_ye,get_path("snd_line_ye.tga"));
	LoadTgaTex(&snd_line_gr,get_path("snd_line_gr.tga"));
	LoadTgaTex(&white_cir,get_path("white_cir.tga"));
	LoadTgaTex(&yellow_cir,get_path("yellow_cir.tga"));

	key_select[0] = load_texture(get_path("key_10.bmp"));	//  *
	key_select[1] = load_texture(get_path("key_13.bmp"));
	key_select[2] = load_texture(get_path("key_14.bmp"));
	key_select[3] = load_texture(get_path("key_0.bmp"));
	key_select[4] = load_texture(get_path("key_1.bmp"));
	key_select[5] = load_texture(get_path("key_11.bmp"));
	key_select[6] = load_texture(get_path("key_12.bmp"));
	key_select[7] = load_texture(get_path("key_2.bmp"));
	key_select[8] = load_texture(get_path("key_3.bmp"));
	key_select[9] = load_texture(get_path("key_4.bmp"));
	key_select[10] = load_texture(get_path("key_5.bmp"));	//  .
	key_select[11] = load_texture(get_path("key_6.bmp"));
	key_select[12] = load_texture(get_path("key_7.bmp"));	//  *
	key_select[13] = load_texture(get_path("key_8.bmp"));
	key_select[14] = load_texture(get_path("key_9.bmp"));
	key_bak = load_texture(get_path("key_bak.bmp"));
	
	LoadBmpTex(&set_2 ,get_path("set_2.bmp"));
	LoadTgaTex(&set,get_path("set.tga"));	
	LoadBmpTex(&ip_rect ,get_path("IP.bmp"));

	LoadBmpTex(&sys_update_on ,get_path("sys_update_on.bmp"));
	LoadBmpTex(&ipset_on ,get_path("ipset_on.bmp"));
	LoadBmpTex(&voice_set_on ,get_path("voice_set_on.bmp"));
	LoadBmpTex(&bluetooth_set_on ,get_path("bluetooth_set_on.bmp"));
	
	LoadBmpTex(&resolution_set ,get_path("resolution_set.bmp"));
	LoadBmpTex(&resolution_set_on ,get_path("resolution_set_on.bmp"));

	WifiImgLoad();
	LoadKeyImg();
}

void VoiceSet(void)
{
	float set_x = -150;
	float set_y = 280;
	char snd_str[8]={0};

	ShowPic(set_x, set_y-250, 0.0f, 60.0f, 60.0f, dec.texID);
	ShowPic(set_x+450, set_y-250, 0.0f, 60.0f, 60.0f, inc.texID);
	if (gSysData.state == VOICESET)
	{
		if(gSysData.voice_pos == 1)
		{
			ShowPic(set_x, set_y-250, 0.0f, 60.0f, 60.0f, dec_voice.texID);
			DrawRectangle(set_x, set_y-250, 60.0f, 60.0f);
		}
		else if(gSysData.voice_pos == 2)
		{
			ShowPic(set_x+450, set_y-250, 0.0f, 60.0f, 60.0f, inc_voice.texID);
			DrawRectangle(set_x+450, set_y-250, 60.0f, 60.0f);
		}
	}
	

	glEnable(GL_BLEND);
	
	ShowPic(set_x, set_y-140, 0.0f, snd_line_gr.width, snd_line_gr.height, snd_line_gr.texID);
	ShowPic(set_x-5, set_y-140, 0.0f, 15, 15, yellow_cir.texID);
	DrawYeRect(set_x, set_y-140,(snd_line_ye.width/100)*gSysData.snd_vol, 15);
	ShowPic(set_x-8+(snd_line_ye.width/100)*gSysData.snd_vol, set_y-143, 0.0f, white_cir.width, white_cir.height, white_cir.texID);
	
	
	sprintf(snd_str,"%d\%",gSysData.snd_vol);
	glColor3ub(231,111,1);
	FontDraw(set_x, set_y-50,"系统音量",30);
	FontDraw(set_x+250, set_y-240,snd_str,25);
	glDisable(GL_BLEND);
}

void ResolutionSet()
{

	float base_pos_x=-150.0f;
	float base_pos_y=280.0f;

	for(int i=0;i<10;++i)
	{
		DrawYeRect(base_pos_x, base_pos_y-103-50*(i+1),400, 40);
	}
	if (gSysData.state == RESULOTION_SET)
	{
		DrawWhRect(base_pos_x, base_pos_y-103-50*(gSysData.resolution_pos),400, 40);
		DrawRectangle(base_pos_x, base_pos_y-103-50*(gSysData.resolution_pos), 400, 40);
	}
	glEnable(GL_BLEND);


	ShowPic(base_pos_x+45,  base_pos_y-95-50*gSysData.resolution, 0.0f, white_cir.width,white_cir.height, white_cir.texID);
	
	glColor3ub(231,111,1);
	FontDraw(base_pos_x, base_pos_y-50,"分辨率选择",30);
	
	
	for(int i=0; i<10; ++i)
	{
		
		if(gSysData.state != RESULOTION_SET)
		{	
			
			glColor3ub(255,255,255);
			FontDraw(base_pos_x+130,base_pos_y-143-i*50,str_xrandr[i],25);
		}
		else
		{
			if(i==gSysData.resolution_pos-1)
			{
				glColor3ub(231,111,1);
				
				FontDraw(base_pos_x+130,base_pos_y-143-i*50,str_xrandr[i],25);
				if(gSysData.resolution_pos == gSysData.resolution)
					ShowPic(base_pos_x+45,  base_pos_y-95-50*gSysData.resolution, 0.0f, white_cir.width,white_cir.height, yellow_cir.texID);
			}
			else
			{
				glColor3ub(255,255,255);
				
				FontDraw(base_pos_x+130,base_pos_y-143-i*50,str_xrandr[i],25);
			}

		}
	}
	//
	glDisable(GL_BLEND);


}

void DrawRecEx(float x1,float y1, float x2,float y2)
{
	glDisable(GL_TEXTURE_2D); 
	glRectf(x1, y1, x2, y2);
	glEnable(GL_TEXTURE_2D);
}

void SettingsSystemRender(void)
{	
	ShowPic(xcoord, ycoord, 0.0f, width,heigth, bg_tex.texID);	
	if (width<1920)
		return;
	TipNote();
	TimeDisplay();
	
	
	ShowPic(PROUPDATE_X-gSysData.trans_update/2.0f, PROUPDATE_Y-gSysData.trans_update/2.0f, 0, PROUPDATE_WIDTH+gSysData.trans_update, PROUPDATE_HEIGTH+gSysData.trans_update, program_update.texID);
	ShowPic(IPSET_X-gSysData.trans_ipset/2.0f, IPSET_Y-gSysData.trans_ipset/2.0f, 0, IPSET_WIDTH+gSysData.trans_ipset, IPSET_HEIGTH+gSysData.trans_ipset, IP_set.texID);
	ShowPic(VOICESET_X-gSysData.trans_voice/2.0f, VOICESET_Y-gSysData.trans_voice/2.0f, 0, VOICESET_WIDTH+gSysData.trans_voice, VOICESET_HEIGTH+gSysData.trans_voice, voice_set.texID);
	ShowPic(BLUETOOTHSET_X-gSysData.trans_bluetooth/2.0f, BLUETOOTHSET_Y-gSysData.trans_bluetooth/2.0f, 0, BLUETOOTHSET_WIDTH+gSysData.trans_bluetooth, BLUETOOTHSET_HEIGTH+gSysData.trans_bluetooth, resolution_set.texID);


	glEnable(GL_BLEND);
	ShowPic(-860, 350.0f, 0.0f, set.width, set.height, set.texID);
	FontDraw(-830, 280.0f,"设 置",40);
	glDisable(GL_BLEND);
	switch(gSysData.set_setting.position)
	{
		case 1:
			ShowPic(PROUPDATE_X-gSysData.trans_update/2.0f, PROUPDATE_Y-gSysData.trans_update/2.0f, 0, PROUPDATE_WIDTH+gSysData.trans_update, PROUPDATE_HEIGTH+gSysData.trans_update, sys_update_on.texID);
			SysInfoRender();
			if(gSysData.update_pos==0)
				DrawRectangle(PROUPDATE_X-gSysData.trans_update/2.0f, PROUPDATE_Y-gSysData.trans_update/2.0f,  PROUPDATE_WIDTH+gSysData.trans_update, PROUPDATE_HEIGTH+gSysData.trans_update);
			break;
		case 2:
			ShowPic(IPSET_X-gSysData.trans_ipset/2.0f, IPSET_Y-gSysData.trans_ipset/2.0f, 0, IPSET_WIDTH+gSysData.trans_ipset, IPSET_HEIGTH+gSysData.trans_ipset, ipset_on.texID);
			IpSetRender();
			
			if(gSysData.net_opt == 0)
				DrawRectangle(IPSET_X-gSysData.trans_ipset/2.0f, IPSET_Y-gSysData.trans_ipset/2.0f,  IPSET_WIDTH+gSysData.trans_ipset, IPSET_HEIGTH+gSysData.trans_ipset);
			break;
		case 3:
			ShowPic(VOICESET_X-gSysData.trans_voice/2.0f, VOICESET_Y-gSysData.trans_voice/2.0f, 0, VOICESET_WIDTH+gSysData.trans_voice, VOICESET_HEIGTH+gSysData.trans_voice, voice_set_on.texID);
			
			
			VoiceSet();
			if(!gSysData.voice_pos)
				DrawRectangle(VOICESET_X-gSysData.trans_voice/2.0f, VOICESET_Y-gSysData.trans_voice/2.0f,  VOICESET_WIDTH+gSysData.trans_voice, VOICESET_HEIGTH+gSysData.trans_voice);
			break;
		case 4:
			
			ShowPic(BLUETOOTHSET_X-gSysData.trans_bluetooth/2.0f, BLUETOOTHSET_Y-gSysData.trans_bluetooth/2.0f, 0, BLUETOOTHSET_WIDTH+gSysData.trans_bluetooth, BLUETOOTHSET_HEIGTH+gSysData.trans_bluetooth, resolution_set_on.texID);
			
			if(gSysData.state !=RESULOTION_SET)
			DrawRectangle(BLUETOOTHSET_X-gSysData.trans_bluetooth/2.0f, BLUETOOTHSET_Y-gSysData.trans_bluetooth/2.0f, BLUETOOTHSET_WIDTH+gSysData.trans_bluetooth, BLUETOOTHSET_HEIGTH+gSysData.trans_bluetooth);
			
			ResolutionSet();
			break;
		default:
			break;
	}
}

void *UpdateUSBThread(void *arg)
{
	if (IsFileExist("/media/usb0/smupdate.dat")==0)
	{
		printf("[update] /media/usb0 not found!\n");
		gSysData.update_start = 10;
		return NULL;
	}
	
	if (mkdir("/tmp/update",0644) <0)
	{
		printf("[update] mkdir err!\n");
		gSysData.update_start = 10;
		return NULL;
	}
	SystemEx("cp /media/usb0/smupdate.dat /tmp/update/smupdate.dat");
	if (update("/tmp/update/smupdate.dat", "/tmp/update/smupdate.tgz"))
	{
		printf("[update] updating\n");
		SystemEx("tar -xvf /tmp/update/smupdate.tgz");
		SystemEx("sync");
		SystemEx("rm -rf /tmp/update");
		gSysData.update_start = 2;  ////////////////////////// =2
	}
	else
	{
		gSysData.update_start = 10;
	}
		return NULL;
	//printf("update = %d\n", gSysData.update_start);
}

void SysInfoRender()
{	
	float base_pos_x=-150.0f;
	float base_pos_y=280.0f;
	glEnable(GL_BLEND);
	glColor3ub(231,111,1);
		
	
	{	FontDraw(base_pos_x, base_pos_y-60,"系统信息",30);
		FontDraw(base_pos_x, base_pos_y-140,"系统版本号:",20);
		FontDraw(base_pos_x, base_pos_y-180,"更新日期:",20);
		FontDraw(base_pos_x, base_pos_y-220,"产品型号:",20);
		glColor3ub(152,152,152);
	
		FontDraw(base_pos_x+200, base_pos_y-140,gSysData.version,20);
		FontDraw(base_pos_x+200, base_pos_y-180,gSysData.update_time,20);
		FontDraw(base_pos_x+200, base_pos_y-220,"SMGO-A1",20);
	}
	
	
	glDisable(GL_BLEND);
	
}


void HandlePos()
{
	if(gSysData.ip_set_pos == 1)
	{
		gSysData.ip_set[pos_addr] = asc[key_pos];
		
		if(pos_addr < 15 && key_pos !=5 && key_pos !=6)
			pos_addr++;	
		//printf("pos_addr:%s  %d\n",gSysData.ip_set,pos_addr);		
	}
	else if(gSysData.ip_set_pos == 2)
	{
		gSysData.mask_set[pos_mask] = asc[key_pos];
		
		if(pos_mask < 15 && key_pos !=5 && key_pos !=6)
			pos_mask++;	
		//printf("pos_mask:%s %d\n",gSysData.mask_set,pos_mask);				
	}
	else if(gSysData.ip_set_pos == 3)
	{
		gSysData.gateway_set[pos_gw] = asc[key_pos];
		
		if(pos_gw < 15 && key_pos !=5 && key_pos != 6)
			pos_gw++;
		//printf("gateway_set:%s %d\n",gSysData.mask_set,pos_gw);						
	}
	else if(gSysData.ip_set_pos == 4)
	{		
		if(pos_dns < 15 && key_pos !=5 && key_pos != 6)
		{
			gSysData.DNS_set[pos_dns] = asc[key_pos];
			pos_dns++;
		}
		//printf("DNS_set:%s %d\n",gSysData.mask_set,pos_dns);						
	}
}
void KeyInit()
{
	pos_addr=0;
	pos_mask=0;
	pos_gw=0;
	pos_dns=0;
	memset(gSysData.ip_set,0,sizeof(gSysData.ip_set));
	memset(gSysData.mask_set,0,sizeof(gSysData.mask_set));
	memset(gSysData.gateway_set,0,sizeof(gSysData.gateway_set));
	memset(gSysData.DNS_set,0,sizeof(gSysData.DNS_set));
}
void IpSetRender()
{	
	float base_pos_x=-150.0f;
	float base_pos_y=280.0f;
	if (gSysData.state == SETTINGS)
	{
		
		glEnable(GL_BLEND);
		
		DrawYeRect(base_pos_x, base_pos_y-50,400, 50);
		

		glColor3ub(255,255,255);
		FontDraw(base_pos_x+100, base_pos_y-37, "配置网络设置",25);

		if (gSysData.getip_state)
		{
			glColor3ub(231,111,1);
			FontDraw(base_pos_x, base_pos_y-160, "IP地址",20);
			FontDraw(base_pos_x, base_pos_y-160-86, "子掩护码",20);
			FontDraw(base_pos_x, base_pos_y-160-86*2, "默认网关",20);
			FontDraw(base_pos_x, base_pos_y-160-86*3, "DNS",20);
			FontDraw(base_pos_x, base_pos_y-160-86*4, "MAC",20);
			glColor3ub(152,152,152);
			FontDraw(base_pos_x, base_pos_y-191,gSysData.ipaddr,25);
			FontDraw(base_pos_x, base_pos_y-195-85,gSysData.mask,25);
			FontDraw(base_pos_x, base_pos_y-195-85*2,gSysData.gateway,25);
			FontDraw(base_pos_x, base_pos_y-195-85*3,gSysData.DNS,25);
			FontDraw(base_pos_x, base_pos_y-195-85*4,gSysData.macaddr,25);
			//glDisable(GL_BLEND);

		}
		else
		{
			
			glColor3ub(231,111,1);
			FontDraw(-25, 100, "网络连接失败",20);
			
		}
		glDisable(GL_BLEND);
	}
	
	switch(gSysData.state)
	{
		
		case IPSET:
		{
			
			glEnable(GL_BLEND);
			DrawWhRect(base_pos_x, base_pos_y-50,400, 50);
			glColor3ub(231,111,1);
			FontDraw(base_pos_x+100, base_pos_y-37, "配置网络设置",25);
			//glDisable(GL_BLEND);

			DrawRectangle(base_pos_x, base_pos_y-50, 400, 50);
			if (gSysData.getip_state)
			{
				glEnable(GL_BLEND);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x, base_pos_y-160, "IP地址",20);
				FontDraw(base_pos_x, base_pos_y-160-86, "子掩护码",20);
				FontDraw(base_pos_x, base_pos_y-160-86*2, "默认网关",20);
				FontDraw(base_pos_x, base_pos_y-160-86*3, "DNS",20);
				FontDraw(base_pos_x, base_pos_y-160-86*4, "MAC",20);
				glColor3ub(152,152,152);
				FontDraw(base_pos_x, base_pos_y-191,gSysData.ipaddr,25);
				FontDraw(base_pos_x, base_pos_y-195-85,gSysData.mask,25);
				FontDraw(base_pos_x, base_pos_y-195-85*2,gSysData.gateway,25);
				FontDraw(base_pos_x, base_pos_y-195-85*3,gSysData.DNS,25);
				FontDraw(base_pos_x, base_pos_y-195-85*4,gSysData.macaddr,25);
				//glDisable(GL_BLEND);
			}
			else
			{
				glEnable(GL_BLEND);
							
				glColor3ub(231,111,1);
				FontDraw(-25, 100, "网络连接失败",20);
				
				
			}
			glDisable(GL_BLEND);

			
		}
			break;
		case IPSET_CON:
			break;
		case IPSET_WRIED_WRIELESS:


			glEnable(GL_BLEND);
			if (gSysData.net_way==0)
			{glEnable(GL_BLEND);
				DrawWhRect(base_pos_x, base_pos_y-130,400, 50);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x+40, base_pos_y-120,"有线网络设置",25);
				if(gSysData.wifi_device == 1)
				{
					DrawYeRect(base_pos_x, base_pos_y-220,400, 50);
					glColor3ub(255,255,255);
					FontDraw(base_pos_x+40, base_pos_y-210,"无线网络设置",25);
					ShowPic(base_pos_x+10, base_pos_y-115, 0.0f, yellow_cir.width, yellow_cir.height, yellow_cir.texID);
				}
				else
				{
					glColor3ub(152,152,152);
					FontDraw(base_pos_x+40, base_pos_y-210,"没有发现无线网卡",25);
				}
				DrawRectangle(base_pos_x, base_pos_y-130, 400, 50);
			}
			else if (gSysData.net_way==1)
			{
				DrawYeRect(base_pos_x, base_pos_y-130,400, 50);
				glColor3ub(255,255,255);
				FontDraw(base_pos_x+40, base_pos_y-120,"有线网络设置",25);
				DrawWhRect(base_pos_x, base_pos_y-220,400, 50);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x+40, base_pos_y-210,"无线网络设置",25);
				ShowPic(base_pos_x+10, base_pos_y-205, 0.0f, yellow_cir.width, yellow_cir.height, yellow_cir.texID);
				
				DrawRectangle(base_pos_x, base_pos_y-220, 400, 50);
			}
			glEnable(GL_BLEND);
			glColor3ub(231,111,1);
			FontDraw(base_pos_x, base_pos_y-30,"选择您的网络设置",25);

			glDisable(GL_BLEND);
			


			break;
		case IPSET_WRIED:
			break;
		case IPSET_WRIELESS:
			
			ShowWifiList();

			break;
		
		case IPSET_WRIELESS_KEY:
			WifiKey();
			
			break;
		case IPSET_AIISC_KEY:
			
			WifiKey();
			
			if (gSysData.wifi_show_keypad == 1)
				ShowSoftKeyPad();
			break;	

		case WIFI_CONNECTING:
			wifi_connecting();
			break;
		
			
			
		case IPSET_WRIED_MANUAL_AUTO:
			glEnable(GL_BLEND);
			if (gSysData.net_way==2)
			{
				glEnable(GL_BLEND);
				DrawWhRect(base_pos_x, base_pos_y-130,400, 50);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x+40, base_pos_y-120,"自动获取网络地址",25);
				DrawYeRect(base_pos_x, base_pos_y-220,400, 50);
				glColor3ub(255,255,255);
				FontDraw(base_pos_x+40, base_pos_y-210,"手动获取网络地址(专家)",25);
				ShowPic(base_pos_x+10, base_pos_y-115, 0.0f, yellow_cir.width, yellow_cir.height, yellow_cir.texID);
				DrawRectangle(base_pos_x, base_pos_y-130, 400, 50);
			}
			else if (gSysData.net_way==3)
			{
				glEnable(GL_BLEND);
				DrawYeRect(base_pos_x, base_pos_y-130,400, 50);
				glColor3ub(255,255,255);
				FontDraw(base_pos_x+40, base_pos_y-120,"自动获取网络地址",25);
				DrawWhRect(base_pos_x, base_pos_y-220,400, 50);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x+40, base_pos_y-210,"手动获取网络地址(专家)",25);
				DrawRectangle(base_pos_x, base_pos_y-220, 400, 50);
				ShowPic(base_pos_x+10, base_pos_y-205, 0.0f, yellow_cir.width, yellow_cir.height, yellow_cir.texID);
			}
			glEnable(GL_BLEND);
			glColor3ub(231,111,1);
			FontDraw(base_pos_x, base_pos_y-30,"选择您的网络设置",25);
			
			glDisable(GL_BLEND);
			break;
		case IPSET_WRIELESS_MANUAL_AUTO:
			break;
		case IPSET_WRIED_MANUAL:
		case SOFTKEY:
			
			if (gSysData.ip_check==0)
			{
				char sel[4] = {0};
				if(1==gSysData.ip_set_pos)
					sel[0] = 3;
				else if(2==gSysData.ip_set_pos)
					sel[1] = 3;
				else if(3==gSysData.ip_set_pos)
					sel[2] = 3;
				else if(4==gSysData.ip_set_pos)
					sel[3] = 3;

					
				glDisable(GL_BLEND);
				ShowPic(base_pos_x, base_pos_y-55*3, sel[0]-3.0f, 400.0f, 50.0f, ip_rect.texID);
				ShowPic(base_pos_x, base_pos_y-55*5, sel[1]-3.0f, 400.0f, 50.0f, ip_rect.texID);
				ShowPic(base_pos_x, base_pos_y-55*7, sel[2]-3.0f, 400.0f, 50.0f, ip_rect.texID);
				ShowPic(base_pos_x, base_pos_y-55*9, sel[3]-3.0f, 400.0f, 50.0f, ip_rect.texID);

				glEnable(GL_BLEND);
				glColor3ub(231,111,1);
				FontDraw(base_pos_x, base_pos_y-30, "请填写以下内容:",25);
				FontDraw(base_pos_x, base_pos_y-100, "IP地址",25);
				FontDraw(base_pos_x, base_pos_y-100-110, "子掩护码",25);
				FontDraw(base_pos_x, base_pos_y-100-110*2, "默认网关",25);
				FontDraw(base_pos_x, base_pos_y-100-110*3, "DNS",25);
				
				glColor3ub(0,0,0);
				FontDraw(base_pos_x+5, base_pos_y-152,gSysData.ip_set,25);
				FontDraw(base_pos_x+5, base_pos_y-152-110,gSysData.mask_set,25);
				FontDraw(base_pos_x+5, base_pos_y-152-110*2,gSysData.gateway_set,25);
				FontDraw(base_pos_x+5, base_pos_y-152-110*3,gSysData.DNS_set,25);
				
				if (gSysData.ip_set_pos==5)
				{
					ShowPic(base_pos_x, base_pos_y-60*10, 0.0f, 224.0f, 52.0f, set_2.texID);
					DrawRectangle(base_pos_x, base_pos_y-60*10, 224.0f, 52.0f);
				}		
				else
				{
					ShowPic(base_pos_x, base_pos_y-60*10, -3.0f, 224.0f, 52.0f, set_2.texID);
				
				}
				if (key_flash)
				{
					
					
					if(gSysData.ip_set_pos == 1)
					{
				
						FontDraw(base_pos_x+20*pos_addr+5, base_pos_y-152-(gSysData.ip_set_pos-1)*110, "l",25);
					}				
					else if(gSysData.ip_set_pos == 2)
					{
						FontDraw(base_pos_x+20*pos_mask+5, base_pos_y-152-(gSysData.ip_set_pos-1)*110, "l",25);
					}
					else if(gSysData.ip_set_pos == 3)
					{
						FontDraw(base_pos_x+20*pos_gw+5, base_pos_y-152-(gSysData.ip_set_pos-1)*110, "l",25);
					}
					else if(gSysData.ip_set_pos == 4)
					{
						FontDraw(base_pos_x+20*pos_dns+5, base_pos_y-152-(gSysData.ip_set_pos-1)*110, "l",25);
					}
				
					
				}
				glDisable(GL_BLEND);
						
			} 
			
			
		
		
			if (gSysData.state == SOFTKEY)
			{
				float key_x=400.0f;
				float key_y = -260.0f;
				ShowPic(key_x, key_y, 0.0f, 400.0f, 550.0f, key_bak);

				if(gSysData.ip_set_pos != 6)
				{
					int x,y;
					x = (key_pos-2)%4;
					if(((key_pos-2)%4) == 0)
						x = 4;
					y = (key_pos-3)/4;

					if(key_pos == 0)
						ShowPic(key_x+17.0f, key_y+17.0f+(116.0f+20.0f)*3.0f, 0.0f, 90.0f, 60.0f, key_select[key_pos]);	
					else if(key_pos == 1)
						ShowPic(key_x+119.0f, key_y+19.0f+(116.0f+20.0f)*3.0f, 0.0f, 120.0f, 60.0f, key_select[key_pos]);
					else if(key_pos == 2)
						ShowPic(key_x+253.0f, key_y+19.0f+(116.0f+20.0f)*3.0f, 0.0f, 130.0f, 60.0f, key_select[key_pos]);	
					else if(key_pos >= 3 && key_pos <= 14)
						ShowPic(key_x+18.0f+95.0f*(x-1), key_y+19.0f+(116.0f+20.0f)*(2-y), 0.0f, 80.0f, 116.0f, key_select[key_pos]);
				}
				
				if (gSysData.key_down==1)
				{
					if(key_pos >= 3 && key_pos <= 14 )
					{
						HandlePos();
					}
					else if (key_pos==0)
					{
						HandlePos();
					}
					else if(key_pos == 1)
					{						
						if(gSysData.ip_set_pos == 1)
						{
							pos_addr--;
							if(pos_addr < 0)
								pos_addr = 0;
							gSysData.ip_set[pos_addr] = 0;
						}
						else if(gSysData.ip_set_pos == 2)
						{
							pos_mask--;
							if(pos_mask < 0)
								pos_mask = 0;
							gSysData.mask_set[pos_mask] = 0;
						}
						else if(gSysData.ip_set_pos == 3)
						{
							pos_gw--;
							if(pos_gw < 0)
								pos_gw = 0;
							gSysData.gateway_set[pos_gw] = 0;
						}	
						else if(gSysData.ip_set_pos == 4)
						{
							pos_dns--;
							if(pos_dns < 0)
								pos_dns = 0;
							gSysData.DNS_set[pos_dns] = 0;
						}
					}
				
					else if(key_pos == 2)
					{
						if(gSysData.ip_set_pos >= 1 && gSysData.ip_set_pos <= 4)
							gSysData.ip_set_pos++;
							
						
						if(gSysData.ip_set_pos ==5)
							gSysData.state = IPSET_WRIED_MANUAL;
					}
					gSysData.key_down=0;
				
				}
				else if(gSysData.key_down ==2)
				{
					if(gSysData.ip_set_pos == 1)
						{
							pos_addr--;
							if(pos_addr < 0)
								pos_addr = 0;
							gSysData.ip_set[pos_addr] = 0;
						}
						else if(gSysData.ip_set_pos == 2)
						{
							pos_mask--;
							if(pos_mask < 0)
								pos_mask = 0;
							gSysData.mask_set[pos_mask] = 0;
						}
						else if(gSysData.ip_set_pos == 3)
						{
							pos_gw--;
							if(pos_gw < 0)
								pos_gw = 0;
							gSysData.gateway_set[pos_gw] = 0;
						}	
						else if(gSysData.ip_set_pos == 4)
						{
							pos_dns--;
							if(pos_dns < 0)
								pos_dns = 0;
							gSysData.DNS_set[pos_dns] = 0;
						}
						gSysData.key_down=0;
				}
				
				else if(gSysData.key_down ==3)
				{
				
					if(gSysData.ip_set_pos >= 1 && gSysData.ip_set_pos <= 4)
						gSysData.ip_set_pos++;
						
					
					if(gSysData.ip_set_pos ==5)
						gSysData.state = IPSET_WRIED_MANUAL;
				
					gSysData.key_down=0;
				}
				
	
			
			}
			glEnable(GL_BLEND);
			glColor3ub(231,111,1);
			if (gSysData.ip_check == 2)
			{
				FontDraw(-25, 100, "手动设置ip成功",20);			
			}
			else if (gSysData.ip_check == 1)
			{
				
				FontDraw(-25, 100, "手动设置ip失败",20);
					
			
			}
			glDisable(GL_BLEND);
	
			break;
		case IPSET_WRIED_AUTO:
			glEnable(GL_BLEND);
			
			glEnable(GL_BLEND);
			glColor3ub(231,111,1);
			if (gSysData.wifi_connecting==1)
			{
				DrawWait(45.0f, 100);
				FontDraw(-25.0f, -100, "正在获取网络...",20);
			}
			else if (gSysData.wifi_connecting==2)
			{			
				if (gSysData.getip_state)
				{
					glColor3ub(231,111,1);
					FontDraw(-25.0f, 100, "自动获取网络成功",20);
					/*
					glColor3ub(231,111,1);
					FontDraw(base_pos_x, base_pos_y-160, "IP地址",25);
					FontDraw(base_pos_x, base_pos_y-160-86, "子掩护码",25);
					FontDraw(base_pos_x, base_pos_y-160-86*2, "默认网关",25);
					FontDraw(base_pos_x, base_pos_y-160-86*3, "DNS",25);
					FontDraw(base_pos_x, base_pos_y-160-86*4, "MAC",25);
					glColor3ub(152,152,152);
					FontDraw(base_pos_x, base_pos_y-191,gSysData.ipaddr,25);
					FontDraw(base_pos_x, base_pos_y-195-85,gSysData.mask,25);
					FontDraw(base_pos_x, base_pos_y-195-85*2,gSysData.gateway,25);
					FontDraw(base_pos_x, base_pos_y-195-85*3,gSysData.DNS,25);
					FontDraw(base_pos_x, base_pos_y-195-85*4,gSysData.macaddr,25);
					*/
				}
				else
				{
					glEnable(GL_BLEND);
					
					glColor3ub(231,111,1);
					FontDraw(-25, 100, "自动获取网络失败",20);					
				}
			}
			glDisable(GL_BLEND);
			break;
		case IPSET_WRIELESS_MANUAL:
			break;
		case IPSET_WRIELESS_AUTO:
			
			
			break;
		default:
			break;
	}
}


















