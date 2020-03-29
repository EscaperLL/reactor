#include "stdafx.h"
#include "tcpSocket.h"
#include"SocketsOps.h"


tcpSocket::~tcpSocket()
{
}


bool tcpSocket::bind(std::string ip, uint16_t port)
{
	return sockets::bind(mfd,ip, port);
}

bool tcpSocket::listen()
{
	return sockets::listen(mfd);
}

int tcpSocket::accept()
{
	return sockets::accept(mfd);
}

void tcpSocket::setReuseAddr()
{
	sockets::setReuse(mfd, 0);
}
