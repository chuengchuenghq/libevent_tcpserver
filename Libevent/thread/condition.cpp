#include "Condition.h"

#include <Windows.h>

namespace base{ namespace win32{ namespace thread{

struct Condition::Private
{
	CRITICAL_SECTION m_cs;
	CONDITION_VARIABLE m_condition;
	Private()
	{
		::InitializeCriticalSection(&m_cs);
		::InitializeConditionVariable(&m_condition);
	}

	~Private()
	{
		::DeleteCriticalSection(&m_cs);
	}
};


Condition::~Condition()
{
	delete pData;
}

Condition::Condition() :pData(new Private)
{

}

void Condition::notifyAll()
{
	::WakeAllConditionVariable(&pData->m_condition);
}

void Condition::notify()
{
	::WakeConditionVariable(&pData->m_condition);
}

bool Condition::tryWait(unsigned long dwMilliseconds)
{
	BOOL res = FALSE;
	::EnterCriticalSection(&pData->m_cs);
	res = ::SleepConditionVariableCS( &pData->m_condition , &pData->m_cs  , dwMilliseconds);
	::LeaveCriticalSection(&pData->m_cs);
	return FALSE != res;
}

void Condition::wait()
{	
	::EnterCriticalSection(&pData->m_cs);
	::SleepConditionVariableCS( &pData->m_condition , &pData->m_cs  , INFINITE);	
	::LeaveCriticalSection(&pData->m_cs);
}


}}}