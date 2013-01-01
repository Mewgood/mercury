#ifndef __CHATPROTOCOL_H__
#define __CHATPROTOCOL_H__

#include <stdio.h>
#include <string.h>
#include "TcpConnection.h"
#include "util/check_revision.hpp"
#include "util/d2cdkey.hpp"
#include "util/bsha1.hpp"
#include "util/string.hpp"

class ChatProtocol : public TcpConnection {
	private:
		// Account information
		char *mAccount, *mPassword, *mKey, *mXKey, *mExeInfo, *mOwner;
		
		// Hashing values and what not
		unsigned int mSToken, mCToken;
		unsigned long mFileTime;
		char *mFileName, *mValueString;
		
		// Generated values
		unsigned long mChecksum;
		unsigned int mKeyPublic, mXKeyPublic;
		unsigned int mKeyHash[5], mXKeyHash[5];
		
		bool sendPacket(char cId, unsigned short nLength, char *pData);
		bool genChecksum(std::string sDirectory);
		bool genKeyHashes();
		bool parseHashResult(char *pTemp);
		bool parseLogonResult(char *pTemp);
	public:
		ChatProtocol();
		~ChatProtocol();
		
		bool setData(const char *sAccount, const char *sPassword, const char *sKey, const char *sXKey, unsigned int nCToken,
					const char *sExeInfo, const char *sOwner);
		bool sendProto();
		bool sendSIDAUTHINFO();
		bool sendSIDAUTHCHECK();
		bool sendSIDLOGONRESPONSE();
		
		bool parsePacket();
};

#endif /* __CHATPROTOCOL_H__ */
