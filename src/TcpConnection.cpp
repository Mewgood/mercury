#include "TcpConnection.h"

TcpConnection::TcpConnection()
{
	mConnected = false;
	mSA = 0;
}

TcpConnection::~TcpConnection()
{
	if (mSA)
		delete mSA;
	
	mConnected = false;
}

bool TcpConnection::_connect(const char *sHost, unsigned short nPort)
{
	mSA = new SocketAddress(sHost, nPort);
	
	mSock.connect(*mSA);
	
	return true;
}

unsigned int TcpConnection::_send(const char *pData, unsigned int nLength)
{	
	return mSock.sendBytes(pData, nLength);
}

unsigned int TcpConnection::_recv(char *pData, unsigned int nLength)
{
	return mSock.receiveBytes(pData, nLength);
}
