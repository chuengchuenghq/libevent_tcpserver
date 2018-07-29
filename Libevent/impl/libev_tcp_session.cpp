#include "libev_tcp_session.h"
#include "tcp_packet.h"

#include "log.h"
#include "Pack.h"

using namespace libevent2;

namespace libevent2
{
	libev_tcp_session::libev_tcp_session() :
		m_ibuf(1024 * 1024)
		, m_buf(new char[TCP_BUFSIZE_READ + 1], [](char *p) {delete[]p; })
		, m_shake_hand(false)
	{
	}


	libev_tcp_session::~libev_tcp_session()
	{
	}

	int libev_tcp_session::callback_read(int fd, const char* buf, int len)
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

			if (intPackLen < CPack::_const_pack_head_size)
			{
				 
				return -1;
			}
			else if (intPackLen <= len)
			{
				if (76 == shtPackType)
				{
					libevent2::tcp_packet packet;
					packet.resize(intPackLen);
					m_ibuf.Read((char*)packet.contents(), intPackLen);

					m_shake_hand = true;
					 
					consume += intPackLen;
				}
				else
				{
					libevent2::tcp_packet packet;
					packet.resize(intPackLen);
					m_ibuf.Read((char*)packet.contents(), intPackLen);

					new_packet_notify(fd, (char*)packet.contents(), intPackLen);

					consume += intPackLen;
				}
			}
			else
			{
				break;
			}
		}

		return consume;
	}

	void libev_tcp_session::callback_init()
	{
		this->set_timer_interval(30); 

		start_timer();
	}

	void libev_tcp_session::new_packet_notify(int fd, const char* buf, int len)
	{
		 
	}

	void libev_tcp_session::callback_timer()
	{
		if (m_init_shake_hand == false)
		{
			if (m_shake_hand == false)
			{
				 
				__close();
				EM_LOG_ERROR("shake hand timeout, close socket.");
				return;
			}
		}

		if (m_init_shake_hand) m_init_shake_hand = false;

		send_shake_hand();
	}

	void libev_tcp_session::get_shake_hand_packet(vector<char> & vectData)
	{
		static UINT64 packetNo = 0;

		vectData.resize(CPack::_const_pack_head_size + sizeof(int) * 2);
		int intLen = vectData.size();
		CPack::Pack2(&*vectData.begin(), intLen, ++packetNo, 100, 76, 0, 0);
	}


	void libev_tcp_session::send_shake_hand()
	{
		vector<char> vectBuf;

		get_shake_hand_packet(vectBuf);
		this->write(&*vectBuf.begin(), vectBuf.size());

		m_shake_hand = false;

		EM_LOG_STATUS("Send shake hand{interval=30,recvSize=%ld}",m_ibuf.GetLength());
	}

}