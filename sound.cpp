
#include "includes.h"
#include "fun.h"
#include "all_path.h"
#include "alsa_src/AlsaPlayCtrl_F.h"
#include "alsa_src/AlsaPcm_F.h"
#include "alsa_src/AlsaWaveData_F.h"
#include "option.h"

CAlsaPlayCtrl gBackSnd;
CAlsaWaveData gSndMoveData;
CAlsaPcm gSndMove;

extern TSysData gSysData;

void SndSet(int n)
{
	if(gSysData.sys_snd!=1)
		return;
	char cmd[128] = {0};
	sprintf(cmd,"amixer  cset numid=3 %d%%",n);
	SystemEx(cmd);
}

void SoundInit()
{
	if(gSysData.sys_snd!=1)
		return;
	
	//gBackSnd.AddToList("/home/steam/.shimmer/src/time.wav");
	gBackSnd.AddToList(AUTO_PATH);

	gSndMoveData.LaunchFile(FIRE_PATH);
	gSndMove.AttachToWaveData(&gSndMoveData);
	
	gBackSnd.InitPlayPos();
	gBackSnd.SetPlayMode(CAlsaPlayCtrl::PlayListModeListRepeat);
	SndSet(gSysData.snd_vol);
	
}

void SndPlay()
{
	if(gSysData.sys_snd!=1)
		return;
	
	gBackSnd.PlayList();
}

void SndStop()
{
	if(gSysData.sys_snd!=1)
		return;
	gBackSnd.PauseCurrent();
}
void SndResume()
{
	if(gSysData.sys_snd!=1)
		return;
	gBackSnd.ResumeCurrent();
}
void SndMove()
{
	if(gSysData.sys_snd!=1)
		return;
	gSndMove.ForceRePlay();
}



