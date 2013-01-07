#ifndef __BOT_H__
#define __BOT_H__

#include <string.h>
#include "ChatProtocol.h"
#include "RealmProtocol.h"
#include "ByteReader.h"
#include "Buffer.h"
#include "util/utility.hpp"

enum BotStatus {
	Unconnected = 0,
	Connecting,
	Connected,
	Lobby,
	InGame,
	Dead
};

class Bot {
	private:
		ChatProtocol Chat;
		RealmProtocol Realm;

		char *mServer, *mRealm, *mAccount, *mPassword, *mCharacter ,*mKey, *mXKey, *mExeInfo, *mOwner;

		unsigned int mSToken, mCToken;

		BotStatus mStatus;
	public:
		Bot(const char *sServer, const char *sRealm, const char *sAccount, const char *sPassword, const char *sCharacter, const char *sKey, const char *sXKey,
			const char *sExeInfo, const char *sOwner);
		~Bot();

		BotStatus status();
		void printStatus();

		bool run();
};

#endif /* __BOT_H__ */
