#include "circular_buffer.h"
#include <string.h>

 
namespace libevent2
{
	circular_buffer::circular_buffer(size_t size)
		: buf(new char[2 * size], [](char *p) {delete[]p; })
		, m_max(size)
		, m_q(0)
		, m_b(0)
		, m_t(0)
		, m_count(0)
	{
	}


	circular_buffer::~circular_buffer()
	{
		//delete[] buf;
	}


	bool circular_buffer::Write(const char *s, size_t l)
	{
		if (m_q + l > m_max)
		{
			//EM_NEW_LOG_ERROR(L"write buffer overflow");
			return false; // overflow
		}
		m_count += (unsigned long)l;
		if (m_t + l > m_max) // block crosses circular border
		{
			// size left until circular border crossing
			size_t l1 = m_max - m_t;
			// ¿½±´ buffer(buf) + top pointer(m_t)
			memcpy(buf.get() + m_t, s, l);
			memcpy(buf.get() , s + l1, l - l1);
			m_t = l - l1;
			m_q += l;
		}
		else
		{
			memcpy(buf.get() + m_t, s, l);
			memcpy(buf.get() + m_max + m_t, s, l);
			m_t += l;
			if (m_t >= m_max)
				m_t -= m_max;
			m_q += l;
		}
		return true;
	}


	bool circular_buffer::Read(char *s, size_t l)
	{
		if (l > m_q)
		{
			//EM_NEW_LOG_ERROR(L"attempt to read beyond buffer.");
			return false; // not enough chars
		}
		if (m_b + l > m_max) // block crosses circular border
		{
			size_t l1 = m_max - m_b;
			if (s)
			{
				memcpy(s, buf.get() + m_b, l1);
				memcpy(s + l1, buf.get(), l - l1);
			}
			m_b = l - l1;
			m_q -= l;
		}
		else
		{
			if (s)
			{
				memcpy(s, buf.get() + m_b, l);
			}
			m_b += l;
			if (m_b >= m_max)
				m_b -= m_max;
			m_q -= l;
		}
		if (!m_q)
		{
			m_b = m_t = 0;
		}
		return true;
	}

	bool circular_buffer::SoftRead(char *s, size_t l)
	{
		if (l > m_q)
		{
			return false;
		}
		if (m_b + l > m_max)                          // block crosses circular border
		{
			size_t l1 = m_max - m_b;
			if (s)
			{
				memcpy(s, buf.get() + m_b, l1);
				memcpy(s + l1, buf.get(), l - l1);
			}
		}
		else
		{
			if (s)
			{
				memcpy(s, buf.get() + m_b, l);
			}
		}
		return true;
	}

	bool circular_buffer::Remove(size_t l)
	{
		return Read(NULL, l);
	}


	size_t circular_buffer::GetLength()
	{
		return m_q;
	}


	const char *circular_buffer::GetStart()
	{
		return buf.get() + m_b;
	}


	size_t circular_buffer::GetL()
	{
		return (m_b + m_q > m_max) ? m_max - m_b : m_q;
	}


	size_t circular_buffer::Space()
	{
		return m_max - m_q;
	}


	unsigned long circular_buffer::ByteCounter(bool clear)
	{
		if (clear)
		{
			unsigned long x = m_count;
			m_count = 0;
			return x;
		}
		return m_count;
	}


	std::string circular_buffer::ReadString(size_t l)
	{
		char *sz = new char[l + 1];
		if (!Read(sz, l)) // failed, debug printout in Read() method
		{
			delete[] sz;
			return "";
		}
		sz[l] = 0;
		std::string tmp = sz;
		delete[] sz;
		return tmp;
	}
}