#pragma once
#include"tcpSocket.h"
#include"Acceptor.h"
#include<memory>
class tcpServer
{
public:
	
	~tcpServer();
	void start();
protected:
	tcpServer(const Ipv4Address& addr);
	virtual void handleNewConnection(int confd) = 0;
private:
	static void newConnectionCallback(void *arg, int iFd);
protected:
	std::shared_ptr<CAcceptor> mAcceptor;
	Ipv4Address mAddr;
};

