#include "include.h"
#include "tcp_server.h"
 
#include <vector>
using namespace std;
 

using libevent2::tcp_server;
using libevent2::event_base_pool;
using libevent2::tcp_session;

static int get_id()
{
	return base::ThisThread::current_thread_id();
}

static	unsigned short g_port = 12345;
static  int g_heartbeart = 0;


struct EchoSession: public tcp_session
{
 
	virtual void callback_err(const std::string& msg)
	{
		printf("thread_id=[%d] fd=[%d] addr=[%s:%d] err=[%s]\n" , get_id() ,this->get_fd() , 
			get_ip().c_str(),
			get_port(),
			msg.c_str() );
	}

	virtual int callback_read(int fd,const char* buf , int len)
	{
		this->write(buf, len);
		 
		return len;
	}
 
 };

 
static
void new_connection_callback(EchoSession* session)
{
	printf("thread_id=[%d] new connection fd=[%d] addr=[%s:%d]\n" , 
		get_id() ,session->get_fd(),
		session->get_ip().c_str(),
		session->get_port()
		);
	
}

static 
void print_session_info(EchoSession* pSession)
{
	unsigned long long read_total = pSession->total_read_bytes();
	unsigned long long write_total = pSession->total_write_bytes();


	printf("client:[%s:%d] fd:[%d] read-bytes:%lld write-bytes:%lld\n" , 
		pSession->get_ip().c_str() , 
		pSession->get_port() ,
		pSession->get_fd(),
		read_total , write_total
		);
}


TASK_INIT(libevent2 , EchoSession)
{

	OUTPUT( g_port);
	OUTPUT( g_heartbeart);

	tcp_server<EchoSession> server;
	server.set_callback_new_connection(new_connection_callback);
	bool ret = server.listening(g_port);
	printf("listen port:%d %s\n" , g_port , ret?"Success":"Failure");
	server.set_session_heartbeat_elaspe(g_heartbeart);
	server.start();
	
 
	while(true)
	{
		string ret = GetConsoleString("-1 to Break , -2 to Show Clients , others continue\n");
		if( ret == "-1")
			break;
		else if( ret == "-2" )
		{
			printf("client information:\n");
			server.foreach_session(print_session_info);
			printf("client count=[%d]\n"  , server.session_count() );
		}
	}


}
