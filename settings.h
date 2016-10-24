#ifndef _SETTINGS_H_
#define _SETTINGS_H_


#define SETTINGS_MINPOS 1
#define SETTINGS_MAXPOS 4

#define PROUPDATE_X -590.0f
#define PROUPDATE_Y 150.0f
#define PROUPDATE_Z -3.0f
#define PROUPDATE_WIDTH 300.0f
#define PROUPDATE_HEIGTH 130.0f

#define IPSET_X PROUPDATE_X
#define IPSET_Y PROUPDATE_Y-155.0f
#define IPSET_Z -3.0f
#define IPSET_WIDTH 300.0f
#define IPSET_HEIGTH 130.0f

#define VOICESET_X PROUPDATE_X
#define VOICESET_Y PROUPDATE_Y-310.0f
#define VOICESET_Z -3.0f
#define VOICESET_WIDTH 300.0f
#define VOICESET_HEIGTH 130.0f

#define BLUETOOTHSET_X PROUPDATE_X
#define BLUETOOTHSET_Y PROUPDATE_Y-465.0f
#define BLUETOOTHSET_Z -3.0f
#define BLUETOOTHSET_WIDTH 300.0f
#define BLUETOOTHSET_HEIGTH 130.0f

#define RESOLUTION_X PROUPDATE_X
#define RESOLUTION_Y PROUPDATE_Y-620.0f
#define RESOLUTION_Z -3.0f
#define RESOLUTION_WIDTH 300.0f
#define RESOLUTION_HEIGTH 130.0f

void SettingsSystemRender(void);
void SettingsPicLoad(void);
void *UpdateUSBThread(void *arg);
void VoiceSet(void);
void KeyInit();

void SysInfoRender();
void IpSetRender();
int SetIp(char *ifname, char *Ipaddr, char *mask,char *gateway,char *DNS);

#endif  //_SETTINGS_H_
