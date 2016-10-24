#include "includes.h"
#include "draw_utils.h"
#include "option.h"


typedef struct {
	TextureImage img;
	char   value;
	float x;
	float y;
}KeyElement;

KeyElement key[42],key1[42];
TextureImage keypad1,keypad2;

extern TSysData gSysData;
static GLbyte key_ssid_pos=0;
static GLbyte key_key_pos=0;


char keypad[42]={'~','!','@','#',' ','A','B','C','D','E','F','G',
		'?','%','^','&','H','I','J','K','L','M',
		'*','(',')','/','N','O','P','Q','R','S',
		',',' ',' ','T','U','V','W','X','Y','Z'};
char key1pad[42]={'1','2','3','4',' ','a','b','c','d','e','f','g',
		'5','6','7','+','h','i','j','k','l','m',
		'8','9','0','-','n','o','p','q','r','s',
		'.',' ',' ' ,'t','u','v','w','x','y','z'};

#define SOKEY_X (-127.0f)
#define  SOKEY_Y (-15.0f)
float posx[42]=
{
	SOKEY_X+57*0,
	SOKEY_X+57*1,
	SOKEY_X+57*2,
	SOKEY_X+57*3,
	SOKEY_X+57*4,
	SOKEY_X+29+57*5,
	SOKEY_X+29+57*6,
	SOKEY_X+29+57*7,
	SOKEY_X+29+57*8,
	SOKEY_X+29+57*9,
	SOKEY_X+29+57*10,
	SOKEY_X+29+57*11,

	SOKEY_X,
	SOKEY_X+63*1,
	SOKEY_X+137,
	SOKEY_X+211,
	SOKEY_X+314,
	SOKEY_X+314+66,
	SOKEY_X+314+66*2+1,
	SOKEY_X+314+66*3+1,
	SOKEY_X+314+66*4+2,
	SOKEY_X+314+66*5+2,

	SOKEY_X,
	SOKEY_X+63*1,
	SOKEY_X+137,
	SOKEY_X+211,
	SOKEY_X+314,
	SOKEY_X+314+66,
	SOKEY_X+314+66*2+1,
	SOKEY_X+314+66*3+1,
	SOKEY_X+314+66*4+2,
	SOKEY_X+314+66*5+2,

	SOKEY_X+57*0,
	SOKEY_X+57*1,
	SOKEY_X+57*4,

	SOKEY_X+57*5+29,
	SOKEY_X+57*6+29,
	SOKEY_X+57*7+29,
	SOKEY_X+57*8+29,
	SOKEY_X+57*9+29,
	SOKEY_X+57*10+29,
	SOKEY_X+57*11+29,
	
};
float posy[42]=
{
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,
	SOKEY_Y,

	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,
	SOKEY_Y-72,

	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,
	SOKEY_Y-141,

	SOKEY_Y-231,
	SOKEY_Y-231+10,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
	SOKEY_Y-231,
};

static GLuint keypad_change;

int LoadKeyImg()
{
	int i=0;
	char img_path[128] = {0};

	for( i=0; i<42; ++i)
	{
		sprintf(img_path,"key/0%d.bmp",i+1);
		LoadBmpTex(&key[i].img ,get_path(img_path));
		sprintf(img_path,"key/%d.bmp",i+1);
		LoadBmpTex(&key1[i].img ,get_path(img_path));
	}
	LoadBmpTex(&keypad1 ,get_path("key/86.bmp"));
	LoadBmpTex(&keypad2 ,get_path("key/87.bmp"));
	
	for( i=0; i<42; ++i)
	{
		key[i].value = keypad[i];
		key1[i].value = key1pad[i];

		key[i].x=posx[i];
		key[i].y=posy[i];

		key1[i].x=posx[i];
		key1[i].y=posy[i];
	}	
}

void SoftKeyInit()
{
	gSysData.key_soft_pos = 1;
	key_ssid_pos = 0;
	key_key_pos= strlen(gSysData.wifi_key_tmp);
}

