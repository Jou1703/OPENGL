/*
 * AlsaPcm.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: lucid
 */

#include "CommLib_F.h"

#include "AlsaPcm_F.h"

CAlsaPcm::CAlsaPcm()
	:BITS_PER_BYTE(8)
{
	m_hPCM = 0;
	m_pData = 0;
	m_pPausePtr = 0;
	m_dwDataLen = 0;
	m_dwChannels = 0;
	m_dwSampleRate = 0;
	m_dwTransRate = 0;
	m_dwBitsPerSample = 0;
	m_dwBitsPerFrame = 0;
	m_dwRemainFrames = 0;
	m_dwWholeFrames = 0;
	m_dwWholeTimes = 0;
	m_bPlayBusy = 0;
	m_bPlayLoop = 1;

	
	SoundDeviceOpen();
}

CAlsaPcm::~CAlsaPcm()
{
	SoundDeviceClose();
}


int CAlsaPcm::PrintHeaderInfo()
{
	DBG_PRINT("DataLen Bytes = %u\n",		m_dwDataLen);
	DBG_PRINT("TransRate     = %u\n",		m_dwTransRate);
	DBG_PRINT("Times         = %u\n",		m_dwWholeTimes);
	DBG_PRINT("Channels      = %u\n",		m_dwChannels);
	DBG_PRINT("BitsPerSample = %u\n",		m_dwBitsPerSample);
	DBG_PRINT("BitsPerFrame  = %u\n",		m_dwBitsPerFrame);
	DBG_PRINT("SampleRate    = %u\n",		m_dwSampleRate);
	DBG_PRINT("WholeFrames   = %u\n",		m_dwWholeFrames);
	return RETURN_SUCCESS;
}

static void* PlayThread(void* pData)
{
	CAlsaPcm* pThis = (CAlsaPcm*)pData;
	
	pThis->PlayImpl();
	//pthread_detach( (pthread_t)pthread_self() );
	return 0;
}

# if (1)

int CAlsaPcm::PlayImpl()
{
	m_bPlayBusy = 1;
	m_EventPlayFinish.ResetEvent();
	snd_pcm_prepare(m_hPCM);
	//DBG_PRINT("++++++++++++++++++++++\n\n");
	//DBG_PRINT("last=%lu\n",m_dwRemainFrames);
	U32 dwPlayedFrames = PlayPcmData(m_pPausePtr,0,m_dwRemainFrames);

	SetCurrentPlayFrame(dwPlayedFrames+m_dwWholeFrames - m_dwRemainFrames);
	//SetCurrentPlayFrame(dwPlayedFrames);
	//DBG_PRINT("last=%lu dwPlayedFrames%lu\n",m_dwRemainFrames,dwPlayedFrames);
	//DBG_PRINT("++++++++++++++++++++++\n\n");
	
	m_bPlayBusy = 0;
	if (0 == m_dwRemainFrames)
	{
		m_EventPlayFinish.SetEvent();
	}
	m_ThreadPlay.DetechThread();
	return RETURN_SUCCESS;
}

# else

int CAlsaPcm::PlayImpl()
{
	m_bPlayBusy = 1;
	m_EventPlayFinish.ResetEvent();

	snd_pcm_uframes_t uWredFrames = 0;
	snd_pcm_uframes_t uWrFrames = 0;

	snd_pcm_prepare(m_hPCM);

	m_bPlayLoop = 1;

	do
	{
		uWrFrames = GetMinVal(m_dwRemainFrames,m_uChunkFrames);
		//DBG_PRINT("-------------RemainFrames = %lu\n",uWredFrames);
		uWredFrames = PlayPcmData(m_pPausePtr,uWrFrames);
		
		if (uWrFrames != uWredFrames)
		{
			DBG_PRINT("OUT BREAK %s\n",m_sFileName.c_str());
			m_bPlayLoop = 0;
		}
		SetCurrentPlayFrame(m_dwWholeFrames - m_dwRemainFrames + uWredFrames);
		
	}
	while( m_bPlayLoop && (m_dwRemainFrames>0) );

	m_bPlayBusy = 0;
	if (0 == m_dwRemainFrames)
	{
		m_EventPlayFinish.SetEvent();
	}
	return RETURN_SUCCESS;
}

# endif

int CAlsaPcm::Play()
{
	if(m_bPlayBusy)
	{
		return RETURN_ERROR_F;
	}
	
	m_ThreadPlay.Create(PlayThread,this);
	return RETURN_SUCCESS;
}

int CAlsaPcm::PlayCounts(U32 dwCount)
{
	while(dwCount--)
	{
		Play();
		JoinPlayEnd();
	}
	return RETURN_SUCCESS;
}

int CAlsaPcm::RePlay()
{
	SetCurrentPlayFrame(0);
	return Play();
}

int CAlsaPcm::ForceRePlay()
{
	Stop();
	return RePlay();
}

int CAlsaPcm::SetFinishEvent()
{
	m_EventPlayFinish.SetEvent();
	return RETURN_SUCCESS;
}

int CAlsaPcm::WaitPlayFinish()
{
	m_EventPlayFinish.WaitEvent();
	return RETURN_SUCCESS;
}

int CAlsaPcm::Stop()
{
	Pause();
	SetCurrentPlayFrame(0);
	return RETURN_SUCCESS;
}

int CAlsaPcm::Pause()
{
	snd_pcm_drop(m_hPCM);
	//snd_pcm_drain(m_hPCM);
	m_bPlayLoop = 0;
	m_ThreadPlay.Join();
	return RETURN_SUCCESS;
}

int CAlsaPcm::Resume()
{
	return Play();
	//snd_pcm_prepare(m_hPCM);
	return RETURN_SUCCESS;
}

void* CAlsaPcm::JoinPlayEnd()
{
	return m_ThreadPlay.Join();
}

int CAlsaPcm::SetCurrentPlayFrame(U32 dwFrame)
{
	if(dwFrame > m_dwWholeFrames)
		return RETURN_ERROR_F;
	//DBG_PRINT("SetCurrentPlayFrame dwFrame = %d\n",dwFrame);
	m_dwRemainFrames = m_dwWholeFrames - dwFrame;	
	//DBG_PRINT("SetCurrentPlayFrame dwFrame = %d\n",dwFrame);
	m_pPausePtr = m_pData + m_dwBitsPerFrame / BITS_PER_BYTE * dwFrame;
	return RETURN_SUCCESS;
}



int CAlsaPcm::SetCurrentPlayTime(U32 dwMilliSecond)
{
	U32 dwFrame = m_dwSampleRate * dwMilliSecond / 1000;
	return SetCurrentPlayFrame(dwFrame);
}

int CAlsaPcm::DbgPrintState()
{
	snd_pcm_state_t state = GetPlayStatus();
	return DBG_PRINT("pcm state = %d\n",int(state));
}





























