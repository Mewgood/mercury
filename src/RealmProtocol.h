#ifndef __REALMPROTOCOL_H__
#define __REALMPROTOCOL_H__

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "TcpConnection.h"
#include "ChatProtocol.h"

enum Difficulty {
	Normal = 0x0000,
	Nightmare = 0x1000,
	Hell = 0x2000
};

class RealmProtocol : public TcpConnection {
	private:
		// Gereral
		char *mAccount, *mCharacter;

		unsigned short mRequestId;

		// Realm server data
		char mIp[INET_ADDRSTRLEN];
		unsigned short mPort;

		unsigned int mCookie, mStatus;
		unsigned int mChunk1[2], mChunk2[12];
		char *mUniqueName;

		// Game data
		unsigned short mGameToken;

		bool mLastGame;

		bool sendPacket(char cId, unsigned short nLength, char *pData);
	public:
		RealmProtocol();
		~RealmProtocol();

		char* getIp() { return mIp; };
		unsigned int getPort() { return mPort; };

		bool lastGame() { return mLastGame; };

		bool setData(char *sAccount, char *sCharacter);
		bool setRealmData(ChatProtocol* pChat);

		bool sendProto();
		bool sendMCPSTARTUP();
		bool sendCHARLOGON();
		bool sendCREATEGAME(const char *sName, const char* sPassword, Difficulty eDiff);

		bool createGame(const char *sName, const char *sPassword, Difficulty eDiff);

		bool parsePacket();
};

#endif /* __REALMPROTOCOL_H__ */
