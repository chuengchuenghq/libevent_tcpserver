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

#include <queue>
#include "condition.h"
#include "mutex.h"
#include "semaphore.h"


namespace base{ namespace win32{ namespace thread{

template<typename T>
struct bloking_queue 
{
	bloking_queue(){}

	void push(const T& data)
	{
		m_lock.lock();
		m_contain.push(data);
		m_lock.unlock();

		m_condition.notify();
	}

	void pop(T& data)
	{
		m_lock.lock();
		while( m_contain.size() == 0)
		{
			m_lock.unlock();
			m_condition.wait();
			m_lock.lock();
		}
		data = m_contain.front();
		m_contain.pop();
		m_lock.unlock();
	}

	int size()
	{
		Mutex::LockerType lk(m_lock);
		return m_contain.size();
	}
private:
	bloking_queue(const bloking_queue& other);
	bloking_queue& operator=(const bloking_queue& other);


	std::queue<T> m_contain;
	Condition m_condition;
	Mutex m_lock;
};


template<typename T>
struct limited_bloking_queue 
{
	limited_bloking_queue()
	{}

	limited_bloking_queue(long resourceCount)
	{
		init(resourceCount);
	}

	void init( long resourceCount )
	{
		mQueueEmpty.initialize(resourceCount , resourceCount);
		mQueueFull.initialize(resourceCount , 0);
	}

	void push(const T& data )
	{
		mQueueEmpty.acquire();
		mLock.lock();
		mQueue.push(data);
		mLock.unlock();
		mQueueFull.release();
	}

	void pop(T& data)
	{
		mQueueFull.acquire();
		mLock.lock();
		data = mQueue.front();
		mQueue.pop();
		mLock.unlock();
		mQueueEmpty.release();
	}

	int size()
	{
		Mutex::LockerType lk(mLock);
		return mQueue.size();
	}

	~limited_bloking_queue()
	{}

private:
	limited_bloking_queue(const limited_bloking_queue& other);
	limited_bloking_queue& operator=(const limited_bloking_queue& other);

	std::queue<T> mQueue;
	Mutex mLock;
	Semaphore mQueueFull;
	Semaphore mQueueEmpty;
};



}}}
