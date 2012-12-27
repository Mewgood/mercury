#include "ChatProtocol.h"

ChatProtocol::ChatProtocol()
{
	mSToken = 0;
	mCToken = 0;
	mFileTime = 0;
	
	// Set pointers to null
	mFileName = 0x0;
	mValueString = 0x0;
}

ChatProtocol::~ChatProtocol()
{
	// Delete memory if allocated
	if (mFileName)
		delete [] mFileName;
	
	if (mValueString)
		delete [] mValueString;
}

bool ChatProtocol::sendProto()
{
	// Send required byte of 0x01 to indicate protocol
	int tmp = _send("\x01", 1);
	
	printf("Sent %u of expected %u bytes\n", tmp, 1);
	
	if (tmp != 1)
		return false;
	
	return true;
}

bool ChatProtocol::sendSIDAUTHINFO()
{
	unsigned short nSize = (4 + (4*9) + 4 + 14); // Header, 9 uint32's, usa\0, united states\0, 58 bytes
	Buffer buf;
	
	// Header \xFF\x50\x00\x3A
	buf.addByte(0xFF);
	buf.addByte(0x50);
	buf.addWord(nSize);
	
	// Body
	buf.addDWord(0x00);		// Protocol ID
	buf.addDWord('IX86');	// Platform ID
	buf.addDWord('D2XP');	// Product ID
	buf.addDWord(0x0D);		// Version byte
	buf.addDWord('enUS');	// Product lang
	buf.addDWord(0x00);		// Local IP, 0 appears to work
	buf.addDWord(0x00);		// Time zone, 0 works
	buf.addDWord(0x00);		// Locale
	buf.addDWord(0x00);		// Lang
	buf.addNTString("USA");
	buf.addNTString("United States");
	
	int tmp = _send(buf.getBuffer(), nSize);
	
	printf("Sent %u of expected %u bytes\n", tmp, nSize);
	
	return true;
}

bool ChatProtocol::parsePacket()
{
	// Receive packet, header only (4 bytes)
	char *pTemp = new char[4];
	int tmp = _recv(pTemp, 4);
	printf("Received %u of expected %u bytes\n", tmp, 4);
	
	if (tmp != 4) {
		printf("Error : received malformed packet header...\n");
		
		if (pTemp)
			delete [] pTemp;
		
		return false;
	}
	
	char cId = ByteReader::ReadByte(1, pTemp);
	unsigned short nLen = ByteReader::ReadWord(2, pTemp);
	
	delete [] pTemp;
	
	printf("Packet : 0x%02X > Length : %u\n", cId, nLen);
	
	nLen -= 4; // Removing packet header from length of 4 bytes, nLen contains the remaing packet size
	
	pTemp = new char[nLen];
	tmp = _recv(pTemp, nLen);
	
	printf("Received %u of expected %u bytes\n", tmp, nLen);
	
	if (tmp != nLen){
		printf("Error : received malformed packet body... >> 0x%02X\n", cId);
		
		if (pTemp)
			delete [] pTemp;
		
		return false;
	}
	
	Buffer buf;
	// Switch packet id parse as necessary...
	switch (cId)
	{
		case 0x25:
			{
				buf.addByte(0xFF);
				buf.addByte(0x25);
				buf.addWord(nLen + 4);
				buf.addString(pTemp);
				
				tmp = _send(buf.getBuffer(), nLen + 4);
				printf("Sent %u of expected %u bytes\n", tmp, (nLen+4));
				if (tmp != (nLen+4)){
					printf("Error : sent malformed packet... >> 0x25\n");
					return false;
				}
				
				break;
			}
		case 0x50:
		{
			unsigned int logontype = ByteReader::ReadDWord(0, pTemp);
			
			if (logontype != 0x0) {
				printf("Error : incorrect logon type, also this error should never happen...\n");
				return false;
			}
			
			unsigned int servertoken = ByteReader::ReadDWord(4, pTemp);
			unsigned long filetime = ByteReader::ReadQWord(8, pTemp);
			
			// Create pointers to, and calculate string lengths
			char *filename = ByteReader::ReadString(16, pTemp);
			unsigned int filename_len = strlen(filename) + 1;
			
			char *valuestr = ByteReader::ReadString(16+filename_len, pTemp);
			unsigned int valuestr_len = strlen(valuestr) + 1;
			
			mSToken = servertoken;
			mFileTime = filetime;
			
			mFileName = new char[filename_len];
			mValueString = new char[valuestr_len];
			
			strcpy(mFileName, filename);
			strcpy(mValueString, valuestr);
			
			break;
		}
	}
	
	if (pTemp)
		delete [] pTemp;
	
	return true;
}
