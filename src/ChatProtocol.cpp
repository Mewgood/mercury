#include "ChatProtocol.h"

ChatProtocol::ChatProtocol()
{
	mAccount = 0;
	mPassword = 0;
	mKey = 0;
	mXKey = 0;
	
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
	if (mAccount)
		delete [] mAccount;
	
	if (mPassword)
		delete [] mPassword;
	
	if (mKey)
		delete [] mKey;
	
	if (mXKey)
		delete [] mXKey;
	
	if (mFileName)
		delete [] mFileName;
	
	if (mValueString)
		delete [] mValueString;
}

bool ChatProtocol::sendPacket(char cId, unsigned short nLength, char *pData)
{
	unsigned short len = 4 + nLength; // Header + Body // 0xFF 0xXX 0x.. 0x00 // Body...
	
	Buffer Packet;
	Packet.addByte(0xFF);
	Packet.addByte(cId);
	Packet.addWord(len);
	Packet.copyMem(pData, nLength);
	
	int tmp = _send(Packet.getBuffer(), len);
	
	if (tmp != len) {
		printf("[BNCS] 0x%02X | Sent %u of expected %u bytes\n", cId, tmp, len);
		return false;
	}
	
	printf("[BNCS] 0x%02X | Sent %u bytes\n", cId, len);
	
	return true;
}

bool ChatProtocol::setData(const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey)
{
	int len = strlen(sAccount);
	if (len < 1)
		return false;
	len++;
	mAccount = new char[len];
	strcpy(mAccount, sAccount);
	len = 0;
	
	int len = strlen(sPassword);
	if (len < 1)
		return false;
	len++;
	mPassword = new char[len];
	strcpy(mPassword, sPassword);
	len = 0;
	
	int len = strlen(sKey);
	if (len < 1)
		return false;
	len++;
	strcpy(mKey, sKey);
	len = 0;
	
	int len = strlen(sXKey);
	if (len < 1)
		return false;
	len++;
	strcpy(mXKey, sXKey);
	len = 0;
	
	return true;
}

bool ChatProtocol::sendProto()
{
	// Send required byte of 0x01 to indicate protocol
	int tmp = _send("\x01", 1);
	
	if (tmp != 1) {
		printf("[BNCS] 0x01 | Sent %u of expected %u bytes\n", tmp, 1);
		return false;
	}
	
	printf("[BNCS] 0x01 | Sent 1 byte\n");
		
	return true;
}

bool ChatProtocol::sendSIDAUTHINFO()
{	
	Buffer buf;
	
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
	
	return sendPacket(0x50, buf.getSize(), buf.getBuffer());
}

bool ChatProtocol::parsePacket()
{
	// Receive packet, header only (4 bytes)
	char *pTemp = new char[4];
	int tmp = _recv(pTemp, 4);
	
	if (tmp != 4) {
		printf("Error : received malformed packet header...\n");
		printf("Received %u of expected %u bytes\n", tmp, 4);
		
		if (pTemp)
			delete [] pTemp;
		
		return false;
	}
	
	char cId = ByteReader::readByte(1, pTemp);
	unsigned short nLen = ByteReader::readWord(2, pTemp);
	
	delete [] pTemp;
	
	printf("[BNCS] 0x%02X | Received header (%u bytes)\n", cId, tmp);
	
	nLen -= 4; // Removing packet header from length of 4 bytes, nLen contains the remaing packet size
	
	pTemp = new char[nLen];
	tmp = _recv(pTemp, nLen);
	
	if (tmp != nLen){
		printf("Error : received malformed packet body... >> 0x%02X\n", cId);
		printf("Received %u of expected %u bytes\n", tmp, nLen);
		
		if (pTemp)
			delete [] pTemp;
		
		return false;
	}
	
	printf("[BNCS] 0x%02X | Received %u bytes\n", cId, tmp);
	
	Buffer buf;
	// Switch packet id parse as necessary...
	switch (cId)
	{
		case 0x25:
			{
				sendPacket(0x25, 4, pTemp);				
				break;
			}
		case 0x50:
		{
			unsigned int logontype = ByteReader::readDWord(0, pTemp);
			
			if (logontype != 0x0) {
				printf("Error : incorrect logon type, also this error should never happen...\n");
				return false;
			}
			
			unsigned int servertoken = ByteReader::readDWord(4, pTemp);
			unsigned long filetime = ByteReader::readQWord(8, pTemp);
			
			// Create pointers to, and calculate string lengths
			char *filename = ByteReader::readString(16, pTemp);
			unsigned int filename_len = strlen(filename) + 1;
			
			char *valuestr = ByteReader::readString(16+filename_len, pTemp);
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
