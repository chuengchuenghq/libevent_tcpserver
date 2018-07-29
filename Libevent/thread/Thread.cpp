#include "Thread.h"
#include <assert.h>
#include <Windows.h>
#include <process.h>

namespace base{

struct Thread::Private
{
	HANDLE m_handle;
	void (*m_callback)(void*);
	void* m_param;
	bool	m_blnStopFlag;
	unsigned int m_id;
	bool m_isAutoDelete;

	Private()
	{
		m_callback = NULL;
		m_handle = NULL;
		m_param = NULL;
		m_blnStopFlag = false;
		m_id = -1;
		m_isAutoDelete = false;
	}

	static unsigned __stdcall start_address( void * lParam);
};

 
unsigned __stdcall Thread::Private::start_address(void * lParam)
{
	Thread *pT = (Thread *)lParam;
	Thread::Private* pData = pT->pData;

	if(pData->m_callback)
		pData->m_callback(pData->m_param);
	else
		pT->onRun();
	pData->m_blnStopFlag = true;

	if(pData->m_isAutoDelete)
		delete pT;
	return 0;
}

Thread::Thread() :pData(new Private)
{
	init();
}


Thread::Thread(void (*callback)(void*) , void* param  ):pData(new Private)
{
	pData->m_callback = callback;
	pData->m_param = param;

	init();
}



void Thread::start()
{
	assert( pData->m_handle != NULL  && "Thread Handle Should Not Be Null");
	::ResumeThread(pData->m_handle);
}

 
void Thread::init()
{
	pData->m_blnStopFlag = false;
	pData->m_handle = (HANDLE)::_beginthreadex( NULL , NULL , Private::start_address , this , 
		CREATE_SUSPENDED , &pData->m_id );
}


Thread::~Thread()
{
	stop();
	delete pData;
}

void Thread::join()
{
	if(pData->m_handle != NULL)
	{
		::WaitForSingleObject(pData->m_handle,INFINITE);
	}
	pData->m_handle = NULL;
}



bool Thread::isRunning()const
{
	return !pData->m_blnStopFlag;
}

void Thread::setStopFlag()
{
	pData->m_blnStopFlag = true;
}

void Thread::onRun()
{

}


void Thread::stop()
{
	stop(INFINITE);
}

void Thread::stop(unsigned long dwMilliseconds)
{
	setStopFlag();
	if(pData->m_handle != NULL)
	{
		::WaitForSingleObject(pData->m_handle,dwMilliseconds);
		::CloseHandle(pData->m_handle);
	}
	pData->m_handle = NULL;
	pData->m_id = -1;
}

unsigned int Thread::id() const
{
	return pData->m_id;
}

bool Thread::setPriority(int priority)
{
	switch(priority)
	{
	case PriorityIdle: priority = THREAD_PRIORITY_IDLE;
		break;
	case PriorityLowest:priority = THREAD_PRIORITY_LOWEST;
		break;
	case PriorityBlowNormal:priority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case PriorityAboveNormal:priority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case PriorityHighest:priority = THREAD_PRIORITY_HIGHEST;
		break;
	case PriorityTimeCritical:priority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default: priority = THREAD_PRIORITY_NORMAL;
	}
	return TRUE == SetThreadPriority(pData->m_handle , priority);
}

int Thread::getPriority()
{
	int priority = PriorityNormal;
	switch( ::GetThreadPriority(pData->m_handle) )
	{
	case THREAD_PRIORITY_IDLE: priority = PriorityIdle;
		break;
	case THREAD_PRIORITY_LOWEST:priority = PriorityLowest ;
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:priority = PriorityBlowNormal;
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:priority = PriorityAboveNormal;
		break;
	case THREAD_PRIORITY_HIGHEST:priority = PriorityHighest ;
		break;
	case THREAD_PRIORITY_TIME_CRITICAL:priority = PriorityTimeCritical ;
		break;
	}
	return priority;

}

void Thread::setAutoDelete(bool isAutoDelete)
{
	pData->m_isAutoDelete = isAutoDelete;
}


void Thread::abort(unsigned long dwMilliseconds  )
{
	setStopFlag();
	if(pData->m_handle != NULL)
	{
		::WaitForSingleObject(pData->m_handle,dwMilliseconds);
		::TerminateThread(pData->m_handle , 1);
		::CloseHandle(pData->m_handle);
	}
	pData->m_handle = NULL;
	pData->m_id = -1;
}



namespace ThisThread{

unsigned int current_thread_id()
{
	return ::GetCurrentThreadId();
}

void sleep(unsigned int ms)
{
	::Sleep(ms);
}

int support_thread_count()
{
	SYSTEM_INFO si;  
	::GetSystemInfo(&si);  
	return si.dwNumberOfProcessors;
}

void yield()
{
	::SwitchToThread();
	
}

void yield_processor()
{
	YieldProcessor();
}

 


} 

}