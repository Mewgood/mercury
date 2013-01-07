#include "ChatProtocol.h"

ChatProtocol::ChatProtocol()
{
	mAccount = 0;
	mPassword = 0;
	mKey = 0;
	mXKey = 0;
	mExeInfo = 0;
	mOwner = 0;

	mSToken = 0;
	mCToken = 0;
	mFileTime = 0;

	// Set pointers to null
	mFileName = 0;
	mValueString = 0;

	mRUniqueName = 0;
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

	if (mExeInfo)
		delete [] mExeInfo;

	if (mOwner)
		delete [] mOwner;

	if (mFileName)
		delete [] mFileName;

	if (mValueString)
		delete [] mValueString;

	if (mRUniqueName)
		delete [] mRUniqueName;
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

bool ChatProtocol::genChecksum(std::string sDirectory)
{
	check_revision_result_type result = check_revision(mValueString, mFileName, sDirectory, mChecksum);

	switch (result) {
		case check_revision_result_success:
			printf("[%s] Generated executable checksum successfully. %lu\n", mAccount, mChecksum);
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
	return true;
}

bool ChatProtocol::genKeyHashes()
{
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

	// Copy hash values
	mKeyPublic = ByteReader::readDWord(0, const_cast<char*>(d2pub.data()));
	for (int x=0; x<5; x++){
		mKeyHash[x] = ByteReader::readDWord(x*4, const_cast<char*>(d2hash.data()));
	}

	// Repeat for expansion
	mXKeyPublic = ByteReader::readDWord(0, const_cast<char*>(xppub.data()));
	for (int x=0; x<5; x++){
		mXKeyHash[x] = ByteReader::readDWord(x*4, const_cast<char*>(xphash.data()));
	}

	printf("[%s] Generated CDKey hashes sucessfully.\n", mAccount);

	return true;
}

bool ChatProtocol::parseHashResult(char *pTemp)
{
	unsigned int result = ByteReader::readDWord(0, pTemp);

	switch (result)
	{
		case 0x000:
			printf("[%s] Passed hash challenge.\n", mAccount);
			break;
		case 0x100:
			printf("[%s] Old game version. %s\n", mAccount, ByteReader::readString(4, pTemp));
			return false;
		case 0x101:
			printf("[%s] Invalid version.\n", mAccount);
			return false;
		case 0x102:
			printf("[%s] Game version must be downgraded. %s\n", mAccount, ByteReader::readString(4, pTemp));
			return false;
		case 0x200:
			printf("[%s] Invalid CDKey (Classic), IPBan for 24hours...\n", mAccount);
			return false;
		case 0x201:
			printf("[%s] CDKey inuse by : %s (Classic)\n", mAccount, ByteReader::readString(4, pTemp));
			return false;
		case 0x202:
			printf("[%s] Banned CDKey. (Classic)\n", mAccount);
			return false;
		case 0x203:
			printf("[%s] Wrong product. (Classic)\n", mAccount);
			return false;
		case 0x210:
			printf("[%s] Invalid CDKey (Expansion), IPBan for 24hours...\n", mAccount);
			return false;
		case 0x211:
			printf("[%s] CDKey inuse by : %s (Expansion)\n", mAccount, ByteReader::readString(4, pTemp));
			return false;
		case 0x212:
			printf("[%s] Banned CDKey. (Expansion)\n", mAccount);
			return false;
		case 0x213:
			printf("[%s] Wrong product. (Expansion)\n", mAccount);
			return false;
		default:
			printf("[%s] Unknown error, code : 0x%03X\n", mAccount, result);
			return false;
	}
	return true;
}

bool ChatProtocol::parseLogonResult(char *pTemp)
{
	unsigned int result = ByteReader::readDWord(0, pTemp);

	switch (result)
	{
		case 0x00:
			printf("[%s] Logged on to account successfully.\n", mAccount);
			break;
		case 0x01:
			printf("[%s] Account does not exist.\n", mAccount);
			return false;
		case 0x02:
			printf("[%s] Invalid password.\n", mAccount);
			return false;
		case 0x06:
			printf("[%s] Account closed, %s\n", mAccount, ByteReader::readString(4, pTemp));
			return false;
	}

	return true;
}

bool ChatProtocol::parseLogonRealm(char *pTemp)
{
	unsigned int cookie, status, ip, port;
	unsigned int chunk1[2], chunk2[12];

	cookie = ByteReader::readDWord(0, pTemp);
	status = ByteReader::readDWord(4, pTemp);

	ip = ByteReader::readDWord(16, pTemp);

	//struct sockaddr_in sa;
	char sip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip, sip, INET_ADDRSTRLEN);

	port = ByteReader::readDWord(20, pTemp);
	unsigned short nport = ntohs(port);

	memcpy(&chunk1, pTemp+8, 8);
	memcpy(&chunk2, pTemp+24, 48);

	char *uni = ByteReader::readString(72, pTemp);
	unsigned int uni_len = strlen(uni)+1;

	mRUniqueName = new char[uni_len];
	strcpy(mRUniqueName, uni);

	// Set values
	mRCookie = cookie;
	mRStatus = status;

	strcpy(mRIp, sip);
	mRPort = nport;

	memcpy(mRChunk1, chunk1, 8);
	memcpy(mRChunk2, chunk2, 48);

	return true;
}

bool ChatProtocol::setData(const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey, unsigned int nCToken,
							const char *sExeInfo, const char *sOwner)
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

	len = strlen(sExeInfo);
	if (len < 1)
		return false;
	len++;
	mExeInfo = new char[len];
	strcpy(mExeInfo, sExeInfo);
	len = 0;

	len = strlen(sOwner);
	if (len < 1 || len > 15){  // Owner must be less then 16 characters including null terminator
		printf("[%s] Owner string to great, max 15 bytes. %u bytes\n", mAccount, len);
		return false;
	}
	len++;
	mOwner = new char[len];
	strcpy(mOwner, sOwner);

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
	Buffer Packet;

	// Body
	Packet.addDWord(0x00);		// Protocol ID
	Packet.addDWord('IX86');	// Platform ID
	Packet.addDWord('D2XP');	// Product ID
	Packet.addDWord(0x0D);		// Version byte
	Packet.addDWord('enUS');	// Product lang
	Packet.addDWord(0x00);		// Local IP, 0 appears to work
	Packet.addDWord(0x00);		// Time zone, 0 works
	Packet.addDWord(0x00);		// Locale
	Packet.addDWord(0x00);		// Lang
	Packet.addNTString("USA");
	Packet.addNTString("United States");

	return sendPacket(0x50, Packet.getSize(), Packet.getBuffer());
}

