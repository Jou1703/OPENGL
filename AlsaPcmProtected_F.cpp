/*
 * AlsaPcmProtected.cpp
 *
 *  Created on: Jul 9, 2012
 *      Author: lucid
 */

#include "CommLib_F.h"

#include "AlsaPcm_F.h"

int CAlsaPcm::AttachToWaveData(CAlsaWaveData* pWaveData)
{
	//DetachWaveData(pWaveData);

	m_pData				= pWaveData->GetWaveDataAddr();
	m_pPausePtr			= m_pData;
	m_dwDataLen			= pWaveData->m_WFH_DATA.dwDataLen;
	m_dwChannels		= pWaveData->m_WFH_FMT.wChannel;
	m_dwSampleRate		= pWaveData->m_WFH_FMT.dwSampleRate;
	m_dwTransRate		= pWaveData->m_WFH_FMT.dwBytesPerSecond;
	m_dwBitsPerSample	= pWaveData->m_WFH_FMT.wBitsPerSample;
	m_dwBitsPerFrame	= m_dwBitsPerSample * m_dwChannels;
	m_dwWholeFrames		= m_dwDataLen * 8 / m_dwBitsPerFrame;
	m_dwWholeTimes		= ((float)(m_dwDataLen)) / m_dwTransRate * 1000;
	m_sFileName			= pWaveData->m_sFileName;

	SoundDeviceSet();
	SetCurrentPlayFrame(0);

	return RETURN_SUCCESS;
}

int CAlsaPcm::DetachWaveData(CAlsaWaveData* pWaveData)
{
	return RETURN_SUCCESS;
}

//static char *device = "plughw:1,8"; 


