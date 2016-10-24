/*
 * file_ex_f.h
 *
 *  Created on: Jul 2, 2012
 *      Author: lucid
 */

#ifndef __FILE_F_H__
#define __FILE_F_H__
#include <sys/types.h>

class CFile
{
public:
	enum FILE_ACCESS_FLAG
	{
		FILE_ACCESS_FLAG_READ			= 0x00000001,
		FILE_ACCESS_FLAG_WRITE			= 0x00000002,
		FILE_ACCESS_FLAG_RDWR			= 0x00000003
	};

protected:
	FILE* m_pFile;

public:
	static int IsFileExist(const char* chName);

public:
	CFile();
	virtual ~CFile();

	operator FILE*()
	{
		return m_pFile;
	}

	int IsOpened();

	int Create(const char* chName,U32 dwFlag);				//return error if exist
	int CreateNewAlways(const char* chName,U32 dwFlag);	//create or open truncate to 0
	int OpenExist(const char* chName,U32 dwFlag);			//return error if not exist
	int CreateOrOpenExist(const char* chName,U32 dwFlag);	//create no truncate
	int OpenDirectly(const char* chName,const char* chModes);

	ssize_t Write(U32 dwPos,const void* pData,U32 dwLen);
	ssize_t Read(U32 dwPos,void* pData,U32 dwLen);
	int Flush();
	int Close();
	U32 GetLength();
	U32 GetCurrentPos();

	int SetLength(U32 uLen);
};


#endif /* __FILE_F_H__ */
