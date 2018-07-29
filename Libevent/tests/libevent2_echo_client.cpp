#include "include.h"
#include "tcp_client.h"
#include "query_timer.h"
#include "event_base_pool.h"


using libevent2::event_base_pool;
using libevent2::tcp_client;
static int get_id()
{
	 
	return 1;
}


 std::vector<tcp_client*> g_clientlist; 
 int g_dispatcher_count = 1;
 int g_num = 1;
 string g_ip = "127.0.0.1";
 unsigned short g_port = 12345;
 double g_reconnect = 1.0;
 bool g_auto_reconnect = true;
 int g_bytes = 1024*4;

struct client_echo : tcp_client
{
	virtual void callback_err(const std::string& msg)
	{
		printf("thread_id=[%d] fd=[%d] err=[%s]\n" , get_id() ,this->get_fd() , msg.c_str() );
	}

	virtual int callback_read(const char* buf , int len)
	{
		this->write(buf,len);
		return len;
	}

	virtual void callback_connected()
	{
		printf("thread_id=[%d] fd=[%d] connectted Success\n" , get_id() ,this->get_fd()  );
		if( g_bytes <= 0)
			return;
		std::vector<char> buf(g_bytes);
		for(int i = 0 ; i < g_bytes; ++i)
			buf[i] = i;
		this->write( &buf[0] , buf.size());
	}

	virtual void callback_write()
	{
		
	}

};

TASK_INIT(libevent2 , client_echo)
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
		client_echo* pClient = new client_echo;
		pClient->init(io_pool.get_service());
		pClient->set_auto_reconnect(g_auto_reconnect);
		pClient->set_host_info(g_ip.c_str() , g_port);
		g_clientlist.push_back(pClient);
	}


	printf("wait all connectted , ...\n");
	base::time::query_timer timerit;
	
	for(int i = 0 ; i< g_num; ++i)
	{
		g_clientlist[i]->start();
		g_clientlist[i]->set_reconnect_interval(g_reconnect);
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

	
	double seconds = timerit.elaspe();
	printf("connectted %d clients , use seconds:%f\n" , g_num , seconds );
	
	timerit.restart();
	while(true)
	{
		printf("main thread id=[%d] ", get_id() );
		string ret = GetConsoleString("-1 to Esc;-2 show traffic;\n");
		if( ret == "-1")
			break;
		
		if( ret == "-2")
		{
			double seconds = timerit.elaspe();
			std::stringstream ss;
			for(int i = 0 ; i< g_num; ++i)
			{
				tcp_client* pClient = g_clientlist[i];
				
				unsigned long long read_total = pClient->total_read_bytes();
				unsigned long long write_total = pClient->total_write_bytes();
				ss <<"client-"<< i << " fd=["<< pClient->get_fd() <<"]"
					<<" read bytes:" << read_total
					<< "," << read_total/1000.0/seconds/1000.0 << "MB/s"
					<< " write bytes:" << write_total 
					<< "," << write_total/1000.0/seconds/1000.0 << "MB/s" << endl;
			}

			cout << ss.str() ;
		}

	}
	
	for(int i = 0 ;i < g_num; ++i)
		delete g_clientlist[i];
}


