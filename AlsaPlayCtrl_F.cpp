/*
 * AlsaPlayCtrl.cpp
 *
 *  Created on: Jul 10, 2012
 *      Author: lucid
 */

#include "CommLib_F.h"

#include "AlsaPlayCtrl_F.h"


CAlsaPlayCtrl::CAlsaPlayCtrl()
{
	m_iPlayingIndex = -1;
	m_bRepeatLoop = 0;
	m_bIsListPlaying = 0;
	m_bStopList = 0;
}

CAlsaPlayCtrl::~CAlsaPlayCtrl()
{

}


int CAlsaPlayCtrl::SetPlayMode(U32 dwMode)
{
	if(dwMode > PlayListModeMax)
		return RETURN_ERROR_F;

	m_dwPlayMode = dwMode;
	return RETURN_SUCCESS;
}

int CAlsaPlayCtrl::PlayCurrent()
{
	return m_Pcm.Play();
}

int CAlsaPlayCtrl::PauseCurrent()
{
	return m_Pcm.Pause();
}

int CAlsaPlayCtrl::ResumeCurrent()
{
	return m_Pcm.Resume();
}

int CAlsaPlayCtrl::StopCurrent()
{
	return m_Pcm.Stop();
}

int CAlsaPlayCtrl::EndCurrent()
{
	m_Pcm.Stop();
	return m_Pcm.SetFinishEvent();
}

int CAlsaPlayCtrl::GotoNext()
{
	vector<string>::iterator it;

	switch (m_dwPlayMode)
	{
	case PlayListModeNormal:
	case PlayListModeListRepeat:
		m_Pcm.Stop();
		m_Pcm.SetFinishEvent();
		break;
	case PlayListModeSingleRepeat:
	case PlayListModeSingleOnce:
		StopList();
		it = GetCurrentPlayIterator();
		++it;
		if ( it == m_vtPlayList.end() )
		{
			it = m_vtPlayList.begin();
		}
		SetCurrentPlayIterator(it);
		PlayList();
		break;
	}
	return RETURN_SUCCESS;
}

U32 CAlsaPlayCtrl::GetCurrentPlayIndex()
{
	return m_iPlayingIndex;
}

vector<string>::iterator CAlsaPlayCtrl::GetCurrentPlayIterator()
{
	return m_itListPlaying;
}

int CAlsaPlayCtrl::SetCurrentPlayIterator(vector<string>::iterator it)
{
	m_itListPlaying = it;
	return RETURN_SUCCESS;
}

int CAlsaPlayCtrl::SetCurrentPlayIndex(int iIndex)
{
	if ( iIndex < int(m_vtPlayList.size()) )
	{
		m_iPlayingIndex = iIndex;
		return RETURN_SUCCESS;
	}
	else
		return RETURN_ERROR_F;
}

U32 CAlsaPlayCtrl::GetPlayListLen()
{
	return m_vtPlayList.size();
}

int CAlsaPlayCtrl::AddToList(const char* chFileName)
{
	if ( !CFile::IsFileExist(chFileName) )
		return RETURN_ERROR_F;

	m_vtPlayList.push_back(chFileName);
	return RETURN_SUCCESS;
}

int CAlsaPlayCtrl::InitPlayPos()
{
	return SetCurrentPlayIterator(m_vtPlayList.begin());
}

void* PlayListProc(void* pData)
{
	CAlsaPlayCtrl* pThis = (CAlsaPlayCtrl*)pData;
	return (void*)pThis->PlayListImpl();
}

int CAlsaPlayCtrl::WaitListStop()
{
	return (long)m_ThPlayList.Join();
	//return 1;
}

int CAlsaPlayCtrl::StopList()
{
	m_bStopList = 1;
	m_bRepeatLoop = 0;
	EndCurrent();
	return WaitListStop();
}


int CAlsaPlayCtrl::PlayList()
{
	if (RETURN_SUCCESS == m_ThPlayList.IsThreadRunning())
		return RETURN_ERROR_F;

	m_bStopList = 0;
	m_ThPlayList.Create(PlayListProc,this);
	return RETURN_SUCCESS;
}

int CAlsaPlayCtrl::PlayListImpl()
{
	vector<string>::iterator it,itStart,itEnd;

	switch (m_dwPlayMode)
	{
	case PlayListModeNormal:
		m_bRepeatLoop = 0;
		itStart = m_vtPlayList.begin();
		itEnd = m_vtPlayList.end();
		break;
	case PlayListModeListRepeat:
		m_bRepeatLoop = 1;
		itStart = m_vtPlayList.begin();
		itEnd = m_vtPlayList.end();
		break;
	case PlayListModeSingleRepeat:
		m_bRepeatLoop = 1;
		itStart = GetCurrentPlayIterator();
		itEnd = GetCurrentPlayIterator() + 1;
		break;
	case PlayListModeSingleOnce:
		m_bRepeatLoop = 0;
		itStart = GetCurrentPlayIterator();
		itEnd = GetCurrentPlayIterator() + 1;
		break;
	}

	do
	{
		for	(it = itStart; it!=itEnd; ++it)
		{
			m_WaveData[0].LaunchFile( (*it).c_str() );
			m_Pcm.AttachToWaveData(&m_WaveData[0]);
			m_Pcm.RePlay();
			m_Pcm.WaitPlayFinish();

			if ( m_bStopList )
			{
				break;
			}
		}
	}
	while (m_bRepeatLoop);

	m_ThPlayList.DetechThread();
	return RETURN_SUCCESS;
}

int CAlsaPlayCtrl::SuspendPlayList()
{
	m_ThPlayList.SuspendThread();
	return RETURN_SUCCESS;
}
int CAlsaPlayCtrl::ResumePlayList()
{
	PlayListImpl();
	return RETURN_SUCCESS;
}
/*
int CAlsaPlayCtrl::ResumePlayList()
{
	m_ThPlayList.ResumeThread();
	return RETURN_SUCCESS;
}
*/















