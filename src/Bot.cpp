#include "Bot.h"

Bot::Bot(const char *sHost, const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey)
{
	// Set all pointers to null
	mHost = 0; mAccount = 0; mPassword = 0; mKey = 0; mXKey = 0;
	
	mStatus = Unconnected;
	
	// Temp string length holder
	int nLen = 0;
	
	// Create buffers for all data and copy from parameters
	nLen = strlen(sHost);
	mHost = new char[nLen];
	strcpy(mHost, sHost);
	
	nLen = strlen(sAccount);
	mAccount = new char[nLen];
	strcpy(mAccount, sAccount);
	
	nLen = strlen(sPassword);
	mPassword = new char[nLen];
	strcpy(mPassword, sPassword);
	
	nLen = strlen(sKey);
	mKey = new char[nLen];
	strcpy(mKey, sKey);
	
	nLen = strlen(sXKey);
	mXKey = new char[nLen];
	strcpy(mXKey, sXKey); 
}

Bot::~Bot()
{
	// Delete all stored data if it exists
	if (mHost)
		delete [] mHost;
	if (mAccount)
		delete [] mAccount;
	if (mPassword)
		delete [] mPassword;
	if (mKey)
		delete [] mKey;
	if (mXKey)
		delete [] mXKey;
}

BotStatus Bot::status()
{	
	return mStatus;
}

void Bot::printStatus()
{
	switch (mStatus) {
		case Unconnected:
			printf("[%s] Status: Not connected to any servers.\n", mAccount);
			break;
		case Connecting:
			printf("[%s] Status: Connecting to servers.\n", mAccount);
			break;
		case Connected:
			printf("[%s] Status: Connected to servers.\n", mAccount);
			break;
		case Lobby:
			printf("[%s] Status: Ready to join game.\n", mAccount);
			break;
		case InGame:
			printf("[%s] Status: Currently in a D2GS game.\n", mAccount);
			break;
		case Dead:
			printf("[%s] Status: Bot died, obviously I did a great job programming.\n", mAccount);
			break;
		default:
			printf("[%s] Status: UNKNOWN\n", mAccount);
	}
}

bool Bot::run()
{
	mStatus = Connecting;
	
	// Connected to ALL protocols here
	
	printf("[%s] Connecting to %s:%u...\n", mAccount, mHost, 6112);
	if (!Chat._connect(mHost, 6112)) {
		printf("[%s] Failed to connect to chat server...\n", mAccount);
		mStatus = Dead;
		return false;
	}
	
	mStatus = Connecting;
	
	if (!Chat.sendProto()) {
		printf("[%s] Failed to send proto packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}
	
	if (!Chat.sendSIDAUTHINFO()) {
		printf("[%s] Failed to SID_AUTH_INFO <0x50> packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}
	
	// 0x25
	if (!Chat.parsePacket()) {
		printf("[%s] Failed to parse expected packet... >> 0x25\n", mAccount);
		mStatus = Dead;
		return false;
	}
	
	// 0x50
	if (!Chat.parsePacket()) {
		printf("[%s] Failed to parse packet... >> 0x50\n", mAccount);
		mStatus = Dead;
		return false;
	}
	
	
	printf("[%s] Bot reached end thread, shutting down...\n", mAccount);
	return true;
}
