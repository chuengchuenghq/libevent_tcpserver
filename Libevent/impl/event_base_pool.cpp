

#include "event_base_pool.h"
#include <event2/thread.h>
#include <utility>


namespace libevent2{

static void _init()
{
#ifdef WIN32  
	WORD wVersionRequested;  
	WSADATA wsaData;  

	wVersionRequested = MAKEWORD(2, 2);  

	(void) WSAStartup(wVersionRequested, &wsaData);  
#endif  

#ifdef WIN32
	int ret = evthread_use_windows_threads(); 
#else
	int ret = evthread_use_pthreads();     
#endif
	assert( ret == 0 && "cannot use threads in event thread");
}

void event_base_pool::stop()
{
	if( !m_is_running )
		return;

	m_is_running = false;
	 
	for (std::size_t i = 0; i < m_io_services.size(); ++i)
	{
		event_base_loopexit(m_io_services[i].get() , NULL);  
	}
	 
	for (std::size_t i = 0; i < m_threads.size(); ++i) 
	{
		m_threads[i]->join();
	}

	m_threads.clear();
	m_io_services.clear();
}

void event_base_pool::run(int flags  )
{
	start();
	 
	for (std::size_t i = 0; i < m_threads.size(); ++i) 
		m_threads[i]->join();
}

void event_base_pool::start(int flags  )
{
	if( m_is_running)
		return;

	 
	for (std::size_t i = 0; i < m_io_services.size(); ++i) 
	{ 
		std::pair<event_base* , int>* pPair = new std::pair<event_base* , int>(m_io_services[i].get() , flags);
		Thread* pTh = new Thread( _thread_func , pPair );
		m_threads.push_back(boost::shared_ptr<Thread>(pTh)); 
		pTh->start();
	}

	m_is_running = true;
}

event_base_pool::~event_base_pool()
{
	stop();
}

event_base_pool::event_base_pool(std::size_t pool_size  )
{
	_init();
	m_next_io_service = 0;

	if (pool_size == 0) 
		pool_size = base::ThisThread::support_thread_count();
 
	for (std::size_t i = 0; i < pool_size; ++i) 
	{ 
#ifdef WIN32 
	#ifdef LIBEVNET_WINDOWS_USE_IOCP
			event_config *evcfg = event_config_new();
			event_config_set_flag(evcfg, EVENT_BASE_FLAG_STARTUP_IOCP);
			event_config_set_num_cpus_hint(evcfg ,base::ThisThread::support_thread_count() );
			event_base *io_service_ptr = event_base_new_with_config(evcfg);
			event_config_free(evcfg);
	#else
			event_base *io_service_ptr = event_base_new();  
	#endif
#else
			event_base *io_service_ptr = event_base_new();  
#endif
		m_io_services.push_back( boost::shared_ptr<event_base>(io_service_ptr , ::event_base_free) ); 
	}

	m_is_running = false;
}


void event_base_pool::_thread_func(void* param)
{
	std::pair<event_base* , int>* pPair = (std::pair<event_base* , int>*)param;
	event_base * io_service = pPair->first;
	int flags = pPair->second;
	::event_base_loop(io_service ,flags );
	delete pPair;
}

event_base * event_base_pool::get_service()
{
	 
	event_base * io_service = m_io_services[m_next_io_service].get(); 
	++m_next_io_service; 
	if (m_next_io_service == m_io_services.size()) 
		m_next_io_service = 0; 
	return io_service;
}


}

