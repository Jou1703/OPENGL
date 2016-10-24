/*
 * AlsaWaveData.h
 *
 *  Created on: Jul 8, 2012
 *      Author: lucid
 */

#ifndef __ALSA_WAVE_DATA_H__
#define __ALSA_WAVE_DATA_H__

#include <vector>
using std::vector;
#include <string>
using std::string;

#include <list>
using std::list;

#include "File_F.h"

template<typename T>
class vector_ex : public vector<T>
{
public:
	~vector_ex()
	{
		DBG_PRINT("dtor vector\n");
		this->resize(1);
		DBG_PRINT("vector dtor end\n");
	}
};

class CAlsaWaveData
{
	struct WFH_RIFF_CHUNK
	{
		char		chRiff[4];
		U32			dwFileLen;
		char		chWave[4];
	};

	struct WFH_FMT_CHUNK
	{
		char		chFormat[4];
		U32			dwFmtChunkLen;
		U16			wFormat;
		U16			wChannel;
		U32			dwSampleRate;
		U32			dwBytesPerSecond;
		U16			wDataBlockAlign;
		U16			wBitsPerSample;
	};

	struct WFH_FACT_CHUNK
	{
		char		chFact[4];
		U32			dwFactChunkLen;
		vector<U8>	vtChunkData;
	};

	struct WFH_DATA_CHUNK
	{
		char		chData[4];
		U32			dwDataLen;
	};

public:
	CFile				m_FileWave;
	vector<U8>			m_vtData;
	const U8 *			m_pWaveData;
	int					m_bIsWaveDataAlloc;
	int					m_bHasFactChunk;

public:
	WFH_RIFF_CHUNK		m_WFH_RIFF;
	WFH_FMT_CHUNK		m_WFH_FMT;
	WFH_FACT_CHUNK		m_WFH_FACT;
	WFH_DATA_CHUNK		m_WFH_DATA;

	string				m_sFileName;

protected:
	int ReadRiffChunk();
	int ReadFmtChunk(int iPos);
	int ReadFactChunk(int iPos);
	int ReadDataChunk(int iPos);
	int ReadWaveData(int iPos);

	int PrintHeaderInfo();
public:

	CAlsaWaveData();
	virtual ~CAlsaWaveData();

	int SaveDataToFile(const char* sFileName);

	int LaunchData(
			U16 wChannel,
			U32 dwSampleRate,
			U16 wBitsPerSample,
			const U8* pWaveData,
			U32 dwWaveDataLen,
			int bIsWaveDataCopy);

	int IsWaveDataLaunched();
	int IsFileLaunched(const char* sFileName);
	int LaunchFile(const char* chFileName);
	int DeleteData();
	const U8* GetWaveDataAddr();
};


#endif /* __ALSA_WAVE_DATA_H__ */
