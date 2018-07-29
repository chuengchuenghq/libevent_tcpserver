
#include "include.h"
#include <fstream>
#include "tcp_server.h"

using libevent2::tcp_server;
using libevent2::event_base_pool;
using libevent2::tcp_session;

static int get_id()
{
	return base::ThisThread::current_thread_id();
}

static int g_port = 12345;
static string g_filename;
static  int g_play_times = 1;
static  int g_speed = 1000000;
static  int g_heartbeart = 0;


struct FileReplaySession: public tcp_session
{
	virtual void callback_err(const std::string& msg)
	{
		printf("thread_id=[%d] fd=[%d] err=[%s]\n" , get_id() ,this->get_fd() , msg.c_str() );
	}

	virtual void callback_timer()
	{
		if(m_need_close) 
		{
			 
			if(get_output_buffer_size() == 0)
				this->close(); 
			return;
		}

		int send_bytes = 0;
		while( send_bytes < m_temp_buf.size() )
		{
			 
			if(get_output_buffer_size() > m_temp_buf.size() * 10)
				break;

			m_file.read( &m_temp_buf[0] , m_temp_buf.size() );
			int cnt =  m_file.gcount();

			if(cnt == 0)
			{
				m_current_times += 1;
				if(m_play_times > 0 && m_current_times >= m_play_times)
				{
					m_need_close = true;
					return; 
				}

				m_file.clear();
				m_file.seekg(0, std::ios::beg);
				continue;
			}

			this->write( &m_temp_buf[0] , cnt );
			send_bytes += cnt;
		}
	}

	virtual void callback_init()
	{
		m_file.open( g_filename.c_str() , std::ios::binary );
		m_speed = g_speed;
		m_play_times = g_play_times;
		m_current_times = 0;

		int buf_size = 1024*1024;
		double timer_elapse = 1.0;
		if( m_speed < buf_size )
		{
			timer_elapse = 1.0;
			buf_size = m_speed;
		}else
		{
			timer_elapse = buf_size/double(m_speed);
		}

		m_temp_buf.resize(buf_size);
		this->set_timer_interval(timer_elapse); 
		m_need_close = false;
		start_timer();
	}

private:
	std::ifstream m_file;
	int m_speed;
	int m_play_times;
	int m_current_times;
	std::vector<char> m_temp_buf;
	bool m_need_close;
};


static
void new_connection_callback(FileReplaySession* session)
{
	printf("thread_id=[%d] new connection fd=[%d] addr=[%s:%d]\n" , 
		get_id() ,session->get_fd(),
		session->get_ip().c_str(),
		session->get_port()
		);
}

static 
void print_session_info(tcp_session* pSession)
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


TASK_INIT(libevent2 , FileReplaySession)
{
	g_filename = GetConsoleString("send file name:");
	OUTPUT( g_filename);
	OUTPUT( g_port);
	OUTPUT( g_play_times);
	OUTPUT( g_speed);	
	OUTPUT( g_heartbeart);

	tcp_server<FileReplaySession> server;
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
