#include "RealmProtocol.h"

RealmProtocol::RealmProtocol()
{
	mAccount = 0;
	mCharacter = 0;

	mRequestId = 0x02;

	mPort	= 0;
	mCookie	= 0;
	mStatus	= 0;
	mUniqueName = 0;
}

RealmProtocol::~RealmProtocol()
{
	if (mAccount)
		delete [] mAccount;
	if (mCharacter)
		delete [] mCharacter;
	if (mUniqueName)
		delete [] mUniqueName;
}

bool RealmProtocol::sendPacket(char cId, unsigned short nLength, char *pData)
{
	unsigned short len = 3 + nLength; // Header + Body

	Buffer Packet;
	Packet.addWord(len);
	Packet.addByte(cId);
	Packet.copyMem(pData, nLength);

	int tmp = _send(Packet.getBuffer(), len);

	if (tmp != len) {
		printf("[%s] REALM << 0x%02X | Sent %u of expected %u bytes\n", mAccount, cId, tmp, len);
		return false;
	}

	printf("[%s] REALM << 0x%02X | Sent %u bytes\n", mAccount, cId, len);

	return true;
}

bool RealmProtocol::parseJoinGame(char *pData)
{
	unsigned int ip = ByteReader::readDWord(6, pData);
	mGHash = ByteReader::readDWord(10, pData);
	mGToken = ByteReader::readWord(2, pData);

	char sip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip, sip, INET_ADDRSTRLEN);
	strcpy(mGIp, sip);

	return true;
}

bool RealmProtocol::setData(char *sAccount, char *sCharacter)
{
	if (mAccount)
		delete [] mAccount;
	if (mCharacter)
		delete [] mCharacter;

	unsigned int len = strlen(sAccount)+1;
	mAccount = new char[len];
	strcpy(mAccount, sAccount);

	len = strlen(sCharacter)+1;
	mCharacter = new char[len];
	strcpy(mCharacter, sCharacter);

	return true;
}

bool RealmProtocol::setRealmData(ChatProtocol* pChat)
{
	char* ip = pChat->getRealmIp();
	int len = strlen(ip)+1;
	strcpy(mIp, ip);

	char* uni = pChat->getRealmUniqueName();
	len = strlen(ip)+1;
	mUniqueName = new char[len];
	strcpy(mUniqueName, uni);

	mPort = pChat->getRealmPort();
	mCookie = pChat->getRealmCookie();
	mStatus = pChat->getRealmStatus();

	memcpy(mChunk1, pChat->getRealmChunk1(), 8);
	memcpy(mChunk2, pChat->getRealmChunk2(), 48);

	return true;
}

bool RealmProtocol::sendProto()
{
	// Send required byte of 0x01 to indicate protocol
	int tmp = _send("\x01", 1);

	if (tmp != 1) {
		printf("[%s] REALM << 0x01 | Sent %u of expected %u bytes\n", mAccount, tmp, 1);
		return false;
	}

	printf("[%s] REALM << 0x01 | Sent 1 byte\n", mAccount);

	return true;
}

bool RealmProtocol::sendMCPSTARTUP()
{
	Buffer Packet;
	Packet.addDWord(mCookie);
	Packet.addDWord(mStatus);
	Packet.copyMem((char*)mChunk1, 8);
	Packet.copyMem((char*)mChunk2, 48);
	Packet.addNTString(mUniqueName);

	return sendPacket(0x01, Packet.getSize(), Packet.getBuffer());
}

bool RealmProtocol::sendCHARLOGON()
{
	Buffer Packet;
	Packet.addNTString(mCharacter);

	return sendPacket(0x07, Packet.getSize(), Packet.getBuffer());
}

bool RealmProtocol::sendCREATEGAME(const char *sName, const char* sPassword, Difficulty eDiff)
{
	Buffer Packet;
	Packet.addWord(mRequestId);
	Packet.addDWord(eDiff);
	Packet.addByte(0x01);
	Packet.addByte(0xFF); // Unrestricted
	Packet.addByte(0x08); // 8 players max
	Packet.addNTString(sName);
	Packet.addNTString(sPassword);
	Packet.addByte(0x00); // NUll terminator for game description

	return sendPacket(0x03, Packet.getSize(), Packet.getBuffer());
}

bool RealmProtocol::createGame(const char *sName, const char *sPassword, Difficulty eDiff)
{
	// Create and join eventually.
	return sendCREATEGAME(sName, sPassword, eDiff);
}

