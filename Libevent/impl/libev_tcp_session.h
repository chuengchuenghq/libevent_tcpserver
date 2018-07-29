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
#pragma once
#include "tcp_server.h"
#include "circular_buffer.h"

namespace libevent2
{
	class LIBEV_DLL_EXPORT libev_tcp_session :
												public tcp_session
	{
	public:
		libev_tcp_session();
		virtual ~libev_tcp_session();

		virtual int callback_read(int fd, const char* buf, int len); 

		virtual void new_packet_notify(int fd, const char* buf, int len);

		virtual void callback_timer();

		virtual void callback_init();

	private:
		virtual void get_shake_hand_packet(vector<char> & vectData);

		virtual void send_shake_hand();

		bool m_shake_hand;

		libevent2::circular_buffer m_ibuf;  
		std::shared_ptr<char> m_buf;

		bool m_init_shake_hand = true;
	};
}