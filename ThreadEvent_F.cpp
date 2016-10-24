/*
 * thread_event.cpp
 *
 *  Created on: 2012-6-18
 *      Author: lucid
 */
#include "CommLib_F.h"

#include "ThreadEvent_F.h"


CTthreadEvent::CTthreadEvent()
{
	sem_init(&m_sem,0,0);
}

CTthreadEvent::~CTthreadEvent()
{
	sem_destroy(&m_sem);
}

void CTthreadEvent::SetEvent()
{
	int iVal;
	sem_getvalue(&m_sem,&iVal);
	if (0 == iVal)
	{
		sem_post(&m_sem);
	}
}

void CTthreadEvent::ResetEvent()
{
	int iVal;
	do
	{
		sem_trywait(&m_sem);
		sem_getvalue(&m_sem,&iVal);
	}while(iVal);
}

int CTthreadEvent::WaitEvent()
{
	return sem_wait(&m_sem);
}

int CTthreadEvent::TimedWaitEvent(const struct timespec * time)
{
	return sem_timedwait(&m_sem,time);
}






