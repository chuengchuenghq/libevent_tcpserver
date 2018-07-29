#include "libev_tcp_client_session.h"

namespace libevent2
{
	libev_tcp_client_session::libev_tcp_client_session() 	:
															m_ibuf(1024 * 1024 * 20)
															, m_buf(new char[TCP_BUFSIZE_READ + 1], [](char *p) {delete[]p; })
															, m_shake_hand(false)
	{
	}
 

	libev_tcp_client_session::~libev_tcp_client_session()
	{
	}

	void libev_tcp_client_session::callback_err(const std::string& msg)
	{
		 
	}

	int libev_tcp_client_session::callback_read(const char* buf, int len)
	{
		int consume = 0;


		if (len > 0 && len <= TCP_BUFSIZE_READ)
		{
			if (!m_ibuf.Write(buf, len))
			{
				EM_LOG_ERROR("Receive ibuf error.");
			}
		}
		else
		{
			int err = GetLastError();
			EM_LOG_ERROR("tcp buffer size ibuf, received bytes=%ld, error=%ld.", len, err);
			return -1;
		}


		while (m_ibuf.GetLength() >= CPack::_const_pack_head_size)
		{
			SPackHead head;
			unsigned int intPackLen = 0;

			char head_buf[12] = { 0 };
			memset(head_buf, 0, 12);

			m_ibuf.SoftRead(head_buf, CPack::_const_pack_head_size);

			CPack::Input(head_buf, intPackLen);

			unsigned short shtPackType = 0;
			CPack::Input(head_buf + 10, shtPackType);


			EM_LOG_STATUS("buf size=%d,shtpacktype =%d", m_ibuf.GetLength(), shtPackType);

			if (intPackLen < CPack::_const_pack_head_size)
			{
				EM_LOG_STATUS("intPackLen < CPack::_const_pack_head_size{intPackLen=%d}", intPackLen);
				return -1;
			}
			else if (intPackLen <= len)
			{
				if (76 == shtPackType)
				{
					libevent2::tcp_packet packet;
					packet.resize(intPackLen);
					m_ibuf.Read((char*)packet.contents(), intPackLen);

					consume += intPackLen;
				}
				else
				{
					libevent2::tcp_packet packet;
					packet.resize(intPackLen);
					m_ibuf.Read((char*)packet.contents(), intPackLen);

					new_packet_notify((char*)packet.contents(), intPackLen);

					consume += intPackLen;
				}
			}
		}

		return consume;

	}

	void libev_tcp_client_session::callback_connected()
	{
		 
 
		 
	}

	void libev_tcp_client_session::callback_write()
	{
		 
	}

	void libev_tcp_client_session::new_packet_notify(const char* buf, int len)
	{
		this->write(buf, len);
	}
}