#include "ByteReader.h"

namespace ByteReader
{
	unsigned char ReadByte(int nOffset, char* pData)
	{
		return *(char*)(pData + nOffset);
	}

	unsigned short ReadWord(int nOffset, char* pData)
	{
		return *(unsigned short*)(pData + nOffset);
	}

	unsigned int ReadDWord(int nOffset, char* pData)
	{
		return *(unsigned int*)(pData + nOffset);
	}
	
	unsigned long long ReadQWord(int nOffset, char* pData)
	{
		return *(unsigned long long*)(pData + nOffset);
	}

	char* ReadString(int nOffset, char* pData)
	{
		return (pData + nOffset);
	}
};

