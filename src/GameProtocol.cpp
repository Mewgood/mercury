#include "GameProtocol.h"

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

bool GameProtocol::setData(char *sIp, unsigned int nGameHash, unsigned short nGameToken)
{
	strcpy(mIp, sIp);
	mGameHash = nGameHash;
	mGameToken = nGameToken;
	return true;
}
