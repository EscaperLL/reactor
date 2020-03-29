#include "stdafx.h"
#include "tcpConnection.h"
#include"CevenSchedule.h"

#include"SocketsOps.h"
#include"PrintLog.h"



tcpConnection::tcpConnection(int fd)
	:mSocket(fd)
{
	mEvent = std::make_shared<IOEvent>(fd,this);
	mEvent->setReadCallback(readCallback);
	mEvent->setWriteCallback(writeCallback);
	mEvent->setErrorCallback(errorCallback);
	mEvent->enableReadHandling(); //Ä¬ÈÏÖ»¿ªÆô¶Á
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->addIOEvent(mEvent.get());
}


tcpConnection::~tcpConnection()
{
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->removeIOEvent(mEvent.get());
}

void tcpConnection::setDisconnectionCallback(DisconnectionCallback cb, void* arg)
{
	mDisconnectionCallback = cb;
	mArg = arg;
}

void tcpConnection::enableReadHandling()
{
	if (mEvent->isReadHandling())
	{
		return;
	}

	mEvent->enableReadHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());
}

void tcpConnection::enableWriteHandling()
{
	if (mEvent->isWriteHandling())
	{
		return;
	}

	mEvent->enableWriteHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());
}

void tcpConnection::enableErrorHandling()
{
	if (mEvent->isErrorHandling())
	{
		return;
	}

	mEvent->enableErrorHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());

}

void tcpConnection::disableReadeHandling()
{
	if (mEvent->isReadHandling())
	{
		return;
	}

	mEvent->disableReadeHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());
}

void tcpConnection::disableWriteHandling()
{
	if (mEvent->isWriteHandling())
	{
		return;
	}

	mEvent->disableWriteHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());
}

void tcpConnection::disableErrorHandling()
{
	if (mEvent->isErrorHandling())
	{
		return;
	}

	mEvent->disableErrorHandling();
	CevenSchedule* pSchedule = CevenSchedule::getInstance().get();
	pSchedule->updateIOEvent(mEvent.get());
}

void tcpConnection::handleRead()
{
	int recv_len = sizeof(m_recv_buf) - 1 - m_recv_len;
	if (recv_len <= 0) {
		
		return ;
	}
	int ret =   sockets::read(mSocket.fd(),m_recv_buf + m_recv_len, recv_len);
	if (ret < 0) {
		
		return ;
	}
	m_recv_len += ret;

	handleReadBytes();
}

void tcpConnection::handleDisconnection()
{
	if (mDisconnectionCallback)
	{
		mDisconnectionCallback(mArg, mSocket.fd());
	}
}

void tcpConnection::handleReadBytes()
{
	LogDebug("default read handle\n");
 }
void tcpConnection::handleWrite()
{
	LogDebug("default Write handle\n");
 }
void tcpConnection::handleError()
{
	LogDebug("default Error handle\n");
 }

void tcpConnection::readCallback(void* arg)
{
	tcpConnection *pCon = static_cast<tcpConnection*>(arg);
	if (NULL != pCon)
	{
		pCon->handleRead();
	}

}
void tcpConnection::writeCallback(void* arg)
{
	tcpConnection *pCon = static_cast<tcpConnection*>(arg);
	if (NULL != pCon)
	{
		pCon->handleWrite();
	}
}
void tcpConnection::errorCallback(void* arg)
{
	tcpConnection *pCon = static_cast<tcpConnection*>(arg);
	if (NULL != pCon)
	{
		pCon->handleError();
	}
}
