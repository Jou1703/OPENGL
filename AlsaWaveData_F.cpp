/*
 * AlsaWaveData.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: lucid
 */

#include "CommLib_F.h"

#include <string.h>

#include "AlsaWaveData_F.h"


CAlsaWaveData::CAlsaWaveData()
{
	m_bIsWaveDataAlloc = 0;
	m_pWaveData = 0;
	m_bHasFactChunk = 0;
}

CAlsaWaveData::~CAlsaWaveData()
{
//	if (m_bIsWaveDataAlloc)
//	{
//		delete [] m_WFH_DATA.pWaveData;
//	}
}

int CAlsaWaveData::PrintHeaderInfo()
{
//	DBG_PRINT("DataLen Bytes = %lu\n",		m_dwDataLen);
//	DBG_PRINT("TransRate     = %lu\n",		m_dwTransRate);
//	DBG_PRINT("Times         = %lu\n",		m_dwWholeTimes);
//	DBG_PRINT("Channels      = %lu\n",		m_dwChannels);
//	DBG_PRINT("BitsPerSample = %lu\n",		m_dwBitsPerSample);
//	DBG_PRINT("BitsPerFrame  = %lu\n",		m_dwBitsPerFrame);
//	DBG_PRINT("SampleRate    = %lu\n",		m_dwSampleRate);
//	DBG_PRINT("WholeFrames   = %lu\n",		m_dwWholeFrames);
	return RETURN_SUCCESS;
}

int CAlsaWaveData::ReadRiffChunk()
{
	if ( m_FileWave.Read(0,&m_WFH_RIFF,sizeof(m_WFH_RIFF)) != sizeof(m_WFH_RIFF) )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}
	if ( 0 != memcmp("RIFF",m_WFH_RIFF.chRiff,4) )
	{
		DBG_PRINT("This is not a RIFF file!\n");
		return -1;
	}
	
	
	U32 dwFileLen = m_FileWave.GetLength();
	if ( dwFileLen != m_WFH_RIFF.dwFileLen )	//if file len no match RIFF len
	{
		
		//DBG_PRINT("File len not match,file maybe destroy!\n");
		//DBG_PRINT("Get File Len = %u,read file len = %u\n",dwFileLen,m_WFH_RIFF.dwFileLen + 8);
		//return -1;
	}
	return m_FileWave.GetCurrentPos();
}

int CAlsaWaveData::ReadFmtChunk(int iPos)
{
	if (iPos < 0)
		return -1;

	if ( m_FileWave.Read(iPos,&m_WFH_FMT,sizeof(m_WFH_FMT)) != sizeof(m_WFH_FMT) )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}
	if ( 0 != memcmp("fmt ",m_WFH_FMT.chFormat,4) )
	{
		DBG_PRINT("Read fmt chunk error!\n");
		return -1;
	}
	return iPos + 8 + m_WFH_FMT.dwFmtChunkLen;
}

int CAlsaWaveData::ReadFactChunk(int iPos)
{
	if ( iPos < 0 )
		return -1;

	if ( m_FileWave.Read(iPos,m_WFH_FACT.chFact,4) != 4 )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}
	if (0 != memcmp("fact",m_WFH_FACT.chFact,4) )
	{
		return iPos;	//if this is not fact chunk,no move the pos,check data chunk at the same pos
	}
	if ( m_FileWave.Read(iPos+4,&m_WFH_FACT.dwFactChunkLen,4) != 4 )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}
	m_WFH_FACT.vtChunkData.resize(m_WFH_FACT.dwFactChunkLen);
	if ( m_FileWave.Read(iPos+8,&m_WFH_FACT.vtChunkData[0],m_WFH_FACT.dwFactChunkLen)
			!= ssize_t(m_WFH_FACT.dwFactChunkLen) )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}

	m_bHasFactChunk = 1;

	return iPos + 8 + m_WFH_FACT.dwFactChunkLen;
}

int CAlsaWaveData::ReadDataChunk(int iPos)
{
	if ( iPos < 0 )
		return -1;

	if ( m_FileWave.Read(iPos,&m_WFH_DATA,sizeof(m_WFH_DATA)) != sizeof(m_WFH_DATA) )
	{
		DBG_PRINT("call fread have something wrong!\n");
		return -1;
	}
	if ( 0 != memcmp("data",m_WFH_DATA.chData,4) )
	{
		DBG_PRINT("Read data chunk error!\n");
		return -1;
	}
	return m_FileWave.GetCurrentPos();
}


int CAlsaWaveData::ReadWaveData(int iPos)
{
	if ( iPos < 0 )
		return RETURN_ERROR_F;

	m_vtData.resize(m_WFH_DATA.dwDataLen);
	if ( m_vtData.size() != m_WFH_DATA.dwDataLen )
	{
		DBG_PRINT("Error malloc data buffer.\n");
		return RETURN_ERROR_F;
	}

	m_pWaveData = &m_vtData[0];

	if ( m_FileWave.Read(iPos,&m_vtData[0],m_vtData.size())
			!= ssize_t(m_vtData.size())
		)
	{
		DBG_PRINT("Error read wave file data.\n");
		return RETURN_ERROR_F;
	}

	return RETURN_SUCCESS;
}


