#ifndef __SEARCH_H__

#define __SEARCH_H__
void ScanSMGames();
void ScanGames(int*);
void ShowNameUp(int n);
void ShowNameDown(int n);
void ShownameRm(int n);
void ScanMobileDisk(int *num);
void Clearlist();
struct SteamLibInfo{
	char dir[256];
	char name[256];
	char acf[256];
	int number;
	int showname;
	char dis_size[128];
};


#define GAME_PATH "/home/steam/.local/share/Steam/steamapps/"
#define DISK_MOUNT_PATH "/media/usb0/shimmergames/"
#define STEAM_GAME_PATH "/home/steam/.local/share/Steam/steamapps/common/"
#define STEAM_GAMEACF_PATH "/home/steam/.local/share/Steam/steamapps/"

#endif//__SEARCH_H__
