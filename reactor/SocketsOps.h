#pragma once
#include <Ws2tcpip.h>
#include<string>
namespace sockets
{
	
	int createTcpSock();
	int createUdpSock();
	void close(int fd);
	bool bind(int sockfd, std::string ip, uint16_t port);
	bool listen(int sockfd, int backlog=128);
	int accept(int sockfd);
	int read(int sockfd, char* buf, int len);
	int send(int sockfd, const char* buf, int len);
	int sendto(int sockfd, const void* buf, int len, const struct sockaddr *destAddr);
	int set_block_opt(int sockfd,bool is_block);
	void setReuse(int sockfd, int on);
	void setNoDelay(int sockfd);
	void setKeepAlive(int sockfd);
	void setNoSigpipe(int sockfd);
	void setSendBufSize(int sockfd, int size);
	void setRecvBufSize(int sockfd, int size);
	std::string getPeerIp(int sockfd);
	int16_t getPeerPort(int sockfd);
	int getPeerAddr(int sockfd, struct sockaddr_in *addr);
	void close(int sockfd);
	bool connect(int sockfd, std::string ip, uint16_t port, int timeout);
	std::string getLocalIp();
}


