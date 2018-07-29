
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

#include "bloking_queue.h"
#include "Thread.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind/placeholders.hpp>
#include <vector>
using base::Thread;
using base::win32::thread::limited_bloking_queue;

template<class T>
struct handler_bounded_buffer : private boost::noncopyable
{
	typedef handler_bounded_buffer<T> this_type; 
	typedef boost::function<bool(const T&)> handler_type;


	template<class Tag>
	static this_type& instance()
	{
		static this_type ins;
		return ins;
	}
	
	handler_bounded_buffer():
	m_need_stop(false){}

	void set_cache_count(int n) 
	{
		m_bounded_buffer.init(n);
	}

	void start(handler_type cachehandler)
	{
		m_need_stop = false;
		m_cacheHandler = cachehandler;
		if( !m_bounded_buffer)
			set_cache_count(1000);

		m_thread.reset( new Thread(handler)	);		
	}

	void cache(const T& data) 
	{
		if(!m_need_stop)
			m_bounded_buffer.push(data);
	}
	
	void stop()
	{
		m_need_stop = true; 
		m_bounded_buffer.push( T() ); 
		m_thread->join(); 
	}

private:
	void handler(void*) 
	{
		while(true)
		{
			T temp;
			m_bounded_buffer.pop(temp);
			bool b = m_cacheHandler(temp);
			if(!b)
				break;
		}
	}
	
	handler_type m_cacheHandler;  
	limited_bloking_queue m_bounded_buffer;
	boost::shared_ptr<Thread> m_thread;
	bool m_need_stop;
};

typedef std::vector< std::vector<char> > CacheBufType;
typedef boost::shared_ptr< CacheBufType > CacheType;

struct MornAsioDecoderTag{};
#define mornAsioDecoder	handler_bounded_buffer<CacheType>::instance<MornAsioDecoderTag>()
