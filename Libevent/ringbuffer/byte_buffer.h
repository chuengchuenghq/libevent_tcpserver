
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
#pragma  once

#include "Types.h"
#include "assert.h"

#include <string>
#include <vector>
#include <map>

using namespace std;


namespace libevent2
{
	class  byte_buffer
	{
	public:
		const static size_t DEFAULT_SIZE = 0x1000;

		// constructor
		byte_buffer() : _rpos(0), _wpos(0)
		{
			_storage.reserve(DEFAULT_SIZE);
		}
		// constructor
		byte_buffer(size_t res) : _rpos(0), _wpos(0)
		{
			_storage.reserve(res);
		}
		// copy constructor
		byte_buffer(const byte_buffer &buf) : _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) { }

		void clear()
		{
			_storage.clear();
			_rpos = _wpos = 0;
		}

		template <typename T> void append(T value)
		{
			append((uint8 *)&value, sizeof(value));
		}
		template <typename T> void put(size_t pos, T value)
		{
			put(pos, (uint8 *)&value, sizeof(value));
		}

		byte_buffer &operator<<(bool value)
		{
			append<char>((char)value);
			return *this;
		}
		byte_buffer &operator<<(uint8 value)
		{
			append<uint8>(value);
			return *this;
		}
		byte_buffer &operator<<(uint16 value)
		{
			append<uint16>(value);
			return *this;
		}
		byte_buffer &operator<<(uint32 value)
		{
			append<uint32>(value);
			return *this;
		}
		byte_buffer &operator<<(uint64 value)
		{
			append<uint64>(value);
			return *this;
		}

		// signed as in 2e complement
		byte_buffer &operator<<(int8 value)
		{
			append<int8>(value);
			return *this;
		}
		byte_buffer &operator<<(int16 value)
		{
			append<int16>(value);
			return *this;
		}
		byte_buffer &operator<<(int32 value)
		{
			append<int32>(value);
			return *this;
		}
		byte_buffer &operator<<(int64 value)
		{
			append<int64>(value);
			return *this;
		}

		// floating points
		byte_buffer &operator<<(float value)
		{
			append<float>(value);
			return *this;
		}
		byte_buffer &operator<<(double value)
		{
			append<double>(value);
			return *this;
		}
		byte_buffer &operator<<(const std::string &value)
		{
			append((uint8 const *)value.c_str(), value.length());
			append((uint8)0);
			return *this;
		}
		byte_buffer &operator<<(const char *str)
		{
			append((uint8 const *)str, str ? strlen(str) : 0);
			append((uint8)0);
			return *this;
		}

		byte_buffer &operator >> (bool &value)
		{
			value = read<char>() > 0 ? true : false;
			return *this;
		}
		byte_buffer &operator >> (uint8 &value)
		{
			value = read<uint8>();
			return *this;
		}
		byte_buffer &operator >> (uint16 &value)
		{
			value = read<uint16>();
			return *this;
		}
		byte_buffer &operator >> (uint32 &value)
		{
			value = read<uint32>();
			return *this;
		}
		byte_buffer &operator >> (uint64 &value)
		{
			value = read<uint64>();
			return *this;
		}

		//signed as in 2e complement
		byte_buffer &operator >> (int8 &value)
		{
			value = read<int8>();
			return *this;
		}
		byte_buffer &operator >> (int16 &value)
		{
			value = read<int16>();
			return *this;
		}
		byte_buffer &operator >> (int32 &value)
		{
			value = read<int32>();
			return *this;
		}
		byte_buffer &operator >> (int64 &value)
		{
			value = read<int64>();
			return *this;
		}

		byte_buffer &operator >> (float &value)
		{
			value = read<float>();
			return *this;
		}
		byte_buffer &operator >> (double &value)
		{
			value = read<double>();
			return *this;
		}
		byte_buffer &operator >> (std::string& value)
		{
			value.clear();
			while (rpos() < size())                         // prevent crash at wrong string format in packet
			{
				char c = read<char>();
				if (c == 0)
					break;
				value += c;
			}
			return *this;
		}

		uint8 operator[](size_t pos)
		{
			return read<uint8>(pos);
		}

		size_t rpos()
		{
			return _rpos;
		};

		size_t rpos(size_t rpos_)
		{
			_rpos = rpos_;
			return _rpos;
		};

		size_t wpos()
		{
			return _wpos;
		}

		size_t wpos(size_t wpos_)
		{
			_wpos = wpos_;
			return _wpos;
		}

		template <typename T> T read()
		{
			T r = read<T>(_rpos);
			_rpos += sizeof(T);
			return r;
		};
		template <typename T> T read(size_t pos) const
		{
			assert(pos + sizeof(T) <= size() || PrintPosError(false, pos, sizeof(T)));
			return *((T const*)&_storage[pos]);
		}

		void read(uint8 *dest, size_t len)
		{
			assert(_rpos + len <= size() || PrintPosError(false, _rpos, len));
			memcpy(dest, &_storage[_rpos], len);
			_rpos += len;
		}

		const uint8 *contents() const { return &_storage[0]; }

		size_t size() const { return _storage.size(); }
		bool empty() const { return _storage.empty(); }

