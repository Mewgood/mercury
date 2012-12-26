#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>

#include "ByteReader.h"
#include "Buffer.h"

using Poco::Net::StreamSocket;
using Poco::Net::SocketStream;
using Poco::Net::SocketAddress;

class TcpConnection {
	protected:
		SocketAddress *mSA;
		StreamSocket mSock;
		
		bool mConnected;
	public:
		TcpConnection();
		~TcpConnection();
		
		bool _connect(const char *sHost, unsigned short nPort);
		unsigned int _send(const char *pData, unsigned int nLength);
		unsigned int _recv(char *pData, unsigned int nLength);
};

#endif /* __TCPCONNECTION_H__ */
