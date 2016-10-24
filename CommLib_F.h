/*
 * CommLibInc.h
 *
 *  Created on: 2012-8-16
 *      Author: lucid
 */

#ifndef __COMM_LIB_INC_F_H__
#define __COMM_LIB_INC_F_H__
#include <stdio.h>

typedef unsigned char BYTE,U8;
typedef unsigned short WORD,U16;
typedef unsigned int DWORD,U32;

const int RETURN_ERROR_F			= 0;
const int RETURN_SUCCESS			= 1;


#define DBG_PRINT_ENABLE

#ifdef DBG_PRINT_ENABLE
	#define DBG_PRINT printf
#else
	#define DBG_PRINT
#endif

#define _STR(s) #s
#define STR(s) _STR(s)
#define DBG_COME_HERE(str) DBG_PRINT(#str)


#endif /* __COMM_LIB_INC_F_H__ */
