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


		//׷��L���ֽ�
		bool Write(const char *p, size_t l);

		//����l�ֽڵ�dest
		bool Read(char *dest, size_t l);

		//����l���ֽڣ����ƶ�������ָ��
		bool SoftRead(char *dest, size_t l);

		//���l���ֽ�
		bool Remove(size_t l);

		//��ȡl���ֽڣ�����string����
		std::string ReadString(size_t l);

		//����������
		size_t GetLength();
		//��������ʼ��ַ
		const char *GetStart();

		//���شӻ�������ʼ�������ַ��������
		size_t GetL();

		//���ػ��������д�С
		size_t Space();

		//����д���ֽ���
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