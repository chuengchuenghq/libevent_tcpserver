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
/// #pragma once
#include "tcp_client.h"
#include "circular_buffer.h"
#include "tcp_packet.h"
#include "Pack.h"
#include "log.h"

using namespace libevent2;

namespace libevent2
{
	class LIBEV_DLL_EXPORT libev_tcp_client_session :
														public tcp_client
	{
	public:
		libev_tcp_client_session();
		virtual ~libev_tcp_client_session();

		virtual void callback_err(const std::string& msg);

		virtual int callback_read(const char* buf, int len);

		virtual void callback_connected();

		virtual void callback_write();

		virtual void new_packet_notify(const char* buf, int len);
	
	private:
		libevent2::circular_buffer m_ibuf;  
		std::shared_ptr<char> m_buf;

		bool m_shake_hand = true;
	};

}