int CAlsaWaveData::LaunchData(
		U16 wChannel,
		U32 dwSampleRate,
		U16 wBitsPerSample,
		const U8* pWaveData,
		U32 dwWaveDataLen,
		int bIsWaveDataCopy)
{
//Setup RIFF chunk
	memcpy(m_WFH_RIFF.chRiff,"RIFF",4);
	memcpy(m_WFH_RIFF.chWave,"WAVE",4);
//Setup FMT chunk
	memcpy(m_WFH_FMT.chFormat,"fmt ",4);
	m_WFH_FMT.dwFmtChunkLen = sizeof(m_WFH_FMT) - 8;
	m_WFH_FMT.wFormat = 1;
	m_WFH_FMT.wChannel = wChannel;
	m_WFH_FMT.dwSampleRate = dwSampleRate;
	m_WFH_FMT.dwBytesPerSecond = dwSampleRate * wChannel * wBitsPerSample / 8;
	m_WFH_FMT.wDataBlockAlign = wChannel * wBitsPerSample / 8;
	m_WFH_FMT.wBitsPerSample = wBitsPerSample;
//Setup FACT chunk
	memcpy(m_WFH_FACT.chFact,"fact",4);
	m_WFH_FACT.dwFactChunkLen = 4;
	m_WFH_FACT.vtChunkData.resize(4);
	memcpy(&m_WFH_FACT.vtChunkData[0],"BAO ",4);
	m_bHasFactChunk = 1;
//Setup DATA chunk
	memcpy(m_WFH_DATA.chData,"data",4);
	m_WFH_DATA.dwDataLen = dwWaveDataLen;

	DBG_PRINT("dwWaveData: %u\n",dwWaveDataLen);

// 4 (RIFF's WAVE) + sizeof(m_WFH_FMT) + 8 (fact & dwFactLen) + factLen + 8(data & dwDataLen) + dataLen
	m_WFH_RIFF.dwFileLen = 4 + sizeof(m_WFH_FMT)
			+ (8 + m_WFH_FACT.dwFactChunkLen) * m_bHasFactChunk
			+ 8 + dwWaveDataLen;

	if (bIsWaveDataCopy)
	{
		m_bIsWaveDataAlloc = 1;
		m_vtData.resize(dwWaveDataLen);
		memcpy((void*)&m_vtData[0],pWaveData,dwWaveDataLen);
		m_pWaveData = &m_vtData[0];
	}
	else
	{
		m_pWaveData = pWaveData;
	}

	return RETURN_SUCCESS;
}

int CAlsaWaveData::IsFileLaunched(const char* sFileName)
{
	if ( m_sFileName == sFileName )
		return RETURN_SUCCESS;
	else
		return RETURN_ERROR_F;
}

int CAlsaWaveData::IsWaveDataLaunched()
{
	if (   (m_sFileName != "")
		&& (m_vtData.size() != 0)  )
		return RETURN_SUCCESS;
	else
		return RETURN_ERROR_F;
}

int CAlsaWaveData::LaunchFile(const char* chFileName)
{
	if (m_FileWave.OpenExist(chFileName,CFile::FILE_ACCESS_FLAG_READ) == 0 )
	{
		DBG_PRINT("Error open [%s]\n",chFileName);
		return RETURN_ERROR_F;
	}

	DeleteData();

	m_bHasFactChunk = 0;

	int iPos = ReadRiffChunk();
	if ( iPos < 0 )
		return RETURN_ERROR_F;

	iPos = ReadFmtChunk(iPos);
	if ( iPos < 0 )
		return RETURN_ERROR_F;

	iPos = ReadFactChunk(iPos);
	if ( iPos < 0 )
		return RETURN_ERROR_F;

	iPos = ReadDataChunk(iPos);
	if ( iPos < 0 )
		return RETURN_ERROR_F;

	int iRet = ReadWaveData(iPos);
	m_FileWave.Close();

	m_sFileName = chFileName;

	//PrintHeaderInfo();

	return iRet;
}

int CAlsaWaveData::SaveDataToFile(const char* sFileName)
{
	CFile m_FileWave;

	if ( RETURN_ERROR_F == m_FileWave.CreateNewAlways(sFileName,CFile::FILE_ACCESS_FLAG_RDWR) )
	{
		DBG_PRINT("Can't create new file [%s]!",sFileName);
		return RETURN_ERROR_F;
	}

	U32 iPos = 0;
	iPos += m_FileWave.Write(iPos,&m_WFH_RIFF,sizeof(m_WFH_RIFF));
	iPos += m_FileWave.Write(iPos,&m_WFH_FMT,m_WFH_FMT.dwFmtChunkLen + 8);//sizeof(m_WFH_FMT));

	if (m_bHasFactChunk)
	{
		iPos += m_FileWave.Write(iPos,&m_WFH_FACT,8);
		iPos += m_FileWave.Write(iPos,&m_WFH_FACT.vtChunkData[0],m_WFH_FACT.dwFactChunkLen);
	}
	iPos += m_FileWave.Write(iPos,&m_WFH_DATA,sizeof(m_WFH_DATA));
	iPos += m_FileWave.Write(iPos,m_pWaveData,m_WFH_DATA.dwDataLen);

	m_FileWave.Close();

	return RETURN_SUCCESS;
}

int CAlsaWaveData::DeleteData()
{
	m_vtData.clear();
	m_sFileName = "";
	memset(&m_WFH_RIFF,0,sizeof(m_WFH_RIFF));
	memset(&m_WFH_FMT,0,sizeof(m_WFH_FMT));
	memset(&m_WFH_DATA,0,sizeof(m_WFH_DATA));
	return RETURN_SUCCESS;
}

const U8* CAlsaWaveData::GetWaveDataAddr()
{
	if( m_vtData.size() > 0 )
		return &m_vtData[0];
	else
		return 0;
}














