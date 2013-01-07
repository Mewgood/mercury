#ifndef __REALMPROTOCOL_H__
#define __REALMPROTOCOL_H__

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "TcpConnection.h"
#include "ChatProtocol.h"

class RealmProtocol : public TcpConnection {
	private:
		// Gereral
		char *mAccount;

		// Realm server data
		char mIp[INET_ADDRSTRLEN];
		unsigned short mPort;

		unsigned int mCookie, mStatus;
		unsigned int mChunk1[2], mChunk2[12];
		char *mUniqueName;

		bool sendPacket(char cId, unsigned short nLength, char *pData);
	public:
		RealmProtocol();
		~RealmProtocol();

		char* getIp() { return mIp; };
		unsigned int getPort() { return mPort; };

		bool setData(char *sAccount);
		bool setRealmData(ChatProtocol* pChat);

		bool sendProto();
		bool sendMCPSTARTUP();

		bool parsePacket();
};

#endif /* __REALMPROTOCOL_H__ */
