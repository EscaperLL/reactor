#include "stdafx.h"
#include "tcpServer.h"


tcpServer::tcpServer(const Ipv4Address& addr)
	:mAddr(addr)
{
	mAcceptor = std::make_shared<CAcceptor>(addr);
}


tcpServer::~tcpServer()
{
}

void tcpServer::start()
{
	mAcceptor->listen();
}

void tcpServer::newConnectionCallback(void *arg, int iFd)
{
	tcpServer* pTcp = static_cast<tcpServer*>(arg);
	if (NULL!= pTcp)
	{
		pTcp->handleNewConnection(iFd);
	}
}