		void resize(size_t newsize)
		{
			_storage.resize(newsize);
			_rpos = 0;
			_wpos = size();
		};
		void reserve(size_t ressize)
		{
			if (ressize > size()) _storage.reserve(ressize);
		};

		void append(const std::string& str)
		{
			append((uint8 const*)str.c_str(), str.size() + 1);
		}
		void append(const char *src, size_t cnt)
		{
			return append((const uint8 *)src, cnt);
		}
		void append(const uint8 *src, size_t cnt)
		{
			if (!cnt) return;

			assert(size() < 10000000);

			if (_storage.size() < _wpos + cnt)
				_storage.resize(_wpos + cnt);
			memcpy(&_storage[_wpos], src, cnt);
			_wpos += cnt;
		}
		void append(const byte_buffer& buffer)
		{
			if (buffer.size()) append(buffer.contents(), buffer.size());
		}

		void appendPackGUID(uint64 guid)
		{
			size_t mask_position = wpos();
			*this << uint8(0);
			for (uint8 i = 0; i < 8; i++)
			{
				if (guid & 0xFF)
				{
					_storage[mask_position] |= (1 << i);
					*this << ((uint8)(guid & 0xFF));
				}

				guid >>= 8;
			}
		}

		void put(size_t pos, const uint8 *src, size_t cnt)
		{
			assert(pos + cnt <= size() || PrintPosError(true, pos, cnt));
			memcpy(&_storage[pos], src, cnt);
		}
		void print_storage()
		{

			//EM_NEW_LOG_INFO(L"STORAGE_SIZE: %u", size());
			for (uint32 i = 0; i < size(); i++);
				//EM_NEW_LOG_STATUS(L"%u - ", read<uint8>(i));

		}

		void textlike()
		{
			//EM_NEW_LOG_STATUS(L"STORAGE_SIZE: %u", size());
			for (uint32 i = 0; i < size(); i++);
				//EM_NEW_LOG_STATUS(L"%c", read<uint8>(i));
			//EM_NEW_LOG_STATUS(L" ");
		}

		void hexlike()
		{
			uint32 j = 1, k = 1;
			//EM_NEW_LOG_STATUS(L"STORAGE_SIZE: %u", size());
			for (uint32 i = 0; i < size(); i++)
			{
				if ((i == (j * 8)) && ((i != (k * 16))))
				{
					if (read<uint8>(i) < 0x0F)
					{
						//EM_NEW_LOG_STATUS(L"| 0%X ", read<uint8>(i));
					}
					else
					{
						//EM_NEW_LOG_STATUS(L"| %X ", read<uint8>(i));
					}
					j++;
				}
				else if (i == (k * 16))
				{
					if (read<uint8>(i) < 0x0F)
					{
						//EM_NEW_LOG_STATUS(L"\n0%X ", read<uint8>(i));
					}
					else
					{
						//EM_NEW_LOG_STATUS(L"\n%X ", read<uint8>(i));
					}

					k++;
					j++;
				}
				else
				{
					if (read<uint8>(i) < 0x0F)
					{
						//EM_NEW_LOG_STATUS(L"0%X ", read<uint8>(i));
					}
					else
					{
						//EM_NEW_LOG_STATUS(L"%X ", read<uint8>(i));
					}
				}
			}
			//EM_NEW_LOG_STATUS(L" ");
		}

	protected:
		bool PrintPosError(bool add, size_t pos, size_t esize) const
		{
			//EM_NEW_LOG_ERROR(L"ERROR: Attempt %s in ByteBuffer (pos: %u size: %u) value with size: %u", (add ? "put" : "get"), pos, size(), esize);

			// assert must fail after function call
			return false;
		}

		size_t _rpos, _wpos;
		std::vector<uint8> _storage;
	};

	template <typename T> byte_buffer &operator<<(byte_buffer &b, std::vector<T> v)
	{
		b << (uint32)v.size();
		for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
		{
			b << *i;
		}
		return b;
	}

	template <typename T> byte_buffer &operator >> (byte_buffer &b, std::vector<T> &v)
	{
		uint32 vsize;
		b >> vsize;
		v.clear();
		while (vsize--)
		{
			T t;
			b >> t;
			v.push_back(t);
		}
		return b;
	}

	template <typename T> byte_buffer &operator<<(byte_buffer &b, std::list<T> v)
	{
		b << (uint32)v.size();
		for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++)
		{
			b << *i;
		}
		return b;
	}

	template <typename T> byte_buffer &operator >> (byte_buffer &b, std::list<T> &v)
	{
		uint32 vsize;
		b >> vsize;
		v.clear();
		while (vsize--)
		{
			T t;
			b >> t;
			v.push_back(t);
		}
		return b;
	}

	template <typename K, typename V> byte_buffer &operator<<(byte_buffer &b, std::map<K, V> &m)
	{
		b << (uint32)m.size();
		for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
		{
			b << i->first << i->second;
		}
		return b;
	}

	template <typename K, typename V> byte_buffer &operator >> (byte_buffer &b, std::map<K, V> &m)
	{
		uint32 msize;
		b >> msize;
		m.clear();
		while (msize--)
		{
			K k;
			V v;
			b >> k >> v;
			m.insert(make_pair(k, v));
		}
		return b;
	}
}