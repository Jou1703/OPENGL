/*
 * thread_event_f.h
 *
 *  Created on: 2012-6-18
 *      Author: lucid
 */

#ifndef __THREAD_EVENT_F_H__
#define __THREAD_EVENT_F_H__

#include <semaphore.h>

class CTthreadEvent
{
protected:
	sem_t m_sem;

public:
	CTthreadEvent();
	~CTthreadEvent();

	void SetEvent();
	void ResetEvent();
	int WaitEvent();
	int TimedWaitEvent(const struct timespec * time);
};


#endif /* __THREAD_EVENT_F_H__ */
