
//  ----------------------------------------------------------------------------
/// 
///  
/// 
//  ----------------------------------------------------------------------------
/// 
/// \file
/// \brief   Short description
/// 
//  ----------------------------------------------------------------------------
/// 
/// \par     Version Info:
///          $Source: v $\n
///          $Author: tzx $\n
///          $Revision: 1.0 $\n
///          $Date: 2017-02-25 $\n
///
//  ----------------------------------------------------------------------------
/// 
/// \par     Resources and environment
/// 
//  ----------------------------------------------------------------------------
/// 
/// \par     Documentation:
/// 
/// \par     Details:
/// 
/// \par     Usage:
///
//  ----------------------------------------------------------------------------
/// 
/// \par     Versions:
///          $Log: v $
///
///          Revision 1.0   2017-02-25 tzx
///          initial check-in
///          email:chuengchuenghq@yeah.net
///
//  ----------------------------------------------------------------------------
/// 

#include "Mutex.h"
#include <Windows.h>

namespace base{ namespace win32{ namespace thread{

 
 
 
struct Mutex::Private
{
	::CRITICAL_SECTION m_cs;
};
 
Mutex::Mutex():pData(new Private)
{
	::InitializeCriticalSection(&pData->m_cs);
	::SetCriticalSectionSpinCount(&pData->m_cs , 4000);
}

Mutex::~Mutex()
{
	::DeleteCriticalSection(&pData->m_cs);
	delete pData;
}


bool Mutex::tryLock()
{
	return TRUE == TryEnterCriticalSection(&pData->m_cs);
}


void Mutex::lock()
{
	::EnterCriticalSection(&pData->m_cs);
}

void Mutex::unlock()
{
	::LeaveCriticalSection(&pData->m_cs);
}


}}}