int CAlsaPcm::SoundDeviceOpen()
{
	const char* chDeviceName = "default";
	int err=0;
	
	if ((err = snd_pcm_open(&m_hPCM, (char*)chDeviceName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) 
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		return 0;
	}
	/*
	if ((err = snd_pcm_open(&m_hPCM,chDeviceName, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		printf("[ALSA]Playback open error: %d %s\n", err,snd_strerror(err));
		//DBG_PRINT("Error snd_pcm_open [%s]\n", chDeviceName);
		return RETURN_ERROR_F;
	}
	*/
	return RETURN_SUCCESS;
}

int CAlsaPcm::SoundDeviceClose()
{
	if (m_hPCM)
	{
		Pause();
		snd_pcm_close(m_hPCM);
		m_hPCM = 0;
	}
	return RETURN_SUCCESS;
}

snd_pcm_format_t CAlsaPcm::GetFormatFromSampleLen(int sample_length)
{
	switch (sample_length) {
	case 16:
		return SND_PCM_FORMAT_S16_LE;
		break;
	case 8:
		return SND_PCM_FORMAT_U8;
		break;
	default:
		return SND_PCM_FORMAT_UNKNOWN;
		break;
	}
	return SND_PCM_FORMAT_UNKNOWN;
}

snd_pcm_state_t CAlsaPcm::GetPlayStatus()
{
	snd_pcm_status_t* pPlayStatus;
	snd_pcm_status_alloca(&pPlayStatus);
	snd_pcm_status(m_hPCM,pPlayStatus);
	return snd_pcm_status_get_state(pPlayStatus);
}

snd_pcm_uframes_t CAlsaPcm::GetPeriodSize(const char* chDeviceName,
		unsigned uChannel,unsigned uSampleRate,unsigned uBitsPerSample)
{
	snd_pcm_uframes_t iPeriodSize = 0;

	snd_pcm_t* hPCM;

	if ( snd_pcm_open(&hPCM,chDeviceName,SND_PCM_STREAM_PLAYBACK,0) != 0 )
		return 0;

	snd_pcm_hw_params_t *hwparams;

	snd_pcm_hw_params_alloca(&hwparams);

	if (snd_pcm_hw_params_any(hPCM,hwparams) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_any\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_access(hPCM, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_access\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_format(hPCM, hwparams, GetFormatFromSampleLen(uBitsPerSample)) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_format\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_channels(hPCM, hwparams,uChannel) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_channels\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_rate_near(hPCM, hwparams, &uSampleRate, 0) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_rate_near\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params(hPCM, hwparams) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params(handle, params)\n");return RETURN_ERROR_F;}

	int iDir = 0;
	if (snd_pcm_hw_params_get_period_size(hwparams,&iPeriodSize,&iDir) < 0)
		{	DBG_PRINT("Get period size error!\n");return RETURN_ERROR_F;}

	iPeriodSize += iDir;

	return iPeriodSize;
}

int CAlsaPcm::SoundDeviceSet()
{
	//PrintHeaderInfo();
	//assert(m_hPCM);

	

	snd_pcm_hw_params_t *hwparams;
	snd_pcm_hw_params_alloca(&hwparams);
	
	if (snd_pcm_hw_params_any(m_hPCM,hwparams) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_any\n");return RETURN_ERROR_F;}
	

	if (snd_pcm_hw_params_set_access(m_hPCM, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_access\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_format(m_hPCM, hwparams, GetFormatFromSampleLen(m_dwBitsPerSample)) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_format\n");return RETURN_ERROR_F;}

	if (snd_pcm_hw_params_set_channels(m_hPCM, hwparams,m_dwChannels) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_channels\n");return RETURN_ERROR_F;}

	unsigned int exact_rate = m_dwSampleRate;
	if (snd_pcm_hw_params_set_rate_near(m_hPCM, hwparams, &exact_rate, 0) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params_set_rate_near\n");return RETURN_ERROR_F;}

	if (m_dwSampleRate != exact_rate)
		{	DBG_PRINT("SampleRate %u not support,we use %u\n",m_dwSampleRate,exact_rate);}


	if (snd_pcm_hw_params((snd_pcm_t*)m_hPCM, hwparams) < 0)
		{	DBG_PRINT("Error snd_pcm_hw_params(handle, params)\n");return RETURN_ERROR_F;}

	int iDir = 0;
	if (snd_pcm_hw_params_get_period_size(hwparams,&m_uChunkFrames,&iDir) < 0)
		{	DBG_PRINT("Get period size error!\n");return RETURN_ERROR_F;}

	m_uChunkFrames += iDir;

//	unsigned int iPeriod;
//	int iDir;
//	int iResult;
//	iResult = snd_pcm_hw_params_get_period_time(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("PeriodTime is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_period_time_min(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("PeriodTime is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_period_time_max(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("PeriodTime is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//
//
//	iResult = snd_pcm_hw_params_get_buffer_time(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("buffer_time is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_buffer_time_min(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("buffer_time is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_buffer_time_max(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("buffer_time is %d us,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//
//
//	iResult = snd_pcm_hw_params_get_periods(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("period is %d,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_periods_min(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("period is %d,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_periods_max(hwparams,&iPeriod,&iDir);
//	DBG_PRINT("period is %d,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//
//
//	iResult = snd_pcm_hw_params_get_period_size(hwparams, (snd_pcm_uframes_t*)&iPeriod, &iDir);
//	DBG_PRINT("period_size is %d Frames,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_period_size_min(hwparams, (snd_pcm_uframes_t*)&iPeriod, &iDir);
//	DBG_PRINT("period_size is %d Frames,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//	iResult = snd_pcm_hw_params_get_period_size_max(hwparams, (snd_pcm_uframes_t*)&iPeriod, &iDir);
//	DBG_PRINT("period_size is %d Frames,near %d,iRet = %d,\n",iPeriod,iDir,iResult);
//
//	int iFifoSize = snd_pcm_hw_params_get_fifo_size (hwparams) ;
//	DBG_PRINT("FifoSize = %d\n",iFifoSize);




	return RETURN_SUCCESS;
}

int CAlsaPcm::SoundDeviceSetWithChunk()
{
	return RETURN_SUCCESS;
}


ssize_t CAlsaPcm::PlayPcmData(const void* pData,U32 dwPass,U32 dwFrame)
{
	if (0 == dwFrame)	//if dwFrame == 0,it maybe some error or it comes to the end of data
		return 0;		//directly return 0 Frames had play,avoid to assess pData(out of range)

	ssize_t iRet = 0;
	ssize_t dwWrFrames = 0;
	const U8 *data = (const U8*)pData;
	U32 dwFramePer = 1024000;
	U32 pos = 0;

	//data+=1024000;
	
	while (dwFrame > 0) {

		//DBG_PRINT("hPCM = %08x,dataPtr = %08x,Frames=%d\n",U32(m_hPCM),U32(pData),dwFrame);
		if (dwFramePer>dwFrame)
			dwFramePer = dwFrame;
		iRet = snd_pcm_writei(m_hPCM, data, dwFramePer);
		if ( (iRet == -EAGAIN) ) {
			//DBG_PRINT("+++++++++++++++++++++++wait start!\n");
			snd_pcm_wait(m_hPCM, 1000);
			//DBG_PRINT("-----------------------wait end!\n");
		}

		else if (iRet == -EPIPE) {
			snd_pcm_prepare(m_hPCM);
			//DBG_PRINT("<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
		}

		else if (iRet == -ESTRPIPE) {
			//DBG_PRINT("<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>\n");
		}

		else if (iRet == -EBADFD) {
			DBG_PRINT("writei return BAD FILE DEVICE\n");
			break;	//normal snd_pcm_drop break the snd_pcm_writei
		}

		else if (iRet < 0) {
			DBG_PRINT("Error snd_pcm_writei: iRet=%d [%s]\n",(int)iRet, snd_strerror(iRet));
			break;
		}
		
		if (iRet > 0) {
			dwFrame -= iRet;
			data += m_dwBitsPerFrame / BITS_PER_BYTE * iRet;
			dwWrFrames += iRet;
			pos+=m_dwBitsPerFrame / BITS_PER_BYTE * iRet;
			//DBG_PRINT("writei ONCE iRet = %d\n",iRet);
		}
		//DBG_PRINT(" iRet:  %d\n",pos);
		//DBG_PRINT(" Played data:  %d\n",dwWrFrames);
		//break;
	}
	return dwWrFrames;
}
