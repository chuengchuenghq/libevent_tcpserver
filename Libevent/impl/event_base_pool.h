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
///  ----------------------------------------------------------------------------
#pragma once
 
#include <vector>
#include <boost/shared_ptr.hpp>
#include <event2/event.h>  

#include "../thread/Thread.h"

#define LIBEVNET_WINDOWS_USE_IOCP

#define LIBEV_DLL_EXPORT __declspec(dllexport)

 
namespace libevent2{
using base::Thread;

class LIBEV_DLL_EXPORT event_base_pool
{
public:
	typedef event_base_pool self;
	 
		 
	explicit event_base_pool(std::size_t pool_size = 0);
	 
		 
	~event_base_pool();

	 
	void start(int flags = EVLOOP_NO_EXIT_ON_EMPTY);

	 
	void run(int flags = EVLOOP_NO_EXIT_ON_EMPTY);

	 
	void stop();

	 
	event_base * get_service();
private:
	event_base_pool(const event_base_pool& );
	event_base_pool& operator=(const event_base_pool& );
	static void _thread_func(void* param);
	
	bool m_is_running;
	std::size_t m_next_io_service; 

	std::vector<boost::shared_ptr<event_base> > m_io_services; 
	std::vector<boost::shared_ptr<Thread> > m_threads; 
};


}
