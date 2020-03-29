#pragma once
#include"Ipv4Address.h"
#include"Event.h"
#include"tcpSocket.h"
#include<memory>
class CAcceptor
{
public:
	static CAcceptor* createNew(Ipv4Address);
	typedef void(*NewConnectionCallback)(void* data, int confd);
	CAcceptor(const Ipv4Address& addr);
	~CAcceptor();
	

	bool listenning()const { return mListenning; }
	void listen();
	void setNewConnectionCallback(NewConnectionCallback cb, void* age);
private:
	static void readCallback(void*);
	void handleRead();

private:
	std::shared_ptr<IOEvent> mAcceptorEvent;
	Ipv4Address mAddr;
	tcpSocket mSocket;
	bool mListenning;
	NewConnectionCallback mNewConnectionCallback;
	void* mArg;
};

