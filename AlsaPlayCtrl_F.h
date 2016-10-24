/*
 * AlsaPlayCtrl_F.h
 *
 *  Created on: Jul 10, 2012
 *      Author: lucid
 */

#ifndef __ALSA_PLAY_CTRL_F_H__
#define __ALSA_PLAY_CTRL_F_H__

#include <vector>
using std::vector;
#include <string>
using std::string;

#include "AlsaPcm_F.h"
#include "AlsaWaveData_F.h"

class CAlsaPlayCtrl
{
public:
	enum PlayListMode
	{
		PlayListModeNormal = 0,			//Play one by one ,and end
		PlayListModeListRepeat = 1,		//Loop in list
		PlayListModeSingleRepeat = 2,	//Loop at one song
		PlayListModeSingleOnce = 3,		//Play at one song once
		PlayListModeMax = PlayListModeSingleOnce
	};

protected:
	CThread		m_ThPlayList;
	vector<string>		m_vtPlayList;
	vector<string>::iterator m_itListPlaying;
	CAlsaPcm			m_Pcm;
	CAlsaWaveData		m_WaveData[2];
	CAlsaWaveData*	m_pPlayingData;
	CAlsaWaveData*	m_pPrepareData;
	U32					m_dwPlayMode;
	int					m_iPlayingIndex;
	int					m_bRepeatLoop;
	

	volatile int		m_bStopList;
	volatile int		m_bIsListPlaying;

	vector<string>::iterator GetCurrentPlayIterator();
	int SetCurrentPlayIterator(vector<string>::iterator it);
	U32 GetCurrentPlayIndex();
	int SetCurrentPlayIndex(int iIndex);
	U32 GetPlayListLen();

	int WaitListStop();

	int SuspendPlayList();
	int ResumePlayList();

public:
	CAlsaPlayCtrl();
	virtual ~CAlsaPlayCtrl();

	int SetPlayMode(U32 dwMode);
	int PlayCurrent();
	int PauseCurrent();
	int ResumeCurrent();
	int StopCurrent();
	int EndCurrent();
	int GotoNext();

	int AddToList(const char* chFileName);
	int InitPlayPos();

	int PlayList();
	int StopList();

	int PlayListImpl();
	int PlayOpenDevice();
};


#endif /* __ALSA_PLAY_CTRL_F_H__ */
