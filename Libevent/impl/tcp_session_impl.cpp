

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

#include <event2/buffer.h>
#include "tcp_server.h"
 
#include <iostream>

using namespace std;

namespace libevent2{

tcp_session::tcp_session()
:m_bev(NULL)
{
	m_timer_interval.tv_sec = 30;
	m_timer_interval.tv_usec = 0;

	m_close_interval.tv_sec = 0;
	m_close_interval.tv_usec = 0;
}

tcp_session::~tcp_session()
{
	this->__close();
}

 
void tcp_session::close()
{
	if(m_close_ev.get())
		evtimer_add( m_close_ev.get() ,  &m_close_interval );
}

evutil_socket_t tcp_session::get_fd()const
{
	return bufferevent_getfd(m_bev);
}

void tcp_session::__init(event_base * io_base , evutil_socket_t fd )
{
	m_io_base = io_base;
	int flags = BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE;
	m_bev = bufferevent_socket_new(io_base, fd,  flags); 
	assert( m_bev && "bufferevent should not be Null");
	
	 
	m_close_ev.reset( evtimer_new( io_base , _close_cb , this )  , event_free);

	m_last_read = std::time(NULL);
	m_total_read_bytes = 0;
	m_total_write_bytes = 0;
	m_is_alive = true;
}

void tcp_session::__start()
{
	bufferevent_setcb(m_bev, _read_cb, _write_cb, _error_cb, this);
	bufferevent_enable(m_bev, EV_READ|EV_WRITE|EV_PERSIST);
}

void tcp_session::__close()
{
	m_is_alive = false;

	stop_timer();
	m_close_ev.reset();

	if(m_bev)
	{
		bufferevent_free(m_bev);
		m_bev = NULL;
	}
}

void tcp_session::set_timer_interval(double intervals)
{
	m_timer_interval.tv_sec = long(intervals);
	m_timer_interval.tv_usec = 1000000*( intervals - long(intervals));
}

bool tcp_session::write(const char* buf , int len)
{
	if (len <= 0)
	{
		return false;
	}

	bool ret = bufferevent_write(m_bev, buf, len) ==0; 
	if(ret)
		m_total_write_bytes += len;

	return ret;
}

std::string tcp_session::get_ip()const
{
	char dest[26] = { '\0' };
	struct in_addr* sin_addr = &((sockaddr_in*)(&m_addr))->sin_addr;
	const unsigned char* bytes = (const unsigned char*)sin_addr;
	evutil_snprintf( dest, 25, "%u.%u.%u.%u",
		bytes[0], bytes[1], bytes[2], bytes[3]);

	return dest;
}

unsigned short tcp_session::get_port()const
{
	return ntohs( ((sockaddr_in*)(&m_addr))->sin_port);
}

std::time_t tcp_session::get_last_read() const
{
	return m_last_read;
}

size_t tcp_session::get_input_buffer_size() const
{
	evbuffer* readBuf = ::bufferevent_get_input(m_bev);
	::evbuffer_lock(readBuf);
	int len = ::evbuffer_get_length(readBuf);
	::evbuffer_unlock(readBuf);
	return len;
}

size_t tcp_session::get_output_buffer_size() const
{
	evbuffer* readBuf = ::bufferevent_get_output(m_bev);
	::evbuffer_lock(readBuf);
	int len = ::evbuffer_get_length(readBuf);
	::evbuffer_unlock(readBuf);
	return len;
}

void tcp_session::start_timer() 
{
	m_timer_ev.reset( event_new( m_io_base, -1, EV_PERSIST , _time_cb , this ) , event_free);	 
	evtimer_add( m_timer_ev.get() ,  &m_timer_interval );
}

void tcp_session::stop_timer()
{
	m_timer_ev.reset();
}

void tcp_session::callback_init()
{}

void tcp_session::callback_err(const std::string& msg)
{}

int tcp_session::callback_read(int fd,const char* buf , int len)
{
	return len;
}

void tcp_session::callback_write()
{}
 
void tcp_session::callback_timer()
{}

void tcp_session::_read_cb(struct bufferevent *bev, void *arg)
{
	self* this_ = (self*)arg;
	this_->m_last_read = std::time(NULL);

	 
	evbuffer* readBuf = bufferevent_get_input(this_->m_bev);
	::evbuffer_lock(readBuf);
	int len = evbuffer_get_length(readBuf);
	const char* buf = (const char*)evbuffer_pullup(readBuf,len);

	int fd = this_->get_fd();
	int cosume_len = this_->callback_read(fd,buf , len);
	if(cosume_len > 0)
	{
		this_->m_total_read_bytes += cosume_len;
		evbuffer_drain(readBuf , cosume_len);
	}
	::evbuffer_unlock(readBuf);
	 
}

void tcp_session::_write_cb(struct bufferevent *bev, void *arg)
{
	self* this_ = (self*)arg;
	this_->callback_write();
}

void tcp_session::_error_cb(struct bufferevent *bev, short event, void *arg)
{
	self* this_ = (self*)arg;
	std::string msg = "socket closed,";

	if (event & BEV_EVENT_TIMEOUT) {
		msg += "time-out";  
	}
	else if (event & BEV_EVENT_EOF) {
		msg += "connection-closed";
	}
	else if (event & BEV_EVENT_ERROR) {
		msg += "some-other-error";
	}

	if(event & BEV_EVENT_READING)
		msg += " when-reading";
	else if(event & BEV_EVENT_WRITING)
		msg += " when-writing";
	
 

	this_->callback_err(msg);
	this_->__close();
}


void tcp_session::_time_cb(evutil_socket_t sock, short flags, void * arg)
{
	self* this_ = (self*)arg;
	this_->callback_timer();
}


void tcp_session::_close_cb(evutil_socket_t sock, short flags, void * arg)
{
	self* this_ = (self*)arg;
	this_->__close();
}

bool tcp_session::is_alive() const
{
	return m_is_alive;
}


}