bool ChatProtocol::sendSIDAUTHCHECK()
{
	Buffer Packet;

	Packet.addDWord(mCToken);
	Packet.addDWord(0x0D030101);
	Packet.addDWord(mChecksum);
	Packet.addDWord(2);			// 2 CDKeys
	Packet.addDWord(0);

	// Classic
	Packet.addDWord(16);
	Packet.addDWord(0x06);
	Packet.addDWord(mKeyPublic);
	Packet.addDWord(0);
	for (int x=0; x<5; x++){
		Packet.addDWord(mKeyHash[x]);
	}

	// Expasion
	Packet.addDWord(16);
	Packet.addDWord(0x0A);
	Packet.addDWord(mXKeyPublic);
	Packet.addDWord(0);
	for (int x=0; x<5; x++){
		Packet.addDWord(mXKeyHash[x]);
	}

	Packet.addNTString(mExeInfo);
	Packet.addNTString(mOwner);

	return sendPacket(0x51, Packet.getSize(), Packet.getBuffer());
}

bool ChatProtocol::sendSIDLOGONRESPONSE()
{
	// Hash password
	std::string pass = nil::string::to_lower(mPassword);
	std::string pass_hash = double_hash(mCToken, mSToken, pass);

	Buffer Packet;
	Packet.addDWord(mCToken);
	Packet.addDWord(mSToken);

	// Copy hashed data, 5 dwords
	for (int x=0; x<5; x++){
		Packet.addDWord(ByteReader::readDWord(x*4, const_cast<char*>(pass_hash.data())));
	}

	Packet.addNTString(mAccount);

	return sendPacket(0x3A, Packet.getSize(), Packet.getBuffer());
}

bool ChatProtocol::sendSIDLOGONREALMEX(const char *sRealm)
{
	std::string realm_hash = double_hash(mCToken, mSToken, "password");

	Buffer Packet;
	Packet.addDWord(mCToken);

	// Copy hashed data, 5 dwords
	for (int x=0; x<5; x++){
		Packet.addDWord(ByteReader::readDWord(x*4, const_cast<char*>(realm_hash.data())));
	}

	Packet.addNTString(sRealm);

	return sendPacket(0x3E, Packet.getSize(), Packet.getBuffer());
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
				if (pTemp)
					delete [] pTemp;

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
			if (!genChecksum("data/")){
				return false;
			}

			// Generate key hashes
			if (!genKeyHashes()){
				if (pTemp)
					delete [] pTemp;
				return false;
			}

			break;
		}
		case 0x51:
		{
			if (!parseHashResult(pTemp)){
				if (pTemp)
					delete [] pTemp;
				return false;
			}
			break;
		}
		case 0x3A:
		{
			if (!parseLogonResult(pTemp)){
				if (pTemp)
					delete [] pTemp;
				return false;
			}
			break;
		}
		case 0x3E:
		{
			if (tmp>8){
				if (!parseLogonRealm(pTemp)){
					if (pTemp)
					delete [] pTemp;
					return false;
				}
			}
			else{
				printf("[%s] Failed to logon to realm.\n", mAccount);
			}
			break;
		}
	}

	if (pTemp)
		delete [] pTemp;

	return true;
}
