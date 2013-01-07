#include "Bot.h"

Bot::Bot(const char *sServer, const char *sRealm, const char *sAccount, const char *sPassword, const char *sCharacter ,const char *sKey, const char *sXKey,
		const char *sExeInfo, const char *sOwner)
{
	// Set all pointers to null
	mServer = 0; mRealm = 0; mAccount = 0; mPassword = 0; mCharacter = 0; mKey = 0; mXKey = 0;
	mExeInfo = 0; mOwner = 0;

	mStatus = Unconnected;

	// Temp string length holder
	int nLen = 0;

	// Create buffers for all data and copy from parameters
	nLen = strlen(sServer)+1;
	mServer = new char[nLen];
	strcpy(mServer, sServer);

	nLen = strlen(sRealm)+1;
	mRealm = new char[nLen];
	strcpy(mRealm, sRealm);

	nLen = strlen(sAccount)+1;
	mAccount = new char[nLen];
	strcpy(mAccount, sAccount);

	nLen = strlen(sPassword)+1;
	mPassword = new char[nLen];
	strcpy(mPassword, sPassword);

	nLen = strlen(sCharacter)+1;
	mCharacter = new char[nLen];
	strcpy(mCharacter, sCharacter);

	nLen = strlen(sKey)+1;
	mKey = new char[nLen];
	strcpy(mKey, sKey);

	nLen = strlen(sXKey)+1;
	mXKey = new char[nLen];
	strcpy(mXKey, sXKey);

	nLen = strlen(sExeInfo)+1;
	mExeInfo = new char[nLen];
	strcpy(mExeInfo, sExeInfo);

	nLen = strlen(sOwner)+1;
	mOwner = new char[nLen];
	strcpy(mOwner, sOwner);
}

Bot::~Bot()
{
	// Delete all stored data if it exists
	if (mServer)
		delete [] mServer;
	if (mRealm)
		delete [] mRealm;
	if (mAccount)
		delete [] mAccount;
	if (mPassword)
		delete [] mPassword;
	if (mCharacter)
		delete [] mCharacter;
	if (mKey)
		delete [] mKey;
	if (mXKey)
		delete [] mXKey;
	if (mExeInfo)
		delete [] mExeInfo;
	if (mOwner)
		delete [] mOwner;
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
	mCToken = get_tick_count();
	if (!Chat.setData(mAccount, mPassword, mKey, mXKey, mCToken, mExeInfo, mOwner)) {
		printf("[%s] Failed to set configuration...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// Connect to protocols, wrap later ( bncs_connect, bncs_disconnect, bncs_logon, mcp_connect, game_connect, game_create, game_join, etc )
	mStatus = Connecting;

	printf("[%s] Connecting to %s:%u...\n", mAccount, mServer, 6112);
	if (!Chat._connect(mServer, 6112)) {
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

	// 0x50
	if (!Chat.sendSIDAUTHINFO()) {
		printf("[%s] Failed to send SID_AUTH_INFO <0x50> packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x25
	if (!Chat.parsePacket()) {
		printf("[%s] Error while parsing packet <0x25> \n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x50
	if (!Chat.parsePacket()) {
		printf("[%s] Error while parsing packet <0x50>\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x51
	if (!Chat.sendSIDAUTHCHECK()){
		printf("[%s] Failed to send SID_AUTH_CHECK <0x51> packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x51
	if (!Chat.parsePacket()){
		printf("[%s] Error while parsing packet <0x51>\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x3A
	if (!Chat.sendSIDLOGONRESPONSE()){
		printf("[%s] Failed to send SID_LOGONRESPONSE <0x3A> packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x3A
	if (!Chat.parsePacket()){
		printf("[%s] Error while parsing packet SID_LOGONRESPONSE <0x3A>\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x3E
	if (!Chat.sendSIDLOGONREALMEX(mRealm)){
		printf("[%s] Failed to send SID_LOGONREALMEX <0x3E> packet...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	// 0x3E
	if (!Chat.parsePacket()){
		printf("[%s] Error while parsing packet SID_LOGONREALMEX <0x3E>\n", mAccount);
		mStatus = Dead;
		return false;
	}

	/////////////////////////////////////////
	//////////////// REALM //////////////////
	/////////////////////////////////////////

	printf("[%s] Connecting to %s:%u...\n", mAccount, Realm.getIp(), Realm.getPort());
	if (!Realm._connect(Realm.getIp(), Realm.getPort())) {
		printf("[%s] Failed to connect to realm server...\n", mAccount);
		mStatus = Dead;
		return false;
	}

	printf("[%s] Bot reached end of thread, shutting down...\n", mAccount);
	return true;
}
