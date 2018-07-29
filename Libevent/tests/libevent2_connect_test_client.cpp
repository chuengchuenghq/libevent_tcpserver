#include "include.h"
#include "Thread.h"


#include "tcp_client.h"
#include "query_timer.h"
#include "event_base_pool.h"

#include <string>

using libevent2::event_base_pool;
using libevent2::tcp_client;
using namespace std;


static int get_id()
{
	return base::ThisThread::current_thread_id();
}

extern std::vector<tcp_client*> g_clientlist; 
extern int g_dispatcher_count;
extern int g_num;
extern string g_ip ;
extern unsigned short g_port;
extern double g_reconnect;
extern bool g_auto_reconnect;
extern int g_bytes ;

struct connect_test_client : tcp_client
{
	virtual void callback_err(const std::string& msg)
	{
		printf("thread_id=[%d] fd=[%d] err=[%s]\n" , get_id() ,this->get_fd() , msg.c_str() );
	}

	virtual void callback_connected()
	{
		printf("thread_id=[%d] fd=[%d] connectted Success\n" , get_id() ,this->get_fd()  );

		int bytes = g_bytes;
		if( bytes <= 0)
			bytes = 1;
		
		std::vector<char> buf(bytes);
		for(int i = 0 ; i < bytes; ++i)
			buf[i] = i;
		this->write( &buf[0] , buf.size());
	}

	virtual void callback_write()
	{
		this->restart();
	}

};

TASK_INIT(libevent2 , connect_test_client)
{

	OUTPUT( g_dispatcher_count );
	OUTPUT( g_num);
	OUTPUT( g_ip);
	OUTPUT( g_port);
	OUTPUT( g_bytes);
	OUTPUT( g_auto_reconnect );
	OUTPUT( g_reconnect );

	event_base_pool io_pool(g_dispatcher_count);
	io_pool.start();

	for(int i = 0 ; i< g_num; ++i)
	{
		connect_test_client* pClient = new connect_test_client;
		pClient->init(io_pool.get_service());
		pClient->set_auto_reconnect(g_auto_reconnect);
		pClient->set_host_info(g_ip.c_str() , g_port);
		pClient->set_reconnect_interval(g_reconnect);
		g_clientlist.push_back(pClient);
	}

	for(int i = 0 ; i< g_num; ++i)
	{
		g_clientlist[i]->start();
		
	}

	while(true)
	{
		bool need_break = true;
		for(int i = 0 ; i< g_num; ++i)
			if(!g_clientlist[i]->is_connected() )
				need_break = false;

		if(need_break)
			break;

		base::ThisThread::yield();
	}


	while(true)
	{
		printf("main thread id=[%d] ", get_id() );
		string ret = GetConsoleString("-1 to Esc;-2 show traffic;\n");
		if( ret == "-1")
			break;
	}

	for(int i = 0 ;i < g_num; ++i)
		delete g_clientlist[i];
}


