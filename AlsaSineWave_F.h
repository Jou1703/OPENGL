/*
 * AlsaSineWave_F.h
 *
 *  Created on: Jul 31, 2012
 *      Author: lucid
 */

#ifndef __ALSA_SINE_WAVE_F_H__
#define __ALSA_SINE_WAVE_F_H__

#include <vector>
using std::vector;

#include <alsa/asoundlib.h>

#include "AlsaWaveData_F.h"

class CAlsaSineWave : public CAlsaWaveData
{
protected:
	void GenerateSine(const snd_pcm_channel_area_t *areas,
			snd_pcm_uframes_t offset,
			int count, double *_phase,
			double freq,
			int channels,
			int rate,
			snd_pcm_format_t format);

	int m_iChannel;
	int m_iBitsPerSample;
	int m_iSampleRate;
	snd_pcm_format_t		m_WaveFormat;


	typedef vector<snd_pcm_channel_area_t> T_vtChannelArea;
	T_vtChannelArea	m_vtChannelAreas;

public:
	void NewSineWaveData(double freq,int iPeriodSize,double& phase);
	void DeleteSineWaveData();

	CAlsaSineWave();
	virtual ~CAlsaSineWave();
};

#include "AlsaPcm_F.h"

class CAlsaSinePcm : public CAlsaPcm
{
protected:
	int m_bPlayLoop;
	double m_fFreq;

public:
	CAlsaSinePcm();
	~CAlsaSinePcm();

	void PlayFreq(double fFreq);
	void PlayFreqImpl();
	void StopFreq();
};


#endif /* __ALSA_SINE_WAVE_F_H__ */
