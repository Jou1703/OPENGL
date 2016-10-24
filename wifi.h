#ifndef __WIFI_H__
#define __WIFI_H__


typedef struct 
{
	char ssid_str[128];
	GLuint qulity;
	char status;
}wifi_info;


void GetWifiInfo();
void WifiConnectThread();
void wifi_connecting();

void wifi_on();
void WifiConnect();
void wifi_disconnect();
int get_net_status();
int get_wifi_state();

void ShowWifiList();
void WifiImgLoad();
void WifiKey();
void WifiSSID();

void wifi_off();

void WiredConnectThread();


#endif //__WIFI_H__
