#include "RealmProtocol.h"

RealmProtocol::RealmProtocol()
{
	mAccount = 0;

	mPort	= 0;
	mCookie	= 0;
	mStatus	= 0;
	mUniqueName = 0;
}

RealmProtocol::~RealmProtocol()
{
	if (mAccount)
		delete [] mAccount;
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

bool RealmProtocol::setData(char *sAccount)
{
	if (mAccount)
		delete [] mAccount;

	unsigned int len = strlen(sAccount)+1;
	mAccount = new char[len];

	strcpy(mAccount, sAccount);

	return true;
}
