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
		printf("[%s] BNCS << 0x%02X | Sent %u of expected %u bytes\n", mAccount, cId, tmp, len);
		return false;
	}
	
	printf("[%s] BNCS << 0x%02X | Sent %u bytes\n", mAccount, cId, len);
	
	return true;
}

bool ChatProtocol::setData(const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey, unsigned int nCToken)
{
	int len = strlen(sAccount);
	if (len < 1)
		return false;
	len++;
	mAccount = new char[len];
	strcpy(mAccount, sAccount);
	len = 0;
	
	len = strlen(sPassword);
	if (len < 1)
		return false;
	len++;
	mPassword = new char[len];
	strcpy(mPassword, sPassword);
	len = 0;
	
	len = strlen(sKey);
	if (len < 1)
		return false;
	len++;
	mKey = new char[len];
	strcpy(mKey, sKey);
	len = 0;
	
	len = strlen(sXKey);
	if (len < 1)
		return false;
	len++;
	mXKey = new char[len];
	strcpy(mXKey, sXKey);
	len = 0;
	
	mCToken = nCToken;
	
	return true;
}

bool ChatProtocol::sendProto()
{
	// Send required byte of 0x01 to indicate protocol
	int tmp = _send("\x01", 1);
	
	if (tmp != 1) {
		printf("[%s] BNCS << 0x01 | Sent %u of expected %u bytes\n", mAccount, tmp, 1);
		return false;
	}
	
	printf("[%s] BNCS << 0x01 | Sent 1 byte\n", mAccount);
		
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
		printf("Error : malformed packet header...\n");
		printf("Received %u of expected %u bytes\n", tmp, 4);
		
		if (pTemp)
			delete [] pTemp;
		
		return false;
	}
	
	char cId = ByteReader::readByte(1, pTemp);
	unsigned short nLen = ByteReader::readWord(2, pTemp);
	
	delete [] pTemp;
	
	printf("[%s] BNCS >> 0x%02X | Received header (%u bytes)\n", mAccount, cId, tmp);
	
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
	
	printf("[%s] BNCS >> 0x%02X | Received %u bytes\n", mAccount, cId, tmp);
	
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
			unsigned long filetime = ByteReader::readQWord(12, pTemp);
			
			// Create pointers to, and calculate string lengths
			char *filename = ByteReader::readString(20, pTemp);
			unsigned int filename_len = strlen(filename) + 1;
			
			char *valuestr = ByteReader::readString(20+filename_len, pTemp);
			unsigned int valuestr_len = strlen(valuestr) + 1;
			
			mSToken = servertoken;
			mFileTime = filetime;
			
			mFileName = new char[filename_len];
			mValueString = new char[valuestr_len];
			
			strcpy(mFileName, filename);
			strcpy(mValueString, valuestr);
			
			// Generate executable checksum
			unsigned long nChecksum = 0;
			check_revision_result_type result = check_revision(mValueString, mFileName, "data/", nChecksum);
			
			switch (result) {
				case check_revision_result_success:
					printf("[%s] Generated executable checksum successfully. %lu\n", mAccount, nChecksum);
					mChecksum = nChecksum;
					break;
				case check_revision_result_file_error:
					printf("[%s] File error while generating checksum, make sure binaries are in data/ directory.\n", mAccount);
					return false;
				case check_revision_result_formula_error:
					printf("[%s] Formula error while generating checksum. %s\n", mAccount, mValueString);
					return false;
				case check_revision_result_mpq_error:
					printf("[%s] MPQ error while generating checksum. %s\n", mAccount, mFileName);
					return false;
				default:
					printf("[%s] Well this is bad, seriously failed to generate a checksum...\n", mAccount);
					return false;
			}
			
			// Hash cdkeys
			std::string d2hash, d2pub, xphash, xppub;
			
			// Classic
			if (!hash_d2key(mKey, mCToken, mSToken, d2hash, d2pub)){
				printf("[%s] Failed to hash D2DV CDKey. %s\nclient_token %u\nserver_token %u\n", mAccount, mKey, mCToken, mSToken);
				return false;
			}
			
			// Expansion
			if (!hash_d2key(mXKey, mCToken, mSToken, xphash, xppub)){
				printf("[%s] Failed to hash D2XP CDKey. %s\nclient_token %u\nserver_token %u\n", mAccount, mXKey, mCToken, mSToken);
				return false;
			}
			printf("[%s] Generated CDKey hashes sucessfully.\n", mAccount);
			
			// Copy hash values
			mKeyPublic = ByteReader::readDWord(0, const_cast<char*>(d2pub.data()));
			for (int x=0; x<5; x++){
				mKeyHash[x] = ByteReader::readDWord(x*4, const_cast<char*>(d2hash.data()));
			}
			
			// Repeat for expansion
			
			break;
		}
	}
	
	if (pTemp)
		delete [] pTemp;
	
	return true;
}
