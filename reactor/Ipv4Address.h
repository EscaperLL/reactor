#pragma once
#include<WinSock2.h>
#include<string>
class Ipv4Address
{
public:
	Ipv4Address();
	Ipv4Address(std::string ip, uint16_t port);
	void setAddr(std::string ip, uint16_t port);
	std::string getIp();
	uint16_t getPort();
	struct sockaddr* getAddr();

private:
	std::string mIp;
	uint16_t mPort;
	struct sockaddr_in mAddr;
};

