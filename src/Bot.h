#ifndef __BOT_H__
#define __BOT_H__

#include <string.h>
#include "ChatProtocol.h"
#include "ByteReader.h"
#include "Buffer.h"

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
		
		char *mHost, *mAccount, *mPassword, *mKey, *mXKey;
		
		unsigned int mSToken, mCToken;
		
		BotStatus mStatus;
	public:
		Bot(const char *sHost, const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey);
		~Bot();
		
		BotStatus status();
		void printStatus();
		
		bool run();
};

#endif /* __BOT_H__ */
