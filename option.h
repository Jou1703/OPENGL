#ifndef __OPTION_H__
#define __OPTION_H__
#include "includes.h"
#include "draw_utils.h"


enum{
	BASE=1,
	SHIMMER ,
	TV ,
	STEAMOS ,
	SETTINGS ,
	UPDATE ,
	GAME ,
	DOWNLOAD,
	
	IPSET ,
	VOICESET ,
	BLUETOOTHSET,
	DIG_MARKET ,
	HAPPY ,
	MARKET ,
	SOFTKEY ,
	NET,
	RESULOTION_SET,
	
	IPSET_CON,
	IPSET_WRIED_WRIELESS,
	IPSET_WRIED,
	IPSET_WRIELESS,
	IPSET_WRIELESS_KEY,
	IPSET_WRIELESS_SSID,
	IPSET_AIISC_KEY,
	
	
	IPSET_WRIED_MANUAL_AUTO,
	IPSET_WRIELESS_MANUAL_AUTO,
	IPSET_WRIED_MANUAL,
	IPSET_WRIED_AUTO,
	IPSET_WRIELESS_MANUAL,
	IPSET_WRIELESS_AUTO,

	WIFI_CONNECTING,
	TEST,
	
};

enum{
	SHIMMER_MGTV=1,
	MGTV_STEAMOS,
	STEAMOS_MGTV,
	MGTV_SHIMMER,
	
	SHIMMER_GAME,
	MGTV_GAME,
	STEAMOS_GAME,

	GAME_SHIMMER,
	SET_SHIMMER,
	SHUTDOWN_SHIMMER,

	GAME_SET,
	SET_SHUTDOWN,
	SHUTDOWN_SET,
	SET_GAME,

	UPDATE_IPSET,
	IPSET_VOICE,
	VOICE_BLUETOOTH,
	BLUETOOTH_VOICE,
	VOICE_IPSET,
	IPSET_UPDATE,
	DIG_WEB,
	WEB_STORE,
	STORE_WEB,
	WEB_DIG,
	SHIMMER_STEAMOS,
	STEAMOS_SHIMMER,
	
};

typedef struct
{
	GLubyte position;
	GLubyte up;
	GLubyte esc;
} UI_PAR;


typedef struct _SYS_DATA_
{
	int width;
	int height;
	GLubyte state;
	int sys_snd;
	int bios_key;
	int nv_card;
	int bios_key_pass;
	int nv_card_pass;
	int particles_num;
	int steam_game_cp;
	int first_run;
	int sys_time_update;
	int shm_tex_load;
	int set_tex_load;
	int game_tex_load;
	
	int smsv_game_sum;
	int sm_game_pos;
	bool down_load_stop;
	
	UI_PAR set_base;
	UI_PAR set_Shimmer;
	UI_PAR set_Steamos;
	UI_PAR set_tv;
	UI_PAR set_setting;
	UI_PAR set_game;
	UI_PAR set_download;
	GLubyte ip_set_pos;
	GLubyte ip_check;
	
	GLubyte update_pos;
	GLubyte update_start;
	GLubyte mg_tv;

	char wlan_name[8];
	char eth_name[8];

	char ipaddr[20];
	char mask[20];
	char gateway[20];
	char DNS[20];
	char macaddr[20];

	char ip_set[20];
	char mask_set[20];
	char gateway_set[20];
	char DNS_set[20];

	char wifi_key_str[64];
	char wifi_ssid_str[64];
	char wifi_key_tmp[64];
	char wifi_ssid_tmp[64];
	GLbyte assic_key_down;
	GLbyte key_down;
	char wifi_show_keypad;
	char wifi_hide_key;
	char wifi_group;
	char wifi_connecting;

	GLbyte download_pos;
	GLbyte voice_pos;
	GLbyte web_open;
	int snd_vol;
	GLbyte wifi_device;
	GLbyte resolution;
	GLbyte resolution_pos;

	GLbyte base_trans;
	GLbyte music_on;		

	GLbyte set_trans;
	float trans_update;
	float trans_ipset;
	float trans_voice;
	float trans_bluetooth;

	GLbyte shm_trans;
	float trans_dig;
	float trans_web;
	float trans_store;

	int net_wired;
	GLubyte net_way;
	GLubyte net_opt;
	GLubyte net_state;
	GLubyte net_l;
	GLubyte getip_state;
	GLubyte key_state;
	GLuint note_time;

	GLubyte wifi_scan_num;
	GLubyte wifi_ssid;
	GLubyte wifi_con;
	GLubyte wifi_other;
	GLubyte wifi_ssid_write;
	GLubyte wifi_key_write;
	
	GLubyte key_soft_pos;

	GLuint sys_update_time;
	char version[64];
	char serial_num[64];
	int download;
	
	int game_local;
	int mounting,mounted;
	int sg_pos,mg_pos;
	int sg_posdown,sg_posup,mg_posdown,mg_posup;
	int game_num,mgame_num;
	int game_save,game_move;
	int game_size,Sgame_size;
	int game_cp;
	int cping,mcping,rming,getsizeing,getmsizeing;
	int conl,conlture;
	int game_selected,sure_copy,mgame_selected,msure_copy;
	int sure_rm,select_del;
	
	char set_resolution[128];
	unsigned int bpoint;
	long int zongchang;
	int update_way;
	char update_time[64];
	float rate;
	GLbyte off;
	
}TSysData,*PSysData;

typedef struct _SM_GAME_
{
	char game_type;
	char game_name[128];
	char game_img[128];
	TextureImage game_tex;
	bool img_load;
	int  game_num;
	char game_description[256];
	char game_company_name[128];
	char game_online_time[16];
	bool game_authority;
	
}TSMGame,*PSMGame;

#endif //__OPTION_H__
