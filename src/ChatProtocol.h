#ifndef __CHATPROTOCOL_H__
#define __CHATPROTOCOL_H__

#include <stdio.h>
#include "TcpConnection.h"

class ChatProtocol : public TcpConnection {
	private:
		unsigned int mSToken, mCToken;
		
		unsigned long mFileTime;
		
		char *mFileName, *mValueString;
		
		bool sendPacket(char cId, unsigned short nLength, char *pData);
	public:
		ChatProtocol();
		~ChatProtocol();
		
		bool sendProto();
		bool sendSIDAUTHINFO();
		
		bool parsePacket();
};

#endif /* __CHATPROTOCOL_H__ */