bool RealmProtocol::sendJOINGAME(const char *sName, const char *sPassword)
{
	Buffer Packet;
	Packet.addWord(mRequestId);
	Packet.addNTString(sName);
	Packet.addNTString(sPassword);

	return sendPacket(0x04, Packet.getSize(), Packet.getBuffer());
}

bool RealmProtocol::joinGame(const char *sName, const char *sPassword)
{
	return sendJOINGAME(sName, sPassword);
}

bool RealmProtocol::parsePacket()
{
	// Receive packet header, 3 bytes
	char *pTemp = new char[3];
	int tmp = _recv(pTemp, 3);

	if (tmp != 3) {
		printf("Error : malformed packet header...\n");
		printf("Received %u of expected %u bytes\n", tmp, 3);

		if (pTemp)
			delete [] pTemp;

		return false;
	}

	char cId = ByteReader::readByte(2, pTemp);
	unsigned short nLen = ByteReader::readWord(0, pTemp);

	delete [] pTemp;

	printf("[%s] REALM >> 0x%02X | Received header (%u bytes)\n", mAccount, cId, tmp);

	nLen -= 3;

	pTemp = new char[nLen];
	tmp = _recv(pTemp, nLen);

	if (tmp != nLen) {
		printf("Error : received malformed packet body... >> 0x%02X\n", cId);
		printf("Received %u of expected %u bytes\n", tmp, nLen);

		if (pTemp)
			delete [] pTemp;

		return false;
	}

	printf("[%s] REALM >> 0x%02X | Received %u bytes\n", mAccount, cId, tmp);

	switch (cId)
	{
		case 0x01:
		{
			switch (ByteReader::readDWord(0, pTemp))
			{
				case 0x02:
				case 0x0A:
				case 0x0B:
				case 0x0C:
				case 0x0D:
				{
					printf("[%s] Realm unavailable, not battle.net connection detected.\n", mAccount);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
				case 0x7E:
				{
					printf("[%s] CDKey banned from realm play.\n", mAccount);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
				case 0x7F:
				{
					printf("[%s] Temporary IP ban from realm servers\n", mAccount);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
				default:
					printf("[%s] Accepted by realm server.\n", mAccount);
			}
			break;
		}
		case 0x03:
		{
			switch (ByteReader::readDWord(6, pTemp))
			{
				case 0x00:
				{
					mGameToken = ByteReader::readWord(2, pTemp);
					mLastGame = true;

					printf("[%s] Successfully created game. Request Id 0x%02X.\n", mAccount, ByteReader::readWord(0, pTemp));
					break;
				}
				case 0x1E:
				{
					mLastGame = false;
					printf("[%s] Invalid game name.\n", mAccount);
					break;
				}
				case 0x1F:
				{
					mLastGame = false;
					printf("[%s] Game already exists.\n", mAccount);
					break;
				}
				case 0x20:
				{
					mLastGame = false;
					printf("[%s] Game servers are down.\n", mAccount);
					break;
				}
				case 0x6E:
				{
					mLastGame = false;
					printf("[%s] A dead hardcore character can't create games.\n", mAccount);
				}
			}
			break;
		}
		case 0x04:
		{
			switch (ByteReader::readDWord(14, pTemp))
			{
				case 0x00:
				{
					mJoined = true;
					printf("[%s] Successfully joined game. Request Id 0x%02X\n", mAccount, mRequestId);
					parseJoinGame(pTemp);
					break;
				}
				default:
				{
					mJoined = false;
					printf("[%s] Failed to join game. 0x%02X\n", mAccount, ByteReader::readDWord(14, pTemp));
				}
			}
			break;
		}
		case 0x07:
		{
			switch (ByteReader::readDWord(0, pTemp))
			{
				case 0x00:
				{
					printf("[%s] Logged onto %s.\n", mAccount, mCharacter);
					break;
				}
				case 0x46:
				{
					printf("[%s] Character %s not found.\n", mAccount, mCharacter);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
				case 0x7A:
				{
					printf("[%s] Character logon failed.\n", mAccount);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
				case 0x7B:
				{
					printf("[%s] Character %s has expired.\n", mAccount, mCharacter);

					if (pTemp)
						delete [] pTemp;
					return false;
				}
			}
			break;
		}
	}

	if (pTemp)
		delete [] pTemp;

	return true;
}
