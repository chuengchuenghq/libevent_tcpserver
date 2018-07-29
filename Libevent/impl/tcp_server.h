
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

#include <string>
#include <map>
#include <list>
#include <ctime>	 

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include "event_base_pool.h"

#include "../thread/mutex.h"

using namespace std;
 
 
 
namespace libevent2{

struct LIBEV_DLL_EXPORT tcp_session :boost::noncopyable
{
public:
	typedef tcp_session self;
	typedef std::vector<char> buffer_type;
	typedef boost::shared_ptr<buffer_type> buffer_ptr_type;

	tcp_session();
	virtual ~tcp_session();
	 
	bool write(const char* buf , int len);
	 
	void close();

	 
	 
	void set_timer_interval(double intervals);
	void start_timer();
	void stop_timer();

	 
	bool is_alive()const;
	evutil_socket_t get_fd()const;
	std::string get_ip()const; 
	unsigned short get_port()const;
	std::time_t get_last_read()const;
	size_t get_input_buffer_size() const;
	size_t get_output_buffer_size() const;
	unsigned long long total_read_bytes()const{ return m_total_read_bytes;}
	unsigned long long total_write_bytes()const { return m_total_write_bytes;}

protected:
	template<class your_session_type> friend class tcp_server;
	void __init(event_base * io_base , evutil_socket_t fd); 
	void __start();
	void __close();
	 
	 
	 
	virtual void callback_init();
	 
	virtual void callback_err(const std::string& msg); 
	 
	virtual int callback_read(int fd ,const char* buf , int len); 
	 
	virtual void callback_write();
	 
	virtual void callback_timer();


	 
	 
	static void _read_cb(struct bufferevent *bev, void *arg);
	static void _write_cb(struct bufferevent *bev, void *arg);
	static void _error_cb(struct bufferevent *bev, short event, void *arg);
	static void _time_cb(evutil_socket_t sock, short flags, void * arg);
	static void _close_cb(evutil_socket_t sock, short flags, void * arg);

	event_base* m_io_base;
	bufferevent* m_bev;
	sockaddr m_addr;
	bool m_is_alive;
	
	std::time_t m_last_read;
	unsigned long long m_total_read_bytes;
	unsigned long long m_total_write_bytes;
	
	 
	timeval  m_timer_interval;
	boost::shared_ptr<event> m_timer_ev;		 

	 
	timeval m_close_interval;
	boost::shared_ptr<event> m_close_ev;
};


template<class your_session_type>
class  LIBEV_DLL_EXPORT tcp_server
{
public:
	BOOST_STATIC_ASSERT( boost::is_base_of<tcp_session , your_session_type>::value );

	typedef tcp_server self;
	typedef your_session_type session_type;
	typedef boost::function<void(session_type*)> callback_new_connection_type;

	typedef base::win32::thread::Mutex mutext_type; 
	typedef mutext_type::LockerType lock_type;
	typedef session_type* session_list_type;

	tcp_server();
	~tcp_server();
	
	bool listening(unsigned short port , int backlog = 32);
	void start();
	void run();
	void stop();

	void set_callback_new_connection(callback_new_connection_type callback);
	 
	void set_session_heartbeat_elaspe(int heartbeart_seconds);
	 
	void set_timer_interval(double intervals);

	void send_packet(int connect, char* buf, int len);

	int session_count();
	
	template<class UnaryOp>
	void foreach_session(UnaryOp op)
	{
		lock_type lockit(m_session_list_mutex);
		for(std::map<int ,session_list_type>::iterator it = m_session_list.begin();
			it != m_session_list.end(); ++it)
		{
			op(it->second);
		}
	}

	void _delete_all_session();

private:
	void _start_timer(); 
	void _stop_timer();
 
	void _delete_dead_session();

	static void _listener_cb(struct evconnlistener *listener, evutil_socket_t fd,struct sockaddr *sa, int socklen, void *arg);
	static void _time_cb(evutil_socket_t sock, short flags, void * arg);
	static void _error_cb(struct evconnlistener *listener, void *arg);

	evconnlistener *m_listener;
	 
	event_base_pool m_server_pool;
	event_base_pool m_session_io_pool;	

	mutext_type m_session_list_mutex;
	std::map<int, session_list_type> m_session_list;

	event_base* m_io_base;
	event_base* m_timer_base;

	timeval  m_timer_interval;
	boost::shared_ptr<event> m_timer_ev; 
	
	int m_heartbeart_seconds; 
	callback_new_connection_type m_new_connection_callback; 
};


}

#include "tcp_server_impl.h"
 