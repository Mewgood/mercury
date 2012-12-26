#ifndef __BYTEREADER_H__
#define __BYTEREADER_H__

#include <string.h>

namespace ByteReader
{
	unsigned char ReadByte(int nOffset, char* pData);
	unsigned short ReadWord(int nOffset, char* pData);
	unsigned int ReadDWord(int nOffset, char* pData);
	unsigned long long ReadQWord(int nOffset, char* pData);
	char* ReadString(int nOffset, char* pData);					// Will only stop at null-terminator '\0'.
}

#endif /* __BYTEREADER_H__ */
