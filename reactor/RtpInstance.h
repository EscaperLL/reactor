#pragma once

#include"Ipv4Address.h"
#include"SocketsOps.h"
#include"Rtp.h"

#include <string>
#include <stdint.h>
class RtpInstance
{
public:
	enum RtpType
	{
		RTP_OVER_UDP,
		RTP_OVER_TCP
	};


	RtpInstance(int localSockfd,uint16_t localport, const std::string& destIp, uint16_t destPort)
		: mRtpType(RTP_OVER_UDP), mSockfd(localSockfd), mLocalPort(localport),
		mDestAddr(destIp, destPort), mIsAlive(false), mSessionId(0)
	{}

	RtpInstance(int clientSockfd, uint8_t rtpChannel) :
		mRtpType(RTP_OVER_TCP), mSockfd(clientSockfd),
		mIsAlive(false), mSessionId(0), mRtpChannel(rtpChannel)
	{

	}
	~RtpInstance();

	uint16_t getLocalPort() const { return mLocalPort; }
	uint16_t getPeerPort() { return mDestAddr.getPort(); }

	int send(RtpPacket* rtpPacket)
	{
		if (mRtpType == RTP_OVER_UDP)
		{
			return sendOverUdp(rtpPacket->mBuffer, rtpPacket->mSize);
		}
		else
		{
			uint8_t* rtpPktPtr = rtpPacket->_mBuffer;
			rtpPktPtr[0] = '$';
			rtpPktPtr[1] = (uint8_t)mRtpChannel;
			rtpPktPtr[2] = (uint8_t)(((rtpPacket->mSize) & 0xFF00) >> 8);
			rtpPktPtr[3] = (uint8_t)((rtpPacket->mSize) & 0xFF);
			return sendOverTcp(rtpPktPtr, rtpPacket->mSize + 4);
		}
	}

	bool alive() const { return mIsAlive; }
	void setAlive(bool alive) { mIsAlive = alive; };
	void setSessionId(uint16_t sessionId) { mSessionId = sessionId; }
	uint16_t sessionId() const { return mSessionId; }

private:
	

	int sendOverUdp(void* buf, int size)
	{
		return sockets::sendto(mSockfd, buf, size, mDestAddr.getAddr());
	}

	int sendOverTcp(void* buf, int size)
	{
		return sockets::send(mSockfd, (char*)buf, size);
	}
private:
	RtpType mRtpType;
	int mSockfd;
	uint16_t mLocalPort; //for udp
	Ipv4Address mDestAddr; //for udp
	bool mIsAlive;
	uint16_t mSessionId;
	uint8_t mRtpChannel; //for tcp
};


class RtcpInstance
{
public:

	RtcpInstance(int localSockfd, uint16_t localPort,
		std::string destIp, uint16_t destPort) :
		mLocalSockfd(localSockfd), mLocalPort(localPort), mDestAddr(destIp, destPort),
		mIsAlive(false), mSessionId(0)
	{   }

	~RtcpInstance()
	{
		sockets::close(mLocalSockfd);
	}

	int send(void* buf, int size)
	{
		return sockets::sendto(mLocalSockfd, buf, size, mDestAddr.getAddr());
	}

	int recv(void* buf, int size, Ipv4Address* addr)
	{
		return 0;
	}

	uint16_t getLocalPort() const { return mLocalPort; }

	int alive() const { return mIsAlive; }
	void setAlive(bool alive) { mIsAlive = alive; };
	void setSessionId(uint16_t sessionId) { mSessionId = sessionId; }
	uint16_t sessionId() const { return mSessionId; }

public:
	

private:
	int mLocalSockfd;
	uint16_t mLocalPort;
	Ipv4Address mDestAddr;
	bool mIsAlive;
	uint16_t mSessionId;
};

