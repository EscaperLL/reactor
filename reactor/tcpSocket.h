#pragma once
#include<string>
class tcpSocket
{
public:
	explicit tcpSocket(int sockfd)
		:mfd(sockfd)
	{}
	~tcpSocket();

	int fd() { return mfd; }
	bool bind(std::string ip, uint16_t port);
	bool listen();
	int accept();
	void setReuseAddr();

private:
	int mfd;
};

