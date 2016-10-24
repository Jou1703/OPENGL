/*
 * pthread_ex.cpp
 *
 *  Created on: 2012-6-18
 *      Author: lucid
 */

#include "CommLib_F.h"
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "Thread_F.h"

const int SIG_RESUME = SIGUSR1;
const int SIG_SUSPEND = SIGUSR2;

static struct sigaction 	s_saSuspend;
static struct sigaction		s_saResume;

int CThread::s_iStaticInited = 0;

CThread::CThread()
{
	pthread_attr_init(&m_ThreadAttr);
	StaticInit();
	m_Thread = 0;
}

CThread::~CThread()
{
	pthread_attr_destroy(&m_ThreadAttr);
}

void SuspendHandler(int signum)
{
    sigset_t nset;
    pthread_sigmask(0, NULL, &nset);
    /* make sure that the resume is not blocked*/
    sigdelset(&nset, SIG_RESUME);
   	sigsuspend(&nset);
}

void ResumeHandler(int signum)
{
}
//static
int CThread::StaticInit()
{
	if (0 != s_iStaticInited)
		return 0;
	s_iStaticInited = 1;

	s_saSuspend.sa_handler = SuspendHandler;
	sigaddset(&s_saSuspend.sa_mask, SIG_RESUME);
	sigaction( SIG_SUSPEND, &s_saSuspend, NULL);

	s_saResume.sa_handler = ResumeHandler;
	sigaddset(&s_saResume.sa_mask, SIG_SUSPEND);
	sigaction( SIG_RESUME, &s_saResume, NULL);
	return 1;
}

int CThread::Create(THREAD_PROC start_routine,void* pData)
{
	return pthread_create(&m_Thread,&m_ThreadAttr,start_routine,pData);
}

int CThread::DetechThread()
{
	pthread_detach( (pthread_t)pthread_self() );
	m_Thread = 0;
	return RETURN_SUCCESS;
}

int CThread::IsThreadRunning()
{
	if ( 0 == m_Thread )
		return RETURN_ERROR_F;

	int iRet = pthread_kill(m_Thread,0);
	if ( ESRCH == iRet )
		return RETURN_ERROR_F;
	else if ( EINVAL == iRet )
		return RETURN_ERROR_F;

	return RETURN_SUCCESS;
}

void* CThread::Join()
{
	if( 0 == m_Thread )
		return 0;
	void* pRet;
	if( 0 != pthread_join(m_Thread,&pRet) )
		return 0;
	return pRet;
}


int CThread::SuspendThread()
{
	return (0 == pthread_kill(m_Thread,SIG_SUSPEND)) ? RETURN_SUCCESS : RETURN_ERROR_F;
}

int CThread::ResumeThread()
{
	return (0 == pthread_kill(m_Thread,SIG_RESUME)) ? RETURN_SUCCESS : RETURN_ERROR_F;
}

int CThread::SetSchedPolicy(int iPolicy)
{
	return pthread_attr_setschedpolicy(&m_ThreadAttr,iPolicy);
}

int CThread::GetSchedPolicy(int& iPolicy)
{
	return pthread_attr_getschedpolicy(&m_ThreadAttr,&iPolicy);
}

int CThread::SetPriority(int iPriority)
{
	struct sched_param TSP;
	TSP.__sched_priority = iPriority;
	return pthread_attr_setschedparam(&m_ThreadAttr,&TSP);
}

int CThread::GetPriority(int& iPriority)
{
	struct sched_param TSP;
	int iRet = pthread_attr_getschedparam(&m_ThreadAttr,&TSP);
	iPriority = TSP.__sched_priority;
	return iRet;
}

//该属性的可选值有：
//PTHREAD_CREATE_DETACHED、PTHREAD_CREATE_JOINABLE
int CThread::SetDetachState(int iState)
{
	return pthread_attr_setdetachstate(&m_ThreadAttr,iState);
}

int CThread::GetDetachState(int& iState)
{
	return pthread_attr_getdetachstate(&m_ThreadAttr,&iState );
}

























