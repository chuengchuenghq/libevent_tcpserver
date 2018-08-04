# libevent_tcpserver
dependency :  libevent , boost
if you want to build this project successfully , you have to install&build those two projects at first.

测试代码在目录：./Libevent\tests

简单的服务器示例代码：
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


int main()
{
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

客户端示例代码：
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

int main()
{
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
    
    return 0;
}


