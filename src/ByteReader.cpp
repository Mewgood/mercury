#include "ByteReader.h"

namespace ByteReader
{
	unsigned char readByte(int nOffset, char* pData)
	{
		return *(char*)(pData + nOffset);
	}

	unsigned short readWord(int nOffset, char* pData)
	{
		return *(unsigned short*)(pData + nOffset);
	}

	unsigned int readDWord(int nOffset, char* pData)
	{
		return *(unsigned int*)(pData + nOffset);
	}
	
	unsigned long long readQWord(int nOffset, char* pData)
	{
		return *(unsigned long long*)(pData + nOffset);
	}

	char* readString(int nOffset, char* pData)
	{
		return (pData + nOffset);
	}
};

