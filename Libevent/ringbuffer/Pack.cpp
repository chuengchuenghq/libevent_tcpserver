 
#include "pack.h"

namespace libevent2
{
	CPack::ErrorCode
		CPack::PackHead(char *pOutPackHead
			, int & nOutPackHeadSize
			, const int intPackSize
			, const int intPackNo
			, const unsigned short shtVersion
			, const unsigned short shtType
		)
	{

		Output(pOutPackHead + 0, intPackSize);
		Output(pOutPackHead + 4, intPackNo);
		Output(pOutPackHead + 8, shtVersion);
		Output(pOutPackHead + 10, shtType);

		nOutPackHeadSize = intPackSize;
		return Err_NoErr;
	};

	CPack::ErrorCode
		CPack::UnpackHead(int & intOutPackSize
			, int & intPackNo
			, unsigned short & shtVersion
			, unsigned short & shtType
			, const char *pchPack
			, const int intPackSize
		)
	{


		Input(pchPack + 0, intOutPackSize);
		Input(pchPack + 4, intPackNo);
		Input(pchPack + 8, shtVersion);
		Input(pchPack + 10, shtType);
		return Err_NoErr;
	};

	CPack::ErrorCode
		CPack::PackHead(char *pOutPackHead
			, int & nOutPackHeadSize
			, const SPackHead *pHead
		)
	{
		return PackHead(pOutPackHead, nOutPackHeadSize, pHead->m_intPackSize, pHead->m_intPackNo, pHead->m_shtVer, pHead->m_shtType);
	};

	CPack::ErrorCode
		CPack::UnpackHead(SPackHead *pHead
			, const char *pInPack
			, const int nInPackSize
		)
	{
		return UnpackHead(pHead->m_intPackSize, pHead->m_intPackNo, pHead->m_shtVer, pHead->m_shtType, pInPack, nInPackSize);
	};


	int CPack::SafeUnpackString(const char * pBuf, const int intLen, string & str)
	{
		unsigned short size = 0;
		int intTmp = intLen;
		if (intTmp < sizeof(size))
		{
			return -1;
		}

		intTmp -= Input(pBuf, size);
		if (intTmp - size < 0)
		{
			return -1;
		}

		vector<char> tmp;
		tmp.resize(size + 1);
		memcpy(&*tmp.begin(), pBuf + sizeof(size), size);
		str = &*tmp.begin();
		intTmp -= size;
		return intLen - intTmp;
	}

	int CPack::UnpackStringList(const char * pBuf, const int intLen, vector<string> & data)
	{
		const char * pPos = pBuf;
		int intTmp = intLen;
		int intSize = 0;
		if (intTmp < sizeof(intSize))
		{
			return -1;
		}

		pPos += Input(pPos, intSize);
		for (int i = 0; i < intSize; i++)
		{
			string str;
			int intStrLen = 0;
			intStrLen = SafeUnpackString(pPos, intLen - (pPos - pBuf), str);
			if (intStrLen == -1)
			{
				return -1;
			}

			pPos += intStrLen;
			data.push_back(str);
		}

		return pPos - pBuf;
	}
}