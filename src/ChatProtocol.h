#ifndef __CHATPROTOCOL_H__
#define __CHATPROTOCOL_H__

#include <stdio.h>
#include "TcpConnection.h"
#include "util/check_revision.hpp"

class ChatProtocol : public TcpConnection {
	private:
		// Account information
		char *mAccount, *mPassword, *mKey, *mXKey;
		
		// Hashing values and what not
		unsigned int mSToken, mCToken;
		unsigned long mFileTime;
		char *mFileName, *mValueString;
		
		bool sendPacket(char cId, unsigned short nLength, char *pData);
	public:
		ChatProtocol();
		~ChatProtocol();
		
		bool setData(const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey);
		bool sendProto();
		bool sendSIDAUTHINFO();
		
		bool parsePacket();
};

#endif /* __CHATPROTOCOL_H__ */
