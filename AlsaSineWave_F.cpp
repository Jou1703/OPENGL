/*
 * AlsaSineWave_F.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lucid
 */
#include "CommLib_F.h"

#include "AlsaSineWave_F.h"
#include "AlsaPcm_F.h"

#include <cmath>


class CAlsaPcm_Ex : public CAlsaPcm
{
public:
	int AttachSineWaveData(CAlsaSineWave& sineWave)
	{
		return RETURN_SUCCESS;
	}
};

CAlsaSineWave::CAlsaSineWave()
	:CAlsaWaveData()
{
	m_iChannel = 2;
	m_iBitsPerSample = 16;
	m_iSampleRate = 44100;
	m_WaveFormat = SND_PCM_FORMAT_S16;
}

CAlsaSineWave::~CAlsaSineWave()
{

}

void CAlsaSineWave::NewSineWaveData(double freq,int iPeriodSize,double& phase)
{
	m_vtChannelAreas.resize(m_iChannel);
	m_vtData.resize( iPeriodSize * m_iChannel * m_iBitsPerSample / 8 );

	int i = 0;
	for(i = 0; i < m_iChannel; ++i )
	{
		m_vtChannelAreas[i].addr = &m_vtData[0];
		m_vtChannelAreas[i].first = i * m_iBitsPerSample;
		m_vtChannelAreas[i].step = m_iChannel * m_iBitsPerSample;
	}

	GenerateSine(&m_vtChannelAreas[0],
			0,
			iPeriodSize,
			&phase,
			freq,
			m_iChannel,
			m_iSampleRate,
			m_WaveFormat);
}

void CAlsaSineWave::DeleteSineWaveData()
{

}

/*
int freq = 300;
int channels = 1;
int rate = 8000;
snd_pcm_format_t format = SND_PCM_FORMAT_U8;
*/

