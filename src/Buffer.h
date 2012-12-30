#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class Buffer {
	private:
		char* m_Buffer;
		char* m_Temp;
		
		unsigned int m_Size;
	public:
		Buffer();
		~Buffer();
		
		char* getBuffer();
		
		int getSize();
		
		// Adders
		void addByte(char cByte);
		void addWord(short nBytes);
		void addDWord(int nBytes);
		void addQWord(long nBytes);
		void addString(const char* pString);
		void addNTString(const char* pString);
		void copyMem(const char* pData, unsigned short nLength);
};

#endif /* __BUFFER_H__ */

