#include "Buffer.h"

Buffer::Buffer()
{
	// Set pointers and size to null
	m_Buffer = NULL;
	m_Temp = NULL;
	m_Size = 0;	
}

Buffer::~Buffer()
{
	if (m_Buffer)
		free(m_Buffer);
	if (m_Temp)
		free(m_Temp);
	m_Size = 0;
}

char* Buffer::getBuffer()
{
	return m_Buffer;
}

int Buffer::getSize()
{
	return m_Size;
}

void Buffer::addByte(char cByte)
{	
	if (m_Buffer) {
		if (m_Temp)
			free(m_Temp);
		
		// Copy old buffer into temp
		m_Temp = (char*)malloc(m_Size);
		memcpy(m_Temp, m_Buffer, m_Size);
		free(m_Buffer);
		
		// Recreate buffer with old size +1 byte, copy old buffer and new data into buffer
		m_Buffer = (char*)malloc(m_Size + 1);
		memcpy(m_Buffer, m_Temp, m_Size);
		memcpy(m_Buffer + m_Size, (void*)&cByte, 1);
	
		free(m_Temp);
		m_Temp = 0;
	} else {
		// Create buffer and copy new data
		m_Buffer = (char*)malloc(1);
		memcpy(m_Buffer, (void*)&cByte, 1);
	}
	
	m_Size++;
}

void Buffer::addWord(short nBytes)
{
	if (m_Buffer) {
		if (m_Temp)
			free(m_Temp);
		
		m_Temp = (char*)malloc(m_Size);
		memcpy(m_Temp, m_Buffer, m_Size);
		free(m_Buffer);
		
		m_Buffer = (char*)malloc(m_Size + 2);
		memcpy(m_Buffer, m_Temp, m_Size);
		memcpy(m_Buffer + m_Size, (void*)&nBytes, 2);
		
		free(m_Temp);
		m_Temp = 0;
	} else {
		m_Buffer = (char*)malloc(2);
		memcpy(m_Buffer, (void*)&nBytes, 2);
	}
	
	m_Size += 2;
}

void Buffer::addDWord(int nBytes)
{
	if (m_Buffer) {
		if (m_Temp)
			free(m_Temp);
		
		m_Temp = (char*)malloc(m_Size);
		memcpy(m_Temp, m_Buffer, m_Size);
		free(m_Buffer);
		
		m_Buffer = (char*)malloc(m_Size + 4);
		memcpy(m_Buffer, m_Temp, m_Size);
		memcpy(m_Buffer + m_Size, (void*)&nBytes, 4);
		
		free(m_Temp);
		m_Temp = 0;
	} else {
		m_Buffer = (char*)malloc(4);
		memcpy(m_Buffer, (void*)&nBytes, 4);
	}
	
	m_Size += 4;
}

void Buffer::addQWord(long nBytes)
{
	if (m_Buffer) {
		if (m_Temp)
			free(m_Temp);
		
		m_Temp = (char*)malloc(m_Size);
		memcpy(m_Temp, m_Buffer, m_Size);
		free(m_Buffer);
		
		m_Buffer = (char*)malloc(m_Size + 8);
		memcpy(m_Buffer, m_Temp, m_Size);
		memcpy(m_Buffer + m_Size, (void*)&nBytes, 8);
		
		free(m_Temp);
		m_Temp = 0;
	} else {
		m_Buffer = (char*)malloc(8);
		memcpy(m_Buffer, (void*)&nBytes, 8);
	}
	
	m_Size += 8;
}

void Buffer::addString(const char* pString)
{
	int len = strlen(pString);
	
	if (m_Buffer) {
		if (m_Temp)
			free(m_Temp);
		
		m_Temp = (char*)malloc(m_Size);
		memcpy(m_Temp, m_Buffer, m_Size);
		free(m_Buffer);
		
		m_Buffer = (char*)malloc(m_Size + len);
		memcpy(m_Buffer, m_Temp, m_Size);
		memcpy(m_Buffer + m_Size, (void*)pString, len);
		
		free(m_Temp);
		m_Temp = 0;
	} else {
		m_Buffer = (char*)malloc(len);
		memcpy(m_Buffer, (void*)pString, len);
	}
	
	m_Size += len;
}

void Buffer::addNTString(const char* pString)
{
	addString(pString);
	addByte(0x00);
}