void CAlsaSineWave::GenerateSine(const snd_pcm_channel_area_t *areas,
						snd_pcm_uframes_t offset,
						int count, double *_phase,
						double freq,
						int channels,
						int rate,
						snd_pcm_format_t format)
{
	  static double max_phase = 2. * M_PI;
	  double phase = *_phase;
	  double step = max_phase*freq/(double)rate;
	  unsigned char *samples[channels];
	  int steps[channels];
	  int chn;
	  int format_bits = snd_pcm_format_width(format);
	  unsigned int maxval = (1 << (format_bits - 1)) - 1;
	  int bps = format_bits / 8;  /* bytes per sample */
	  int phys_bps = snd_pcm_format_physical_width(format) / 8;
	  int big_endian = snd_pcm_format_big_endian(format) == 1;
	  int to_unsigned = snd_pcm_format_unsigned(format) == 1;
	  int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
					  format == SND_PCM_FORMAT_FLOAT_BE);

	  /* verify and prepare the contents of areas */
	  for (chn = 0; chn < channels; chn++) {
			  if ((areas[chn].first % 8) != 0) {
					  printf("areas[%i].first == %i, aborting...\n", chn, areas[chn].first);
					  exit(EXIT_FAILURE);
			  }
			  samples[chn] = /*(signed short *)*/(((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
			  if ((areas[chn].step % 16) != 0) {
					  printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
					  exit(EXIT_FAILURE);
			  }
			  steps[chn] = areas[chn].step / 8;
			  samples[chn] += offset * steps[chn];
	  }
	  /* fill the channel areas */
	  while (count-- > 0) {
			  union {
					  float f;
					  int i;
			  } fval;
			  int res, i;
			  if (is_float) {
					  fval.f = sin(phase) * maxval;
					  res = fval.i;
			  } else
					  res = sin(phase) * maxval;
			  if (to_unsigned)
					  res ^= 1U << (format_bits - 1);
			  for (chn = 0; chn < channels; chn++) {
					  /* Generate data in native endian format */
					  if (big_endian) {
							  for (i = 0; i < bps; i++)
									  *(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
					  } else {
							  for (i = 0; i < bps; i++)
									  *(samples[chn] + i) = (res >>  i * 8) & 0xff;
					  }
					  samples[chn] += steps[chn];
			  }
			  phase += step;
			  if (phase >= max_phase)
					  phase -= max_phase;
	  }
	  *_phase = phase;
}


CAlsaSinePcm::CAlsaSinePcm()
{
	m_bPlayLoop = 1;
}

CAlsaSinePcm::~CAlsaSinePcm()
{

}

void generate_sine(const snd_pcm_channel_area_t *areas,
						snd_pcm_uframes_t offset,
						int count, double *_phase,
						double freq,
						int channels,
						int rate,
						snd_pcm_format_t format);

void* PlayFreqProc(void* pData)
{
	CAlsaSinePcm* pThis = (CAlsaSinePcm*) pData;
	pThis->PlayFreqImpl();
	pthread_detach( pthread_self() );
	return 0;

}

void CAlsaSinePcm::PlayFreq(double fFreq)
{
	m_fFreq = fFreq;
	CThread th;
	th.Create(PlayFreqProc,this);
}

void CAlsaSinePcm::PlayFreqImpl()
{
	m_dwChannels = 1;
	m_dwBitsPerSample = 16;
	m_dwSampleRate = 44100;

	SoundDeviceSet();

	int period_size = m_uChunkFrames;

	snd_pcm_t *handle = m_hPCM;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16;

	signed short *samples;
	samples = new signed short[
	                           (period_size
							   * m_dwChannels
							   * snd_pcm_format_physical_width(format))
							   / 8
							   ];

	snd_pcm_channel_area_t* areas = new snd_pcm_channel_area_t[m_dwChannels];

	for (unsigned chn = 0; chn < m_dwChannels; chn++) {
		  areas[chn].addr = samples;
		  areas[chn].first = chn * snd_pcm_format_physical_width(format);
		  areas[chn].step = m_dwChannels * snd_pcm_format_physical_width(format);
	}

	double phase = 0;
	signed short *ptr;
	int iRet, cptr;

	m_bPlayLoop = 1;

	while (m_bPlayLoop) {
		generate_sine(areas, 0, period_size, &phase,
				m_fFreq,m_dwChannels,m_dwSampleRate,format);
		ptr = samples;
		cptr = period_size;
		while (cptr > 0) {
			iRet = snd_pcm_writei(handle, ptr, cptr);
			if ( (iRet == -EAGAIN) ) {
				//DBG_PRINT("+++++++++++++++++++++++wait start!\n");
				snd_pcm_wait(m_hPCM, 1000);
				//DBG_PRINT("-----------------------wait end!\n");
			}

			else if (iRet == -EPIPE) {
				snd_pcm_prepare(m_hPCM);
				DBG_PRINT("<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
			}

			else if (iRet == -ESTRPIPE) {
				DBG_PRINT("<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>\n");
			}

			else if (iRet == -EBADFD) {
				DBG_PRINT("writei return BAD FILE DEVICE\n");
				break;	//normal snd_pcm_drop break the snd_pcm_writei
			}

			else if (iRet < 0) {
				DBG_PRINT("Error snd_pcm_writei: iRet=%d [%s]\n",iRet, snd_strerror(iRet));
				break;
			}

			ptr += iRet * m_dwChannels;
			cptr -= iRet;
		}
	}
}

void generate_sine(const snd_pcm_channel_area_t *areas,
						snd_pcm_uframes_t offset,
						int count, double *_phase,
						double freq,
						int channels,
						int rate,
						snd_pcm_format_t format)
{
	  static double max_phase = 2. * M_PI;
	  double phase = *_phase;
	  double step = max_phase*freq/(double)rate;
	  unsigned char *samples[channels];
	  int steps[channels];
	  int chn;
	  int format_bits = snd_pcm_format_width(format);
	  unsigned int maxval = (1 << (format_bits - 1)) - 1;
	  int bps = format_bits / 8;  /* bytes per sample */
	  int phys_bps = snd_pcm_format_physical_width(format) / 8;
	  int big_endian = snd_pcm_format_big_endian(format) == 1;
	  int to_unsigned = snd_pcm_format_unsigned(format) == 1;
	  int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
					  format == SND_PCM_FORMAT_FLOAT_BE);

	  /* verify and prepare the contents of areas */
	  for (chn = 0; chn < channels; chn++) {
			  if ((areas[chn].first % 8) != 0) {
					  printf("areas[%i].first == %i, aborting...\n", chn, areas[chn].first);
					  exit(EXIT_FAILURE);
			  }
			  samples[chn] = /*(signed short *)*/(((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
			  if ((areas[chn].step % 16) != 0) {
					  printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
					  exit(EXIT_FAILURE);
			  }
			  steps[chn] = areas[chn].step / 8;
			  samples[chn] += offset * steps[chn];
	  }
	  /* fill the channel areas */
	  while (count-- > 0) {
			  union {
					  float f;
					  int i;
			  } fval;
			  int res, i;
			  if (is_float) {
					  fval.f = sin(phase) * maxval;
					  res = fval.i;
			  } else
					  res = sin(phase) * maxval;
			  if (to_unsigned)
					  res ^= 1U << (format_bits - 1);
			  for (chn = 0; chn < channels; chn++) {
					  /* Generate data in native endian format */
					  if (big_endian) {
							  for (i = 0; i < bps; i++)
									  *(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
					  } else {
							  for (i = 0; i < bps; i++)
									  *(samples[chn] + i) = (res >>  i * 8) & 0xff;
					  }
					  samples[chn] += steps[chn];
			  }
			  phase += step;
			  if (phase >= max_phase)
					  phase -= max_phase;
	  }
	  *_phase = phase;
}

void CAlsaSinePcm::StopFreq()
{
	m_bPlayLoop = 0;
}








