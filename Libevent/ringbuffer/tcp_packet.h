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
#include "byte_buffer.h"
#include "event_base_pool.h"

#define  TCP_BUFSIZE_READ 1024*1024*3

namespace libevent2
{

	class LIBEV_DLL_EXPORT tcp_packet :
		public byte_buffer
	{
	public:
		// just container for later use
		tcp_packet() : byte_buffer(0), m_opcode(0)
		{
		}
		explicit tcp_packet(uint16 opcode, size_t res = 200) : byte_buffer(res), m_opcode(opcode) { }
		// copy constructor
		tcp_packet(const tcp_packet &packet) : byte_buffer(packet), m_opcode(packet.m_opcode)
		{
		}

		void Initialize(uint16 opcode, size_t newres = 200)
		{
			clear();
			_storage.reserve(newres);
			m_opcode = opcode;
		}

		uint16 GetOpcode() const { return m_opcode; }
		void SetOpcode(uint16 opcode) { m_opcode = opcode; }

	protected:
		uint16 m_opcode;
	};
}