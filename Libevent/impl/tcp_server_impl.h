
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
#include "tcp_server.h"

namespace libevent2{
 
 
template<class your_session_type>
tcp_server<your_session_type>::tcp_server():m_server_pool(2)
{
	m_io_base = m_server_pool.get_service();
	m_timer_base = m_server_pool.get_service();
	m_heartbeart_seconds = 0;
	m_timer_interval.tv_sec = 2;
	m_timer_interval.tv_usec = 0;
}

template<class your_session_type>
tcp_server<your_session_type>::~tcp_server()
{
	stop();
}

template<class your_session_type>
bool tcp_server<your_session_type>::listening(unsigned short port , int backlog  )
{
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);

	m_listener = evconnlistener_new_bind( m_io_base, _listener_cb, this ,
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, backlog,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if(m_listener != NULL)
	{
		evconnlistener_set_error_cb(m_listener , _error_cb);
	}
	return m_listener != NULL;
}

template<class your_session_type>
void tcp_server<your_session_type>::start()
{
	m_session_io_pool.start();
	_start_timer();
	m_server_pool.start();
}

template<class your_session_type>
void tcp_server<your_session_type>::run()
{
	m_session_io_pool.start();
	_start_timer();
	m_server_pool.run();
}

template<class your_session_type>
void tcp_server<your_session_type>::stop()
{
	_stop_timer();
	if(m_listener)
	{
		evconnlistener_free(m_listener);
		m_listener = NULL;
	}
	_delete_all_session();
	m_session_io_pool.stop();
	m_server_pool.stop();
}


template<class your_session_type>
void tcp_server<your_session_type>::send_packet(int connect, char* buf, int len)
{
	lock_type lockit(m_session_list_mutex);

	map<int, session_list_type>::iterator it = m_session_list.find(connect);
	if(it != m_session_list.end())
	{
		if(it->second->is_alive())
			it->second->write(buf , len);
	}
}

template<class your_session_type>
BOOST_FORCEINLINE void tcp_server<your_session_type>::set_callback_new_connection(callback_new_connection_type callback)
{
	m_new_connection_callback = callback;
}

template<class your_session_type>
BOOST_FORCEINLINE void tcp_server<your_session_type>::set_session_heartbeat_elaspe(int heartbeart_seconds)
{
	m_heartbeart_seconds = heartbeart_seconds;
}

template<class your_session_type>
BOOST_FORCEINLINE void tcp_server<your_session_type>::set_timer_interval(double intervals)
{
	m_timer_interval.tv_sec = long(intervals);
	m_timer_interval.tv_usec = 1000000*( intervals - long(intervals));
}


template<class your_session_type>
BOOST_FORCEINLINE void tcp_server<your_session_type>::_start_timer()
{
	m_timer_ev.reset( event_new( m_timer_base, -1, EV_PERSIST , _time_cb , this ) , event_free);	 
	evtimer_add( m_timer_ev.get() ,  &m_timer_interval );
}

template<class your_session_type>
BOOST_FORCEINLINE void tcp_server<your_session_type>::_stop_timer()
{
	m_timer_ev.reset();
}

template<class your_session_type>
void tcp_server<your_session_type>::_delete_all_session()
{
#if 0
	lock_type lockit(m_session_list_mutex);
	for(map<int ,session_list_type>::iterator it = m_session_list.begin();
		it != m_session_list.end(); ++it)
	{
		session_type* pSession = it->second;
		delete pSession;
	}
	m_session_list.clear();
#endif
}

template<class your_session_type>
void tcp_server<your_session_type>::_delete_dead_session()
{
	std::time_t cur_time = std::time(NULL);
	lock_type lockit(m_session_list_mutex);
	for (map<int, session_list_type>::iterator it = m_session_list.begin();
		it != m_session_list.end(); )
	{
		session_type* pSession = it->second;

		if (!pSession->is_alive())
		{
			it = m_session_list.erase(it);
			delete pSession;
			continue;
		}

		++it;
	}
}


template<class your_session_type>
int tcp_server<your_session_type>::session_count()
{
	int cnt = 0;
	lock_type lockit(m_session_list_mutex);
	for(map<int ,session_list_type>::iterator it = m_session_list.begin();
		it != m_session_list.end(); ++it)
		if( it->second->is_alive() )
			++cnt;
	return cnt;
}

template<class your_session_type>
void tcp_server<your_session_type>::_listener_cb(
		struct evconnlistener *listener, evutil_socket_t fd,
		struct sockaddr *sa, int socklen, void *arg)
{
	self* this_ = (self*)arg;
	event_base *io_base = this_->m_session_io_pool.get_service();

	session_type* session = new session_type;
	session->m_addr = *sa;  
	session->__init( io_base , fd ); 
	if(this_->m_new_connection_callback) 
		this_->m_new_connection_callback(session);
	session->callback_init();
	session->__start(); 

	lock_type lockit(this_->m_session_list_mutex); 
	this_->m_session_list[fd] = session;
}


template<class your_session_type>
void tcp_server<your_session_type>::_time_cb(evutil_socket_t sock, 
											short flags, void * arg)
{
	 
	self* this_ = (self*)arg;
	this_->_delete_dead_session();
}

template<class your_session_type>
void tcp_server<your_session_type>::_error_cb(struct evconnlistener *listener, void *arg)
{
	self* this_ = (self*)arg;
	printf("listen error callback\n");
}

}