#pragma once

#include"Event.h"
#include"tcpSocket.h"

#include<memory>
class tcpConnection
{
public:
	typedef void(*DisconnectionCallback)(void*, int);
	tcpConnection(int fd);
	~tcpConnection();
	void setDisconnectionCallback(DisconnectionCallback cb, void* arg);

protected:
	enum {
		MAX_RECV_BUF_LEN = 1024 * 4,
	};
	void enableReadHandling();
	void enableWriteHandling();
	void enableErrorHandling();
	void disableReadeHandling();
	void disableWriteHandling();
	void disableErrorHandling();

	void handleRead();
	virtual void handleReadBytes();
	virtual void handleWrite();
	virtual void handleError();

	void handleDisconnection();

private:
	static void readCallback(void* arg);
	static void writeCallback(void* arg);
	static void errorCallback(void* arg);

protected:
	tcpSocket mSocket;
	std::shared_ptr<IOEvent> mEvent;
	DisconnectionCallback mDisconnectionCallback{NULL};
	void* mArg{ NULL };
	char m_recv_buf[MAX_RECV_BUF_LEN];
	uint32_t m_recv_len;
	char mBuffer[2048];
};

