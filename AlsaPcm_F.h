/*
 * AlsaPcm.h
 *
 *  Created on: Jul 8, 2012
 *      Author: lucid
 */

#ifndef __ALSA_PCM_H__
#define __ALSA_PCM_H__

#include "CommLib_F.h"
#include <string>
using std::string;

#include <alsa/asoundlib.h>

#include "Thread_F.h"
#include "ThreadEvent_F.h"

#include "AlsaWaveData_F.h"

class CAlsaPcm
{
public:
	enum PlayListMode
	{
		PlayListModeNormal = 0,
		PlayListModeRepeat = 1,
		PlayListModeMax = PlayListModeRepeat
	};

protected:
	const int BITS_PER_BYTE;

public:
	CAlsaPcm();
	virtual ~CAlsaPcm();

protected:
	CThread		m_ThreadPlay;
	CTthreadEvent		m_EventPlayFinish;

	snd_pcm_t*			m_hPCM;
	const U8*			m_pData;
	const U8*			m_pPausePtr;
	U32					m_dwDataLen;
	U32					m_dwChannels;
	U32					m_dwSampleRate;
	U32					m_dwTransRate;
	U32					m_dwBitsPerSample;
	U32					m_dwBitsPerFrame;
	snd_pcm_uframes_t	m_dwRemainFrames;
	snd_pcm_uframes_t	m_dwWholeFrames;
	U32					m_dwWholeTimes;
	string				m_sFileName;

	U32 				m_dwPlayPerTime;
U32 m_dwPassPos;

	snd_pcm_uframes_t	m_uChunkFrames;

	volatile int		m_bPlayBusy;
	volatile int		m_bPlayLoop;

	int SoundDeviceSet();
	int CalcDataFrames();

	int SoundDeviceSetWithChunk();

	snd_pcm_format_t GetFormatFromSampleLen(int iSampleLen);
	snd_pcm_state_t GetPlayStatus();
	ssize_t PlayPcmData(const void* pData,U32 dwPass,U32 dwFrame);

public:
	snd_pcm_uframes_t GetPeriodSize(const char* chDeviceName,
			unsigned uChannel,unsigned uSampleRate,unsigned uBitsPerSample);

	int PrintHeaderInfo();

	int AttachToWaveData(CAlsaWaveData* pWaveData);
	int DetachWaveData(CAlsaWaveData* pWaveData);
	int SoundDeviceOpen();
	int SoundDeviceClose();

	int SetCurrentPlayFrame(U32 dwFrame);
	int SetCurrentPlayTime(U32 dwMilliSecond);
	int Play();
	int PlayCounts(U32 dwCount);
	int RePlay();
	int ForceRePlay();
	int WaitPlayFinish();
	int SetFinishEvent();
	int PlayImpl();
	int Stop();
	int Pause();
	int Resume();
	void* JoinPlayEnd();

	int DbgPrintState();
};


#endif /* __ALSA_PCM_H__ */
