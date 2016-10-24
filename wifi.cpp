#include "includes.h"

#include "fun.h"
#include "draw_utils.h"
#include "option.h"
#include "softkey.h"
#include "wifi.h"
#include "net.h"
#include "conf.h"


int wifi_online;
int wifi_num;


wifi_info wifi_res[32];
extern TSysData gSysData;
void WifiStatusClear();
static int ConvertSignQulity(int qulity);

void CleanString( char *str )
{
   
      char *start = NULL;
      char *end = NULL;
      char *p = str;

      while ( ( *p != 0) && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t') )
     {
         p++;
     }
     if ( *p == 0 )

     {

         *str = 0;

         return;

     }

     else

     {

         start = p;              // mark the first no-blank position
          while( *(p+1)  != 0 )   // move p to str end
 
         {
              p++;
          }
      }
      while ( *p == ' ' || *p == '\r' || *p == '\n' || *p == '\t' )

     {
          p--;
      }
      end = p;
      int newLen = (int)(end - start + 1);

     memmove( str, start, newLen );
 
     *(str + newLen ) = 0;

} 
static int ConvertSignQulity(int qulity)
{
	if (qulity>0 && qulity<=33){
		return 1;
	} else if(qulity>33 && qulity<=66) {
		return 2;
	} else if(qulity>66 && qulity<=100) {
		return 3;
	}
	return 0;
}



void GetWifiInfo()
{
	FILE *fp; 
	char buf[128]={0};
	char* str_pos;
	char qulity[8]={0};
	
	int wifi_scan = 0;
	fp = popen("nmcli dev wifi", "r");
	if(NULL == fp)  
	{  
		LOG_ERR("popen error");  
		return ;  
	}  
	WifiStatusClear(); 
	
	fgets(buf, sizeof(buf), fp);
	while(fgets(buf,sizeof(buf),fp) != NULL)
	{
		str_pos = strstr(buf,"Mbit/s");
		str_pos+=strlen("Mbit/s");
		int qul=0;
		while(*str_pos != '\n')
		{
			if (*str_pos != ' ')
			{
				qulity[qul] = *str_pos;
				qul++;
			}
			if(qul>2)
				break;
			str_pos++;
		}
		wifi_res[wifi_scan].qulity = atoi(qulity);
		if(buf[0]=='*')
		{
			wifi_res[wifi_scan].status = 1;
		}
		str_pos = strstr(buf,"Infra");
		memcpy(wifi_res[wifi_scan].ssid_str,buf+2,str_pos-buf-2);
		CleanString(wifi_res[wifi_scan].ssid_str);
		if(wifi_res[wifi_scan].qulity<65)
			continue;
		wifi_scan++;
		if(wifi_scan>18)
			break;
	}
	fclose(fp);	
	gSysData.wifi_scan_num = wifi_scan;
	for(int i=0;i<gSysData.wifi_scan_num;++i)
	{
		LOG_INFO("ssid:%s qulity:%d",wifi_res[i].ssid_str,wifi_res[i].qulity);
	}
	
}
void WifiStatusClear()
{
	for(int i=0;i<32;++i)
	{
		memset(&wifi_res[i],0,sizeof(wifi_info));
	}	
}

void WifiConnect()
{
	ReadConfStr("wifi_ssid",gSysData.wifi_ssid_str);
	ReadConfStr("wifi_key",gSysData.wifi_key_str);
	
	char cmd[256] = {0};
	sprintf(cmd,"nmcli dev wifi con %c%s%c password %c%s%c",'"',gSysData.wifi_ssid_str,'"','"',gSysData.wifi_key_str,'"');	
	SystemEx(cmd);
}

void wifi_off()
{
	char cmd[256] = {0};
	sprintf(cmd,"nmcli radio wifi off");
	LOG_INFO("%s",cmd);	
	SystemEx(cmd);
}

void wifi_on()
{
	char cmd[256] = {0};
	sprintf(cmd,"nmcli radio wifi on");
	LOG_INFO("%s",cmd);	
	SystemEx(cmd);
}
void WifiDisconnect()
{
	char cmd[256] = {0};
	sprintf(cmd,"nmcli dev disconnect %s",gSysData.wlan_name);
	LOG_INFO("%s",cmd);	
	SystemEx(cmd);
}

void *WifiConnect(void *)
{
	WifiDisconnect();
	WifiConnect();
	get_wifi_state();
	gSysData.wifi_connecting=2;
	
	return NULL;
}

void *WiredConnect(void *)
{
	char netstr[64]= {0};
	gSysData.ip_check=0;
	gSysData.getip_state = 0;	
	
	sprintf(netstr,"nmcli device disconnect %s",gSysData.wlan_name);
	SystemEx(netstr);
	memset(netstr,0,sizeof(netstr));						
	sprintf(netstr,"nmcli device disconnect %s",gSysData.eth_name);
	SystemEx(netstr);
	memset(netstr,0,sizeof(netstr));
	sprintf(netstr,"nmcli device connect %s",gSysData.eth_name);
	SystemEx(netstr);
	
	
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
	WriteConfInt("net_wired",1);
	gSysData.wifi_connecting=2;
	return NULL;
}
void WiredConnectThread()
{
	gSysData.wifi_connecting=1;
	thread_create(WiredConnect,NULL);
}
void WifiConnectThread()
{
	gSysData.wifi_connecting=1;
	thread_create(WifiConnect,NULL);
	WriteConfInt("net_wired",0);
}

int get_wifi_state()
{
	wifi_online = GetNetStatus(gSysData.wlan_name);
	return wifi_online;
}	


/*
int get_net_status()
{
	
	

	
	
	FILE *fp; 
	char buf[128]={0};
	char *result;
	char *connecting;

	
	fp = popen("nmcli dev status", "r");  
    if(NULL == fp)  
    {  
        perror("popen error");  
        return 0;  
    }   

	while(1)
	{	
		if(fgets(buf, sizeof(buf), fp) != NULL)  
		{  
			
			result = strstr(buf,"wlan0");
			
			if (result == NULL)
			{
				continue;
			}
			else
			{
				printf("%s\n",buf);
				result = strstr(buf,"disconnected");
				
				
				if(result != NULL)
				{
					wifi_online=2;
				}
				else
				{
					wifi_online=1;
				}
				connecting = strstr(buf,"connecting");
				
				if(connecting != NULL)
				{
					wifi_online=2;
				}
				else
				{
					wifi_online=1;
				}
				
			}
		}
		else
			break;
	} 
	printf("wifi online:%d\n",wifi_online);
	pclose(fp);
	return 1;
	
}
*/

static TextureImage search_wifi_fail,wifi_scan_font,wifi_key,wifi_key_on;
static TextureImage wifi_1,wifi_1on,wifi_2,wifi_2on,wifi_3,wifi_3on;
static TextureImage wifi_rect;
static TextureImage wifi_key_font,wifi_show_key_font,key_hide,key_hide_on,wifi_conn;
static TextureImage ssid_write,wifi_other,wifi_other_on;


void WifiImgLoad()
{
	LoadTgaTex(&wifi_scan_font,get_path("wifi_scan_font.tga"));
	LoadTgaTex(&search_wifi_fail,get_path("search_wifi_fail.tga"));
	
	LoadTgaTex(&wifi_key,get_path("wifi_key.tga"));
	LoadTgaTex(&wifi_key_on,get_path("wifi_key_on.tga"));
	LoadTgaTex(&wifi_1,get_path("wifi_1.tga"));
	LoadTgaTex(&wifi_1on,get_path("wifi_1on.tga"));
	LoadTgaTex(&wifi_2,get_path("wifi_2.tga"));
	LoadTgaTex(&wifi_2on,get_path("wifi_2on.tga"));
	LoadTgaTex(&wifi_3,get_path("wifi_3.tga"));
	LoadTgaTex(&wifi_3on,get_path("wifi_3on.tga"));


	LoadTgaTex(&wifi_key_font,get_path("wifi_key_font.tga"));
	LoadTgaTex(&wifi_show_key_font,get_path("wifi_show_key_font.tga"));
	LoadTgaTex(&ssid_write,get_path("ssid_write.tga"));
	LoadTgaTex(&wifi_other,get_path("wifi_other.tga"));
	LoadTgaTex(&wifi_other_on,get_path("wifi_other_on.tga"));

	LoadBmpTex(&wifi_rect ,get_path("wifi_rect.bmp"));

	LoadTgaTex(&key_hide ,get_path("white_cir.tga"));
	LoadTgaTex(&key_hide_on ,get_path("yellow_cir.tga"));

	LoadBmpTex(&wifi_conn ,get_path("set_2.bmp"));
	
}
void ShowWifiList()
{
	float base_pos_x=-150.0f;
	float base_pos_y=280.0f;
	
	//ShowPic(base_pos_x, 200.0f-(wifi_rect.height+4) *(gSysData.wifi_ssid%10), 0.0f, wifi_rect.width-280, wifi_rect.height, wifi_rect.texID);
	DrawWhRect(base_pos_x, base_pos_y-100-50*(gSysData.wifi_ssid%10),500, 40);
	DrawRectangle(base_pos_x, base_pos_y-100-50*(gSysData.wifi_ssid%10), 500, 40);
	
	
	glEnable(GL_BLEND);
	glColor3ub(231,111,1);
	FontDraw(base_pos_x, base_pos_y-30, "搜索到的无线网络:",25);
	//ShowPic(base_pos_x, base_pos_y-wifi_scan_font.height, 0.0f, wifi_scan_font.width, wifi_scan_font.height, wifi_scan_font.texID);

	int i=0;
	
	for(i=0; i<gSysData.wifi_scan_num; ++i)
	{
		//dis_text(-85.0f, 210.0f-i*50, 0,iwresult[i].ap_name,1);
		if(i<10 && gSysData.wifi_group==0)
		{
			if (gSysData.wifi_ssid == i)
			{
				//ShowPic(-85.0f, 200.0f-(wifi_rect.height+4) *i, 0.0f, wifi_rect.width, wifi_rect.height, wifi_rect.texID);
				glDisable(GL_TEXTURE_2D); 
				glColor3f (0.9f, 0.43f, 0.0f);
				if(wifi_res[i].status)
					Drawtext((char*)"*",base_pos_x+10, base_pos_y-90-i*50);
				
				//Drawtext(wifi_res[i].ssid_str,base_pos_x+30, base_pos_y-90-i*50);
				FontDraw(base_pos_x+30, base_pos_y-90-i*50, wifi_res[i].ssid_str,20);
			
				glEnable(GL_TEXTURE_2D); 
				if (i!=gSysData.wifi_scan_num)
				{
					ShowPic(base_pos_x+350, base_pos_y-92-i*50, 0.0f, wifi_key_on.width, wifi_key_on.height, wifi_key_on.texID);
					if (ConvertSignQulity(wifi_res[i].qulity)==1)
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_1on.width, wifi_1on.height, wifi_1on.texID);
					else if (ConvertSignQulity(wifi_res[i].qulity)==2)
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_2on.width, wifi_2on.height, wifi_2on.texID);
					else
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_3on.width, wifi_3on.height, wifi_3on.texID);
				}
			}
			else 
			{
				glDisable(GL_TEXTURE_2D); 
				glColor3f (0.7f, 0.7f, 0.7f);
				if(wifi_res[i].status)
					Drawtext((char*)"*",base_pos_x+10, base_pos_y-90-i*50);
				
				//Drawtext(wifi_res[i].ssid_str,base_pos_x+30, base_pos_y-90-i*50);
				FontDraw(base_pos_x+30, base_pos_y-90-i*50, wifi_res[i].ssid_str,20);
			
				glEnable(GL_TEXTURE_2D); 
				if (i!=gSysData.wifi_scan_num)
				{
					ShowPic(base_pos_x+350, base_pos_y-90-i*50, 0.0f, wifi_key.width, wifi_key.height, wifi_key.texID);

					if (ConvertSignQulity(wifi_res[i].qulity)==1)
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_1.width, wifi_1.height, wifi_1.texID);
					else if (ConvertSignQulity(wifi_res[i].qulity)==2)
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_2.width, wifi_2.height, wifi_2.texID);
					else
						ShowPic(base_pos_x+400, base_pos_y-92-i*50, 0.0f, wifi_3.width, wifi_3.height, wifi_3.texID);
				}
			}

		}
		else if(i>=10 && gSysData.wifi_group)
		{
			
			if (gSysData.wifi_ssid == i)
			{

				glDisable(GL_TEXTURE_2D); 
				glColor3f (0.9f, 0.43f, 0.0f);
				if(wifi_res[i].status)
					Drawtext((char*)"*",base_pos_x+10, base_pos_y-90-(i%10)*50);
				
				//Drawtext(wifi_res[i].ssid_str,base_pos_x+30, base_pos_y-90-(i%10)*50);
				FontDraw(base_pos_x+30, base_pos_y-90-(i%10)*50, wifi_res[i].ssid_str,20);
			
				glEnable(GL_TEXTURE_2D); 
				if (i!=gSysData.wifi_scan_num)
				{
					ShowPic(base_pos_x+350, base_pos_y-92-(i%10)*50, 0.0f, wifi_key_on.width, wifi_key_on.height, wifi_key_on.texID);
					if (ConvertSignQulity(wifi_res[i].qulity)==1)
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_1on.width, wifi_1on.height, wifi_1on.texID);
					else if (ConvertSignQulity(wifi_res[i].qulity)==2)
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_2on.width, wifi_2on.height, wifi_2on.texID);
					else
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_3on.width, wifi_3on.height, wifi_3on.texID);
				}

			}
			else 
			{
				glDisable(GL_TEXTURE_2D); 
				glColor3f (0.7f, 0.7f, 0.7f);
				if(wifi_res[i].status)
					Drawtext((char*)"*",base_pos_x+10, base_pos_y-90-(i%10)*50);
				
				Drawtext(wifi_res[i].ssid_str,base_pos_x+30, base_pos_y-90-(i%10)*50);
			
				glEnable(GL_TEXTURE_2D); 
				if (i!=gSysData.wifi_scan_num)
				{
					ShowPic(base_pos_x+350, base_pos_y-92-(i%10)*50, 0.0f, wifi_key.width, wifi_key.height, wifi_key.texID);

					if (ConvertSignQulity(wifi_res[i].qulity)==1)
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_1.width, wifi_1.height, wifi_1.texID);
					else if (ConvertSignQulity(wifi_res[i].qulity)==2)
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_2.width, wifi_2.height, wifi_2.texID);
					else
						ShowPic(base_pos_x+400, base_pos_y-92-(i%10)*50, 0.0f, wifi_3.width, wifi_3.height, wifi_3.texID);
				}

			}

		}
		
	}
	memcpy(gSysData.wifi_ssid_tmp,wifi_res[gSysData.wifi_ssid].ssid_str,64);
	
	if(gSysData.wifi_scan_num<10)
		ShowPic(base_pos_x+30, base_pos_y-90-gSysData.wifi_scan_num*50, 0.0f, wifi_other.width, wifi_other.height, wifi_other.texID);
	else if (gSysData.wifi_scan_num>=10 && gSysData.wifi_group)
		ShowPic(base_pos_x+30, base_pos_y-90-(gSysData.wifi_scan_num%10)*50, 0.0f, wifi_other.width, wifi_other.height, wifi_other.texID);
	
	if(gSysData.wifi_ssid == gSysData.wifi_scan_num)
		ShowPic(base_pos_x+30, base_pos_y-90-(gSysData.wifi_scan_num%10)*50, 0.0f, wifi_other_on.width, wifi_other_on.height, wifi_other_on.texID);


	
	
	glDisable(GL_BLEND);
	//printf("ssid = %s\n",gSysData.wifi_ssid_tmp);
}




