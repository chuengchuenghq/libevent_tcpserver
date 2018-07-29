
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
#pragma once

#include "event_base_pool.h"

namespace base{ namespace win32{ namespace thread{

template<class MutexType>
struct LIBEV_DLL_EXPORT  Locker
{
	Locker(MutexType& mu):m_mutex(mu)
	{
		m_mutex.lock();
	}

	~Locker()
	{
		m_mutex.unlock();
	}

	MutexType& m_mutex;
};


struct LIBEV_DLL_EXPORT  VoidMutex
{
	typedef VoidMutex MutexType;
	typedef Locker<VoidMutex> LockerType;

	VoidMutex(){}
	~VoidMutex(){}
	bool tryLock(){ return true;}
	void lock(){}
	void unlock(){}
private:
	VoidMutex(const VoidMutex& other);
	VoidMutex& operator=(const VoidMutex& other);
};

struct LIBEV_DLL_EXPORT Mutex
{
	typedef Mutex MutexType;
	typedef Locker<Mutex> LockerType;

	Mutex();
	~Mutex();

	void lock();
	bool tryLock();
	void unlock();
private:
	Mutex(const Mutex& other);
	Mutex& operator=(const Mutex& other);

	struct Private;
	Private* pData;

};


typedef	Mutex::LockerType AutoLock;


}}}
