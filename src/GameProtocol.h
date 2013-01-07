#ifndef __GAMEPROTOCOL_H_
#define __GAMEPROTOCOL_H_

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "TcpConnection.h"
#include "RealmProtocol.h"

class GameProtocol {
	private:
		char mIp[INET_ADDRSTRLEN];
		unsigned int mGameHash;
		unsigned short mGameToken;
	public:
		GameProtocol();
		~GameProtocol();

		bool setData(char *sIp, unsigned int nGameHash, unsigned short nGameToken);
};

#endif /* __GAMEPROTOCOL_H_ */
