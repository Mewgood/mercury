#ifndef __BYTEREADER_H__
#define __BYTEREADER_H__

#include <string.h>

namespace ByteReader
{
	unsigned char readByte(int nOffset, char* pData);
	unsigned short readWord(int nOffset, char* pData);
	unsigned int readDWord(int nOffset, char* pData);
	unsigned long long readQWord(int nOffset, char* pData);
	char* readString(int nOffset, char* pData);					// Will only stop at null-terminator '\0'.
}

#endif /* __BYTEREADER_H__ */