void WifiKey()
{

	float x = -155.0f;
	float y = 310.0f;

	float key_font_y =        y;
	float show_key_font_y=    y-200;
	float rect_y=             y-160;
	float con_y = 			  y-380;

	
	float rect_ssid_y = y-130;

	

	float show_key_font_z=0;
	float rect_z=0;
	float rect_ssid_z=0;
	float con_z=0;

	if(gSysData.wifi_con==0)
	{
		rect_ssid_z=0.0f;
		rect_z = -3.0f;
		show_key_font_z=-3.0f;
		con_z = -3.0f;
	}
	else if(gSysData.wifi_con==1)
	{
		rect_ssid_z=-3.0f;
		rect_z = 0.0f;
		show_key_font_z=-3.0f;
		con_z = -3.0f;
	}
	else if(gSysData.wifi_con==2)
	{
		rect_ssid_z=-3.0f;
		rect_z = -3.0f;
		show_key_font_z=0.0f;
		con_z = -3.0f;
	}
	else if(gSysData.wifi_con==3)
	{
		rect_ssid_z=-3.0f;
		rect_z = -3.0f;
		show_key_font_z=-3.0f;
		con_z = 0.0f;
		if(gSysData.wifi_other)
			DrawRectangle(x,con_y-120, wifi_conn.width-56, wifi_conn.height-13);
		else
			DrawRectangle(x,con_y, wifi_conn.width-56, wifi_conn.height-13);
	}	
	
	glEnable(GL_BLEND);
	glColor3ub(231,111,1);
	if (gSysData.wifi_other)
	{
		FontDraw(x, key_font_y-60, "请输入网络名称",25);
		
		key_font_y-=150;
		show_key_font_y-=120;
		con_y-=120; 
		rect_y-=120;
	}
	//ShowPic(x, key_font_y-70, 0.0f, wifi_key_font.width, wifi_key_font.height, wifi_key_font.texID);
	
	FontDraw(x, key_font_y-60, "网络访问密码",25);
	glColor3ub(231,111,1);
	FontDraw(x+30, show_key_font_y, "隐藏密码",25);
	//ShowPic(x+30, show_key_font_y, 0.0f, wifi_show_key_font.width, wifi_show_key_font.height, wifi_show_key_font.texID);
	glDisable(GL_BLEND);


	if(gSysData.wifi_other)
		ShowPic(x, rect_ssid_y, rect_ssid_z, 400, wifi_rect.height, wifi_rect.texID);
	
	ShowPic(x, rect_y, rect_z, 400, wifi_rect.height, wifi_rect.texID);
	glEnable(GL_BLEND);
	ShowPic(x, show_key_font_y, show_key_font_z, key_hide.width+5, key_hide.height+5, key_hide.texID);
	if(gSysData.wifi_hide_key)
	{
		ShowPic(x+4, show_key_font_y+4, show_key_font_z, key_hide_on.width, key_hide_on.height, key_hide_on.texID);
	}
	
	ShowPic(x,con_y, con_z, wifi_conn.width-56, wifi_conn.height-13, wifi_conn.texID);

	//glDisable(GL_TEXTURE_2D); 
	glEnable(GL_BLEND);
	glColor3f (0.0f, 0.0f, 0.0f);

	if(gSysData.wifi_other)
	{
		FontDraw(-145.0f, rect_ssid_y+10, gSysData.wifi_ssid_tmp,20);
		//Drawtext(gSysData.wifi_ssid_tmp,-145.0f, rect_ssid_y+10);
	}

	char hide_key[40]={0};
	for(unsigned int i=0;i<strlen(gSysData.wifi_key_tmp);++i)
	{
		hide_key[i]='*';
	}
	
		
	if (gSysData.wifi_hide_key)
		//Drawtext(hide_key,-65.0f, rect_y);
		FontDraw(-145.0f, rect_y+10, hide_key,25);
	else
	{
		FontDraw(-145.0f, rect_y+10, gSysData.wifi_key_tmp,20);
		//Drawtext(gSysData.wifi_key_tmp,-145.0f, rect_y+10);
	}
}
void wifi_connecting()
{
	glEnable(GL_BLEND);
	glColor3ub(231,111,1);
	char ssid[128] = {0};
	if (gSysData.wifi_connecting==1)
	{
		snprintf(ssid,sizeof(ssid),"正在连接 %s",gSysData.wifi_ssid_str);
		DrawWait(45.0f, 100);
		FontDraw(-25.0f, -100, ssid,20);
	}
	else if (gSysData.wifi_connecting==2)
	{
		
		if(wifi_online==0)
		{
			snprintf(ssid,sizeof(ssid),"%s 连接成功",gSysData.wifi_ssid_str);
		}
		else
		{
			snprintf(ssid,sizeof(ssid),"%s 连接失败",gSysData.wifi_ssid_str);
		}
		FontDraw(-25.0f, 100, ssid,20); 
	}
}




