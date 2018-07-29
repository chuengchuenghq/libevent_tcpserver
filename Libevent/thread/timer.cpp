#include "timer.h"
#include "Thread.h"

#include <memory>
#include <Windows.h>

namespace base{


struct timer::Private
{
	Private()
	{
		m_isSingleShot = false;
		m_intervalMillSeconds = 1000;
		m_param = NULL;
		m_callback = NULL;
		m_pcall = NULL;
	}

	void close()
	{
		if(Thread* pTh = m_thread.get() )
			pTh->stop();
	}

	bool m_isSingleShot;
	int m_intervalMillSeconds;

	timer_event *m_pcall;
	void* m_param;
	void (*m_callback)(void*);

	std::auto_ptr<Thread> m_thread;

	static void ThreadProc(void* para);
};


void timer::Private::ThreadProc(void* para)
{
	timer::Private* pData = (timer::Private*)para;

	while( pData->m_thread->isRunning() )
	{
		for(int i = 0 ; i< pData->m_intervalMillSeconds/50 && pData->m_thread->isRunning() ; ++i)
			base::ThisThread::sleep(50);

		if(pData->m_callback)
			pData->m_callback(pData->m_param);
		else if(pData->m_pcall)
			pData->m_pcall->onTimeEvent();
		
		if(pData->m_isSingleShot)
			break;
	}

}

timer::timer():pData(new Private)
{
	
}

timer::timer(timer_event* pcall):pData(new Private)
{
	setCallBack(pcall);
}

timer::timer(void(*callback)(void*) , void* param):pData(new Private)
{
	setCallBack(callback,param);
}

timer::timer(void(*callback)(void*) , void* param , int intervalMillSeconds , bool isSingleShot):pData(new Private)
{
	setCallBack(callback,param);
	setInterval(intervalMillSeconds);
	setSingleShot(isSingleShot);
}

bool timer::start()
{
	pData->m_thread.reset(new Thread(Private::ThreadProc , pData) );
	pData->m_thread->start();
	return true;
}


timer::~timer()
{
	stop();
	delete pData;
}


void timer::setCallBack(timer_event* pcall)
{
	pData->m_pcall = pcall;
}

void timer::setCallBack(void(*callback)(void*) , void* param  )
{
	pData->m_callback = callback;
	pData->m_param = param;
}

void timer::setInterval(int intervalMillSeconds)
{
	pData->m_intervalMillSeconds = intervalMillSeconds;
}

void timer::setSingleShot(bool isSingleShot)
{
	pData->m_isSingleShot = isSingleShot;
}

void timer::stop()
{
	if(Thread* pTh = pData->m_thread.get() )
		pTh->stop();
}

}