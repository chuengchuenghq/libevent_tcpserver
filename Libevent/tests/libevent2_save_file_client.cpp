#include <fstream>
#include "include.h"


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

static string g_ip = "127.0.0.1";
static unsigned short g_port = 12345;
static double g_reconnect = 1.0;
static bool g_auto_reconnect = true;
static string g_filename;


struct save_file_client : tcp_client
{
	virtual void callback_err(const std::string& msg)
	{
		printf("thread_id=[%d] fd=[%d] err=[%s]\n" , get_id() ,this->get_fd() , msg.c_str() );
		this->stop();
	}

	virtual int callback_read(const char* buf , int len)
	{
		if(!m_file.is_open())
		{
			m_file.open(g_filename.c_str() , std::ios::binary);
		}
		m_file.write( buf , len);
		m_file.flush();
		return len;
	}

	virtual void callback_connected()
	{
		printf("thread_id=[%d] fd=[%d] connectted Success\n" , get_id() ,this->get_fd()  );
	}

	virtual void callback_write()
	{
		printf("thread_id=[%d] fd=[%d] total write bytes=[%d]\n" , get_id() ,this->get_fd() , this->total_write_bytes() );
	}

private:
	std::ofstream m_file;
};

TASK_INIT(libevent2 , save_file_client)
{
	g_filename = GetConsoleString("save file name:");
	OUTPUT( g_ip);
	OUTPUT( g_port);
	OUTPUT( g_auto_reconnect );
	OUTPUT( g_reconnect );
	OUTPUT( g_filename );

	event_base_pool io_pool(1);
	save_file_client client;

	client.init(io_pool.get_service());
	client.set_auto_reconnect(g_auto_reconnect);
	client.set_reconnect_interval(g_reconnect);
	client.set_host_info(g_ip.c_str() , g_port);
	client.start();
	io_pool.run(0);
}


