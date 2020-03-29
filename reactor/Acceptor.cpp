#include "stdafx.h"
#include "Acceptor.h"
#include"SocketsOps.h"

CAcceptor::CAcceptor(const Ipv4Address& addr)
	:mAddr(addr),
	mSocket(sockets::createTcpSock()),
	mNewConnectionCallback(NULL)
{
	mSocket.setReuseAddr();
	mSocket.bind(mAddr.getIp(), mAddr.getPort());
	mAcceptorEvent = std::make_shared<IOEvent>(mSocket.fd(), this);
	mAcceptorEvent->setReadCallback(readCallback);
	mAcceptorEvent->enableReadHandling();
}


CAcceptor* CAcceptor::createNew(Ipv4Address addr)
{
	return new CAcceptor(addr);
}

CAcceptor::~CAcceptor()
{

}

void CAcceptor::listen()
{
	mListenning = true;
	mSocket.listen();
}


void CAcceptor::setNewConnectionCallback(NewConnectionCallback cb, void* arg)
{
	mNewConnectionCallback = cb;
	mArg = arg;
}

void CAcceptor::readCallback(void* arg)
{
	CAcceptor* acceptor = (CAcceptor*)arg;
	acceptor->handleRead();
}

void CAcceptor::handleRead()
{
	int fd = mSocket.accept();
	if (mNewConnectionCallback)
	{
		mNewConnectionCallback(mArg,fd);
	}
}