void ShowSoftKeyPad()
{
	float fix_x=0.0f;
	float fix_y=0.0f;
	if(gSysData.wifi_other==0)
	{
		fix_y += 20.0f;
	}
	else
	{
		if(gSysData.wifi_ssid_write)
			fix_y += 55.0f;
		else if(gSysData.wifi_key_write)
			fix_y -= 100.0f;
	}
	if (keypad_change)
		ShowPic(SOKEY_X-28.0f+fix_x, SOKEY_Y-320.0f+fix_y, 0.0f, keypad1.width, keypad1.height, keypad1.texID);
	else 
		ShowPic(SOKEY_X-28.0f+fix_x, SOKEY_Y-320.0f+fix_y, 0.0f, keypad2.width, keypad2.height, keypad2.texID);
	for (int i=0; i<42 ;++i)
	{
		if ((gSysData.key_soft_pos-1) == i)
		{
			
			if (keypad_change)
				ShowPic(key[i].x+fix_x,key[i].y+fix_y, 0.0f, key[i].img.width, key[i].img.height, key[i].img.texID);
			else
				ShowPic(key1[i].x+fix_x,key1[i].y+fix_y, 0.0f, key1[i].img.width, key1[i].img.height, key1[i].img.texID);
					
		}
	}
	

	if (gSysData.assic_key_down==1)
	{
		if(gSysData.key_soft_pos == 5)
		{
			if(gSysData.wifi_ssid_write)
			{
				if(key_ssid_pos>0)
					key_ssid_pos--;
				gSysData.wifi_ssid_tmp[key_ssid_pos]=0;
			}
			else if(gSysData.wifi_key_write)
			{
				if(key_key_pos>0)
					key_key_pos--;
				gSysData.wifi_key_tmp[key_key_pos]=0;
			}
		}
		else if(gSysData.key_soft_pos==34)
		{
			gSysData.wifi_show_keypad = 0;
			gSysData.state = IPSET_WRIELESS_KEY;
		}
		else if (gSysData.key_soft_pos==35)
		{
			keypad_change=!keypad_change;
		}
		else
		{
			
			if(keypad_change)
			{		
					if(gSysData.wifi_ssid_write)
						gSysData.wifi_ssid_tmp[key_ssid_pos]=key[gSysData.key_soft_pos-1].value;
					else if(gSysData.wifi_key_write)
						gSysData.wifi_key_tmp[key_key_pos]=key[gSysData.key_soft_pos-1].value;
			}			
			else
			{	
					if(gSysData.wifi_ssid_write)
						gSysData.wifi_ssid_tmp[key_ssid_pos]=key1[gSysData.key_soft_pos-1].value;
					else if(gSysData.wifi_key_write)
						gSysData.wifi_key_tmp[key_key_pos]=key1[gSysData.key_soft_pos-1].value;
			}
			if(gSysData.wifi_ssid_write)
			{
				if(	key_ssid_pos<20)
				{
					key_ssid_pos++;
				}
			}
			else if(gSysData.wifi_key_write)
			{
				if(key_key_pos<20)
				{
					key_key_pos++;
				}
			}

		}
		gSysData.assic_key_down=0;
		
		
	}
	else if(gSysData.assic_key_down==2)
	{	
		if(gSysData.wifi_ssid_write)
		{
			if(key_ssid_pos>0)
				key_ssid_pos--;
			gSysData.wifi_ssid_tmp[key_ssid_pos]=0;
		}
		else if(gSysData.wifi_key_write)
		{
			if(key_key_pos>0)
				key_key_pos--;
			gSysData.wifi_key_tmp[key_key_pos]=0;
		}
		gSysData.assic_key_down=0;
	}
	else if(gSysData.assic_key_down==3)
	{	
		keypad_change=!keypad_change;
		
		gSysData.assic_key_down=0;
	}
}
