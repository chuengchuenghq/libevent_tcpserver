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
 
#include <string>
#include <memory>
#include "event_base_pool.h"

namespace libevent2
{
	class  LIBEV_DLL_EXPORT circular_buffer
	{
	public:
		circular_buffer(size_t size);
		~circular_buffer();


		//追加L个字节
		bool Write(const char *p, size_t l);

		//拷贝l字节到dest
		bool Read(char *dest, size_t l);

		//拷贝l个字节，不移动缓冲区指针
		bool SoftRead(char *dest, size_t l);

		//清除l个字节
		bool Remove(size_t l);

		//读取l个字节，并以string返回
		std::string ReadString(size_t l);

		//缓冲区长度
		size_t GetLength();
		//缓冲区开始地址
		const char *GetStart();

		//返回从缓冲区开始到物理地址结束长度
		size_t GetL();

		//返回缓冲区空闲大小
		size_t Space();

		//返回写入字节数
		unsigned long ByteCounter(bool clear = false);

	private:
		circular_buffer(const circular_buffer& s) {}
		circular_buffer& operator=(const circular_buffer&) { return *this; }

		std::shared_ptr<char> buf;
		size_t m_max;
		size_t m_q;
		size_t m_b;
		size_t m_t;
		unsigned long m_count;
	};
}