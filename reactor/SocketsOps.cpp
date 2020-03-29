#include "stdafx.h"
#include "SocketsOps.h"


void sockets::close(int fd)
{
	::closesocket(fd);
}

int sockets::createTcpSock()
{
	return ::socket(AF_INET, SOCK_STREAM, 0);
}

int sockets::createUdpSock()
{
	return ::socket(AF_INET, SOCK_DGRAM, 0);
}

bool sockets::bind(int sockfd, std::string ip, uint16_t port)
{
	struct sockaddr_in local_addr;
	//获取结构体
	auto lumbdaFunGet_addr = [](const char* ip, int port, sockaddr_in &addrIn)->int
	{
		int ip_n = 0;
		if (ip == NULL || ip[0] == '\0')
			ip_n = 0;
		else if (inet_addr(ip) == INADDR_NONE) {
			struct hostent* hent = NULL;
			if ((hent = gethostbyname(ip)) == NULL) {
				//LogError("get host by name failed, name:%s\n", ip);
				return -1;
			}
			memcpy(&ip_n, *(hent->h_addr_list), sizeof(ip_n));
		}
		else
			ip_n = inet_addr(ip);
		addrIn.sin_family = AF_INET;
		addrIn.sin_addr.s_addr = ip_n;
		addrIn.sin_port = htons(port);
		return 0;
	};

	if (-1 == ::bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)))
	{
		return false;
	}
	return true;
}

bool sockets::listen(int sockfd, int backlog)
{
	if (::listen(sockfd, backlog) == -1)
	{
		return true;
	}
	return false;
}
int sockets::read(int sockfd, char* buf, int len)
{
	if (buf == NULL || len <= 0)
		return -1;
	int ret = ::recv(sockfd, buf, len, 0);
	if (ret < -1)
	{
		return -1;			 
	}
	return ret;
}
int sockets::accept(int sockfd)
{
	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof(struct sockaddr_in);

	int connfd = ::accept(sockfd, (struct sockaddr*)&addr, &addrlen);
	return connfd;
}


int sockets::send(int sockfd, const char* buf, int len)
{
	return ::send(sockfd, buf, len,0);
}

int sockets::sendto(int sockfd, const void* buf, int len,
	const struct sockaddr *destAddr)
{
	socklen_t addrLen = sizeof(struct sockaddr);
	return ::sendto(sockfd,(char*) buf, len, 0, destAddr, addrLen);
}

int sockets::set_block_opt(int sockfd, bool is_block)
{
	u_long block = 1;
	if (is_block)
		block = 0;
	if (SOCKET_ERROR == ::ioctlsocket(sockfd, FIONBIO, &block)) {
		//LogError("set socket block failed!\n");
		return -1;
	}
	return 0;
}

void sockets::setReuse(int sockfd, int on)
{

	int opt = 1;
	if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		//LogError("set reuseaddr failed, err info:%d %s\n", ERROR_NO, ERROR_STR);
		
	}
}




void sockets::setNoDelay(int sockfd)
{
#ifdef TCP_NODELAY
	int on = 1;
	int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
#endif
}

void sockets::setKeepAlive(int sockfd)
{
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
}

void sockets::setNoSigpipe(int sockfd)
{
#ifdef SO_NOSIGPIPE
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char *)&on, sizeof(on));
#endif
}

void sockets::setSendBufSize(int sockfd, int size)
{
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
}

void sockets::setRecvBufSize(int sockfd, int size)
{
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
}

std::string sockets::getPeerIp(int sockfd)
{
	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof(struct sockaddr_in);
	if (getpeername(sockfd, (struct sockaddr *)&addr, &addrlen) == 0)
	{
		return inet_ntoa(addr.sin_addr);
	}

	return "0.0.0.0";
}

int16_t sockets::getPeerPort(int sockfd)
{
	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof(struct sockaddr_in);
	if (getpeername(sockfd, (struct sockaddr *)&addr, &addrlen) == 0)
	{
		return ntohs(addr.sin_port);
	}

	return 0;
}

int sockets::getPeerAddr(int sockfd, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	return getpeername(sockfd, (struct sockaddr *)addr, &addrlen);
}


bool sockets::connect(int sockfd, std::string ip, uint16_t port, int timeout)
{
	bool isConnected = true;
	if (timeout > 0)
	{
		sockets::set_block_opt(sockfd,false);
	}

	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (::connect(sockfd, (struct sockaddr*)&addr, addrlen) < 0)
	{
		if (timeout > 0)
		{
			isConnected = false;
			fd_set fdWrite;
			FD_ZERO(&fdWrite);
			FD_SET(sockfd, &fdWrite);
			struct timeval tv = { timeout / 1000, timeout % 1000 * 1000 };
			select(sockfd + 1, NULL, &fdWrite, NULL, &tv);
			if (FD_ISSET(sockfd, &fdWrite))
			{
				isConnected = true;
			}
			sockets::set_block_opt(sockfd, false);
		}
		else
		{
			isConnected = false;
		}
	}

	return isConnected;
}

std::string sockets::getLocalIp()
{	
	return "0.0.0.0";
}