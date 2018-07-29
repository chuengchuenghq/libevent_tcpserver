#include <event2/buffer.h>
#include "tcp_client.h"


namespace libevent2{

tcp_client::tcp_client(event_base * io_base)
{
	init(io_base);
}

tcp_client::tcp_client()
{

}

tcp_client::~tcp_client()
{
	stop();
}

void tcp_client::init(event_base * io_base)
{
	m_io_base = io_base;

	m_is_connectted = false;
	m_need_stop = false;
	m_total_read_bytes = 0;
	m_total_write_bytes = 0;

	 
	m_auto_reconnect = false;
	m_reconnect_interval.tv_sec = 0;
	m_reconnect_interval.tv_usec = 0;
	m_reconnect_ev.reset( evtimer_new( m_io_base , _reconnect_cb , this ) , event_free);	 

	 
	m_timer_interval.tv_sec = 30;
	m_timer_interval.tv_usec = 0;

	 
	m_stop_interval.tv_sec = 0;
	m_stop_interval.tv_usec = 0;
	m_stop_ev.reset( evtimer_new( io_base , _stop_cb , this )  , event_free);
}

void tcp_client::set_host_info(const char* ip , unsigned short port)
{
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(port);
	evutil_inet_pton( AF_INET ,  ip, &m_sockaddr.sin_addr);
	memset(m_sockaddr.sin_zero, 0x00, 8);
}

 
void tcp_client::start()
{
	this->_start();
}

 
void tcp_client::restart()
{
	evtimer_add( m_reconnect_ev.get() , &m_reconnect_interval);
}

 
void tcp_client::stop()
{
	evtimer_add( m_stop_ev.get() ,  &m_stop_interval );
}

 
void tcp_client::start_timer()
{
	m_timer_ev.reset( event_new( m_io_base, -1, EV_PERSIST , _time_cb , this ) , event_free);	 
	evtimer_add( m_timer_ev.get() ,  &m_timer_interval );
}

 
void tcp_client::stop_timer()
{
	m_timer_ev.reset();
}

 
bool tcp_client::write(const char* buf , int len)
{
	bool ret = bufferevent_write(m_bev.get(), buf, len) ==0;
	if(ret)
		m_total_write_bytes += len;
	return ret;
}


void tcp_client::set_timer_interval(double intervals)
{
	m_timer_interval.tv_sec = long(intervals);
	m_timer_interval.tv_usec = 1000000*( intervals - long(intervals));
}

void tcp_client::set_reconnect_interval(double intervals)
{
	m_reconnect_interval.tv_sec = long(intervals);
	m_reconnect_interval.tv_usec = 1000000*( intervals - long(intervals));
}

void tcp_client::set_auto_reconnect(bool val)
{
	m_auto_reconnect = val;
}

bool tcp_client::is_connected()const
{
	return m_is_connectted;
}

evutil_socket_t tcp_client::get_fd() const
{
	return ::bufferevent_getfd(m_bev.get());
}

std::string tcp_client::get_ip() const
{
	char dest[26] = { '\0' };
	struct in_addr* sin_addr = (struct in_addr*)(&m_sockaddr.sin_addr);
	const unsigned char* bytes = (const unsigned char*)sin_addr;
	::evutil_snprintf( dest, 25, "%u.%u.%u.%u",
		bytes[0], bytes[1], bytes[2], bytes[3]);
	return dest;
}

unsigned short tcp_client::get_port() const
{
	return ntohs( m_sockaddr.sin_port);
}

size_t tcp_client::get_input_buffer_size() const
{
	evbuffer* readBuf = ::bufferevent_get_input(m_bev.get());
	::evbuffer_lock(readBuf);
	int len = ::evbuffer_get_length(readBuf);
	::evbuffer_unlock(readBuf);
	return len;
}

size_t tcp_client::get_output_buffer_size() const
{
	evbuffer* readBuf = ::bufferevent_get_output(m_bev.get());
	::evbuffer_lock(readBuf);
	int len = ::evbuffer_get_length(readBuf);
	::evbuffer_unlock(readBuf);
	return len;
}

void tcp_client::callback_err(const std::string& msg)
{}

int tcp_client::callback_read(const char* buf , int len)
{
	return len;
}

void tcp_client::callback_connected()
{}

void tcp_client::callback_timer()
{}

void tcp_client::callback_write()
{}

void tcp_client::_read_cb(struct bufferevent *bev, void *arg)
{
	self* this_ = (self*)arg;
	evbuffer* readBuf = ::bufferevent_get_input(this_->m_bev.get());
	::evbuffer_lock(readBuf);

	int len = ::evbuffer_get_length(readBuf);
	const char* buf = (const char*)::evbuffer_pullup(readBuf,len);

	int fd = this_->get_fd();

	int cosume_len = this_->callback_read(buf , len );
	if(cosume_len > 0)
	{
		this_->m_total_read_bytes += cosume_len;
		::evbuffer_drain(readBuf , cosume_len);
	}
	::evbuffer_unlock(readBuf);
}


void tcp_client::_write_cb(struct bufferevent *bev, void *arg)
{
	self* this_ = (self*)arg;
	this_->callback_write();
}

void tcp_client::_error_cb(struct bufferevent *bev, short event, void *arg)
{
	self* this_ = (self*)arg;
	bool has_error = false;
	std::string msg = "unknown";
	if (event & BEV_EVENT_TIMEOUT) {
		msg = "time-out";  
		has_error = true;
	}
	else if (event & BEV_EVENT_EOF) {
		msg = "connection-closed";
		has_error = true;
	}
	else if (event & BEV_EVENT_ERROR) {
		msg = "some-other-error";
		has_error = true;
	}

	if( event & BEV_EVENT_CONNECTED) 
	{	
		this_->m_is_connectted = true;
		this_->callback_connected();
	}
	else if(event & BEV_EVENT_READING)
		msg += " when-reading";
	else if(event & BEV_EVENT_WRITING)
		msg += " when-writing";

	if( has_error )
	{
		this_->m_is_connectted = false;
		this_->callback_err(msg);
		 
		if( !this_->m_need_stop && this_->m_auto_reconnect )
			this_->restart();
	}

}

void tcp_client::_time_cb(evutil_socket_t sock, short flags, void * arg)
{
	self* this_ = (self*)arg;
	this_->callback_timer();
}

void tcp_client::_reconnect_cb(evutil_socket_t sock, short flags, void * arg)
{
	self* this_ = (self*)arg;
	this_->_start();
}

void tcp_client::_stop_cb(evutil_socket_t sock, short flags, void * arg)
{
	self* this_ = (self*)arg;
	this_->_stop();
}

void tcp_client::_start()
{
	int flags = BEV_OPT_CLOSE_ON_FREE |BEV_OPT_THREADSAFE;
	bufferevent* bev_ptr = bufferevent_socket_new(m_io_base, -1, 
		flags  ); 
	m_bev.reset( bev_ptr , bufferevent_free ); 
	assert( bev_ptr && "initialize bufferevent_socket_new error ");
	bufferevent_setcb(bev_ptr, _read_cb, _write_cb, _error_cb, this);
	bufferevent_enable(bev_ptr, EV_READ|EV_WRITE|EV_PERSIST);
	bufferevent_socket_connect( bev_ptr , (const sockaddr *)&m_sockaddr , sizeof(m_sockaddr) );
}

void tcp_client::_stop()
{
	m_need_stop = true;
	stop_timer();
	evtimer_del(m_reconnect_ev.get());
	m_bev.reset();
}




}
