
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
#include <ctime>
#include <vector>
#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include "event_base_pool.h"
 
 
 
namespace libevent2{

class LIBEV_DLL_EXPORT tcp_client : boost::noncopyable
{
public:
	typedef tcp_client self;
	
	tcp_client();
	tcp_client(event_base * io_base);
	virtual ~tcp_client();
	 
	void init(event_base * io_base);
	 
	void set_host_info(const char* ip , unsigned short port);
	
	 
	void start();
	 
	void restart();
	 
	void stop();
	 
	bool write(const char* buf , int len);
	
	 
	void set_timer_interval(double intervals);
	void start_timer();
	void stop_timer();
	
	 
	void set_reconnect_interval(double intervals);
	void set_auto_reconnect(bool val);

	 
	 
	bool is_connected()const;
	evutil_socket_t get_fd()const;
	std::string get_ip()const;
	unsigned short get_port()const;
	size_t get_input_buffer_size()const;
	size_t get_output_buffer_size()const;
	unsigned long long total_read_bytes()const{ return m_total_read_bytes;}
	unsigned long long total_write_bytes()const { return m_total_write_bytes;}
private:
	 
	virtual void callback_err(const std::string& msg);

	virtual int callback_read(const char* buf , int len );
	
	virtual void callback_write();

	virtual void callback_connected(); 

	 
	virtual void callback_timer(); 

	static void _read_cb(struct bufferevent *bev, void *arg);
	static void _write_cb(struct bufferevent *bev, void *arg);
	static void _error_cb(struct bufferevent *bev, short event, void *arg);
	static void _time_cb(evutil_socket_t sock, short flags, void * arg);
	static void _reconnect_cb(evutil_socket_t sock, short flags, void * arg);
	static void _stop_cb(evutil_socket_t sock, short flags, void * arg);

	void _start();
	void _stop();

	event_base * m_io_base;
	boost::shared_ptr<bufferevent> m_bev;		 

	sockaddr_in m_sockaddr;
	bool m_need_stop;
	bool m_is_connectted;
	unsigned long long m_total_read_bytes;
	unsigned long long m_total_write_bytes;

	 
	std::time_t m_last_read;
	timeval  m_timer_interval;
	boost::shared_ptr<event> m_timer_ev;		 
	
	 
	bool m_auto_reconnect;
	timeval m_reconnect_interval;
	boost::shared_ptr<event> m_reconnect_ev;	 
	
	 
	timeval m_stop_interval;
	boost::shared_ptr<event> m_stop_ev;		 
};


}
 