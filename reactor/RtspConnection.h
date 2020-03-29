#pragma once
#include"tcpConnection.h"
#include"RtpInstance.h"
#include"MediaSession.h"

#include "Def.h"
class RtspServer;
class CRtspConnection : public tcpConnection
{
public:

	CRtspConnection(RtspServer* rtpserver,int fd);
	virtual ~CRtspConnection();

protected:
	virtual void handleReadBytes();

private:
	bool parseRequest1(const char* begin, const char* end);
	bool parseRequest2(const char* begin, const char* end);

	int  parse_data(const char* data, int len);
	RtspMethodT parse_method(const char* data, int len);
	int handle_cmd(const char* data, int len);
	bool parseCSeq(std::string& message);
	bool parseAccept(std::string& message);
	bool parseTransport(std::string& message);
	bool parseMediaTrack();
	bool parseSessionId(std::string& message);

	bool handleCmdOption();
	bool handleCmdDescribe();
	bool handleCmdPlay();
	bool handleCmdTeardown();
	bool handleCmdGetParamter();
	bool handleCmdSetup();

	int sendMessage(void* buf, int size);
	int sendMessage();

	bool createRtpRtcpOverUdp(MediaSession::TrackId trackId, std::string peerIp,
		uint16_t peerRtpPort, uint16_t peerRtcpPort);
	bool createRtpOverTcp(MediaSession::TrackId trackId, int sockfd, uint8_t rtpChannel);

	void handleRtpOverTcp();
private:
	RtspServer* mRtspServer;
	std::string mPeerIp;
	RtspMethodT mMethod;
	std::string mUrl;
	std::string mSuffix;
	uint32_t mCSeq;
	uint16_t mPeerRtpPort;
	uint16_t mPeerRtcpPort;
	MediaSession::TrackId mTrackId;
	std::shared_ptr<RtpInstance> mRtpInstances[MEDIA_MAX_TRACK_NUM];
	std::shared_ptr<RtcpInstance> mRtcpInstances[MEDIA_MAX_TRACK_NUM];
	std::shared_ptr<MediaSession> mSession;
	int mSessionId;
	bool mIsRtpOverTcp;
	uint8_t mRtpChannel;
};

