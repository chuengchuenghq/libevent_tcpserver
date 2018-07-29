#include "Semaphore.h"

#include <Windows.h>
#include <cassert>

namespace base{ namespace win32{ namespace thread{

struct Semaphore::Private
{
	HANDLE m_handle;

	Private()
	{
		m_handle = NULL;
	}
};

Semaphore::Semaphore(long totalCount , long initCount  ) :pData(new Private)
{
	initialize(totalCount , initCount);
}

Semaphore::Semaphore():pData(new Private)
{

}

Semaphore::~Semaphore()
{
	if(pData->m_handle != NULL)
		::CloseHandle(pData->m_handle);
	pData->m_handle = NULL;
	delete pData;
}

void Semaphore::initialize(long totalCount , long initCount  )
{
	if(pData->m_handle != NULL)
		::CloseHandle(pData->m_handle);
	pData->m_handle = CreateSemaphore(NULL, initCount, totalCount, NULL);
}

void Semaphore::release(long count  )
{
	assert( pData->m_handle!=NULL && "Semaphore::release handle is not initialize , default Null" );
	::ReleaseSemaphore(pData->m_handle, count, NULL);
}

void Semaphore::acquire()
{
	assert( pData->m_handle!=NULL && "Semaphore::acquire handle is not initialize , default Null" );
	::WaitForSingleObject(pData->m_handle, INFINITE); 
}

bool Semaphore::tryAcquire(unsigned long dwMilliseconds)
{
	assert( pData->m_handle!=NULL && "Semaphore::acquire handle is not initialize , default Null" );
	return ::WaitForSingleObject(pData->m_handle, dwMilliseconds) == WAIT_OBJECT_0; 
}

int Semaphore::getCount() const
{
	assert( pData->m_handle!=NULL && "Semaphore::acquire handle is not initialize , default Null" );
	LONG v = -1;

	if (!::ReleaseSemaphore (pData->m_handle, 0, &v) || v < 0)
	{
		 
		assert("error here:cannot get value" );
	}

	return v;
}





}}}

