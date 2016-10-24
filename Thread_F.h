/*
 * pthread_ex_f.h
 *
 *  Created on: 2012-6-18
 *      Author: lucid
 */

#ifndef __THREAD_F_H__
#define __THREAD_F_H__

//#define _REENTRANT

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>


typedef void* (* THREAD_PROC)(void*);

template<typename T>
class MberFnPtr
{
public:
	typedef void (T::* MberFnPtrInCls)(void);	//类成员函数指针的模板
	MberFnPtrInCls m_MTP;							//指向 T 类型的成员函数

	MberFnPtr(){}

	MberFnPtr(MberFnPtrInCls mtp)
	{
		m_MTP = mtp;
	}

	static void* _MidThreadProc(void* pData);
};

template<typename T>
struct _MidParam
{
	T * pThis;
	MberFnPtr<T> m_MberFnPtr;
};

template<typename T>
void* MberFnPtr<T>::_MidThreadProc(void* pData)
{
	_MidParam<T>* pMidParam = (_MidParam<T>*)pData;
	(pMidParam->pThis->* pMidParam->m_MberFnPtr.m_MTP)();
	delete pMidParam;
	return 0;
}

class CThread
{
protected:
	pthread_t					m_Thread;
	pthread_attr_t				m_ThreadAttr;

	static int s_iStaticInited;

public:
	static int StaticInit();

public:
	CThread();
	virtual ~CThread();

	operator pthread_t()
	{
		return m_Thread;
	}

	int Create(THREAD_PROC start_routine,void* pData);
	int IsThreadRunning();
	void* Join();

	int SetSchedPolicy(int iPolicy);
	int GetSchedPolicy(int& iPolicy);
	int SetPriority(int iPriority);
	int GetPriority(int& iPriority);
	int SetDetachState(int iState);
	int GetDetachState(int& iState);
	int SuspendThread();
	int ResumeThread();
	int DetechThread();

	template<class Type1>
	void CreateAtMberFn(MberFnPtr<Type1>& MberFn,Type1* pThis)
	{
		_MidParam<Type1>* pMP = new _MidParam<Type1>;
		pMP->pThis = pThis;
		pMP->m_MberFnPtr.m_MTP = MberFn.m_MTP;
		pthread_create(&m_Thread,&m_ThreadAttr,
				MberFnPtr<Type1>::_MidThreadProc,pMP);
	}
};



#endif /* __THREAD_F_H__ */
