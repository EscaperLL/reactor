#include "stdafx.h"
#include "RtspConnection.h"
#include"RtspServer.h"
#include"PrintLog.h"
#include"SocketsOps.h"
#include"Rtp.h"
#include<windows.h>

CRtspConnection::CRtspConnection(RtspServer* rtpserver, int fd)
	:tcpConnection(fd),
	mRtspServer(rtpserver),
	mMethod(RTSP_METHOD_MAX),
	mSessionId(rand()),
	mIsRtpOverTcp(false)
{
	for (int i = 0; i < MEDIA_MAX_TRACK_NUM; ++i)
	{
		mRtpInstances[i] = NULL;
		mRtcpInstances[i] = NULL;
	}
	for (int i = 0; i < MEDIA_MAX_TRACK_NUM; ++i)
	{
		mRtpInstances[i] = NULL;
		mRtcpInstances[i] = NULL;
	}

	mPeerIp = [](int sockId)->std::string {
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		int ret = 0;
		memset(&addr, 0, sizeof(addr));
		ret = getpeername(sockId, (struct sockaddr *)&addr, &addr_len);
		if (ret == 0)
		{
			
		}
		else
		{
			LogError("getpeername succ:%s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		}
		return inet_ntoa(addr.sin_addr);
	}(fd);
}


CRtspConnection::~CRtspConnection()
{
	for (int i = 0; i < MEDIA_MAX_TRACK_NUM; ++i)
	{
		if (mRtpInstances[i])
		{
			if (mSession)
				mSession->removeRtpInstance(mRtpInstances[i]);		
			
		}
		//关rtcp
	}
}

bool CRtspConnection::parseRequest1(const char* begin, const char* end)
{
	std::string message(begin, end);
	char method[64] = { 0 };
	char url[512] = { 0 };
	char version[64] = { 0 };

	if (sscanf(message.c_str(), "%s %s %s", method, url, version) != 3)
	{
		return false;
	}

	if (!strcmp(method, "OPTIONS"))
		mMethod = RTSP_OPTIONS;
	else if (!strcmp(method, "DESCRIBE"))
		mMethod = RTSP_DESCRIBE;
	else if (!strcmp(method, "SETUP"))
		mMethod = RTSP_SETUP;
	else if (!strcmp(method, "PLAY"))
		mMethod = RTSP_PLAY;
	else if (!strcmp(method, "TEARDOWN"))
		mMethod = RTSP_TEARDOWN;
	else if (!strcmp(method, "GET_PARAMETER"))
		mMethod = RTSP_GET_PARAMETER;
	else
	{
		mMethod = RTSP_METHOD_MAX;
		return false;
	}

	if (strncmp(url, "rtsp://", 7) != 0)
	{
		return false;
	}

	uint16_t port = 0;
	char ip[64] = { 0 };
	char suffix[64] = { 0 };

	if (sscanf(url + 7, "%[^:]:%hu/%s", ip, &port, suffix) == 3)
	{

	}
	else if (sscanf(url + 7, "%[^/]/%s", ip, suffix) == 2)
	{
		port = 554;
	}
	else
	{
		return false;
	}

	mUrl = url;
	mSuffix = suffix;

	return true;
}

bool CRtspConnection::parseCSeq(std::string& message)
{
	std::size_t pos = message.find("CSeq");
	if (pos != std::string::npos)
	{
		uint32_t cseq = 0;
		sscanf(message.c_str() + pos, "%*[^:]: %u", &cseq);
		mCSeq = cseq;
		return true;
	}

	return false;
}

bool CRtspConnection::parseAccept(std::string& message)
{
	if ((message.rfind("Accept") == std::string::npos)
		|| (message.rfind("sdp") == std::string::npos))
	{
		return false;
	}

	return true;
}

bool CRtspConnection::parseTransport(std::string& message)
{
	std::size_t pos = message.find("Transport");
	if (pos != std::string::npos)
	{
		if ((pos = message.find("RTP/AVP/TCP")) != std::string::npos)
		{
			uint8_t rtpChannel, rtcpChannel;
			mIsRtpOverTcp = true;

			if (sscanf(message.c_str() + pos, "%*[^;];%*[^;];%*[^=]=%hhu-%hhu",
				&rtpChannel, &rtcpChannel) != 2)
			{
				return false;
			}

			mRtpChannel = rtpChannel;

			return true;
		}
		else if ((pos = message.find("RTP/AVP")) != std::string::npos)
		{
			uint16_t rtpPort = 0, rtcpPort = 0;
			if (((message.find("unicast", pos)) != std::string::npos))
			{
				if (sscanf(message.c_str() + pos, "%*[^;];%*[^;];%*[^=]=%hu-%hu",
					&rtpPort, &rtcpPort) != 2)
				{
					return false;
				}
			}
			else if ((message.find("multicast", pos)) != std::string::npos)
			{
				return true;
			}
			else
				return false;

			mPeerRtpPort = rtpPort;
			mPeerRtcpPort = rtcpPort;
		}
		else
		{
			return false;
		}

		return true;
	}

	return false;
}

bool CRtspConnection::parseMediaTrack()
{
	std::size_t pos = mUrl.find("track0");
	if (pos != std::string::npos)
	{
		mTrackId = MediaSession::TrackId0;
		return true;
	}

	pos = mUrl.find("track1");
	if (pos != std::string::npos)
	{
		mTrackId = MediaSession::TrackId1;
		return true;
	}

	return false;
}

bool CRtspConnection::parseSessionId(std::string& message)
{
	std::size_t pos = message.find("Session");
	if (pos != std::string::npos)
	{
		uint32_t sessionId = 0;
		if (sscanf(message.c_str() + pos, "%*[^:]: %u", &sessionId) != 1)
			return false;
		return true;
	}

	return false;
}

bool CRtspConnection::handleCmdOption()
{
	snprintf(mBuffer, sizeof(mBuffer),
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %u\r\n"
		"Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
		"\r\n", mCSeq);

	if (sendMessage(mBuffer, strlen(mBuffer)) < 0)
		return false;

	return true;
}

bool CRtspConnection::parseRequest2(const char* begin, const char* end)
{
	std::string message(begin, end);

	if (parseCSeq(message) != true)
		return false;

	if (mMethod == RTSP_OPTIONS)
		return true;

	if (mMethod == RTSP_DESCRIBE)
		return parseAccept(message);

	if (mMethod == RTSP_SETUP)
	{
		if (parseTransport(message) != true)
			return false;

		return parseMediaTrack();
	}

	if (mMethod == RTSP_PLAY)
		return parseSessionId(message);

	if (mMethod == RTSP_TEARDOWN)
		return true;
	if (mMethod == RTSP_GET_PARAMETER)
		return true;

	return false;
}

int CRtspConnection::parse_data(const char* data, int len)
{
	const char *start = data;
	const char *end_mark = "\r\n\r\n";
	const char *end = NULL;
	if (NULL == (end = strstr(start, end_mark)))
		return -1;
	int header_len = end - start + strlen(end_mark);
	//解析第一行
	std::string strData(start);
	std::string message = strData.substr(0, header_len);
	
	int ret = parseRequest1(start, start + header_len);

	int iEndPoint = strData.find_last_of("\r\n\r\n");
	if (string::npos != iEndPoint)
	{
		ret = parseRequest2(start, start+iEndPoint);
	}
	else
	{
		ret = -1;
		LogError("parse_data err");
	}
	
	return ret;
}





int CRtspConnection::sendMessage(void* buf, int size)
{
	sockets::send(mSocket.fd(), (char*)buf, size);
	return 1;
}


bool CRtspConnection::handleCmdDescribe()
{
	std::shared_ptr<MediaSession> session = mRtspServer->loopupMediaSession(mSuffix);
	if (!session)
	{
		LogError("can't loop up %s session\n", mSuffix.c_str());
		return false;
	}

	mSession = session;
	std::string sdp = session->generateSDPDescription();

	memset((void*)mBuffer, 0, sizeof(mBuffer));
	snprintf((char*)mBuffer, sizeof(mBuffer),
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %u\r\n"
		"Content-Length: %u\r\n"
		"Content-Type: application/sdp\r\n"
		"\r\n"
		"%s",
		mCSeq,
		(unsigned int)sdp.size(),
		sdp.c_str());

	if (sendMessage(mBuffer, strlen(mBuffer)) < 0)
		return false;

	return true;
}


bool CRtspConnection::handleCmdPlay()
{
	snprintf((char*)mBuffer, sizeof(mBuffer),
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"Range: npt=0.000-\r\n"
		"Session: %08x; timeout=60\r\n"
		"\r\n",
		mCSeq,
		mSessionId);

	if (sendMessage(mBuffer, strlen(mBuffer)) < 0)
		return false;

	for (int i = 0; i < MEDIA_MAX_TRACK_NUM; ++i)
	{
		if (mRtpInstances[i])
			mRtpInstances[i]->setAlive(true);

		if (mRtcpInstances[i])
			mRtcpInstances[i]->setAlive(true);
	}

	return true;
}

bool CRtspConnection::handleCmdTeardown()
{
	snprintf((char*)mBuffer, sizeof(mBuffer),
		"RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"\r\n",
		mCSeq);

	if (sendMessage(mBuffer, strlen(mBuffer)) < 0)
	{
		return false;
	}

	return true;
}

bool CRtspConnection::handleCmdGetParamter()
{
	return true;
}

bool CRtspConnection::handleCmdSetup()
{
	char sessionName[100];
	if (sscanf(mSuffix.c_str(), "%[^/]/", sessionName) != 1)
	{
		return false;
	}
	std::shared_ptr<MediaSession> session = mRtspServer->loopupMediaSession(mSuffix);
	if (!session)
	{
		LogError("can't loop up %s session\n", mSuffix.c_str());
		return false;
	}
	if (mTrackId >= MEDIA_MAX_TRACK_NUM || mRtpInstances[mTrackId] || mRtcpInstances[mTrackId])
		return false;
	if (session->isStartMulticast())
	{
		snprintf((char*)mBuffer, sizeof(mBuffer),
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP;multicast;"
			"destination=%s;source=%s;port=%d-%d;ttl=255\r\n"
			"Session: %08x\r\n"
			"\r\n",
			mCSeq,
			session->getMulticastDestAddr().c_str(),
			sockets::getLocalIp().c_str(),
			session->getMulticastDestRtpPort(mTrackId),
			session->getMulticastDestRtpPort(mTrackId) + 1,
			mSessionId);
	}
	else
	{
		if (mIsRtpOverTcp) //rtp over tcp
		{
			/* 创建rtp over tcp */
			createRtpOverTcp(mTrackId, mSocket.fd(), mRtpChannel);
			mRtpInstances[mTrackId]->setSessionId(mSessionId);
			session->addRtpInstance(mTrackId, mRtpInstances[mTrackId]);

			snprintf((char*)mBuffer, sizeof(mBuffer),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %d\r\n"
				"Transport: RTP/AVP/TCP;unicast;interleaved=%hhu-%hhu\r\n"
				"Session: %08x\r\n"
				"\r\n",
				mCSeq,
				mRtpChannel,
				mRtpChannel + 1,
				mSessionId);
		}
		else //rtp over udp
		{
			if (createRtpRtcpOverUdp(mTrackId, mPeerIp, mPeerRtpPort, mPeerRtcpPort) != true)
			{
				LogDebug("failed to create rtp and rtcp\n");
				return false;
			}

			mRtpInstances[mTrackId]->setSessionId(mSessionId);
			mRtcpInstances[mTrackId]->setSessionId(mSessionId);

			/* 添加到会话中 */
			session->addRtpInstance(mTrackId, mRtpInstances[mTrackId]);

			snprintf((char*)mBuffer, sizeof(mBuffer),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %u\r\n"
				"Transport: RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu\r\n"
				"Session: %08x\r\n"
				"\r\n",
				mCSeq,
				mPeerRtpPort,
				mPeerRtcpPort,
				mRtpInstances[mTrackId]->getLocalPort(),
				mRtcpInstances[mTrackId]->getLocalPort(),
				mSessionId);
		}
	}


	if (sendMessage(mBuffer, strlen(mBuffer)) < 0)
		return false;

	return true;
}



int CRtspConnection::handle_cmd(const char* data, int len)
{
	switch (mMethod)
	{
	case RTSP_OPTIONS:
		if (handleCmdOption() != true)
			goto err;
		break;
	case RTSP_DESCRIBE:
		if (handleCmdDescribe() != true)
			goto err;
		break;
	case RTSP_SETUP:
		if (handleCmdSetup() != true)
			goto err;
		break;
	case RTSP_PLAY:
		if (handleCmdPlay() != true)
			goto err;
		break;
	case RTSP_TEARDOWN:
		if (handleCmdTeardown() != true)
			goto err;
		break;
	case RTSP_GET_PARAMETER:
		if (handleCmdGetParamter() != true)
			goto err;
		break;
	default:
		goto err;
		break;
	err:
		handleDisconnection();
	}


	return 0;

}

RtspMethodT CRtspConnection::parse_method(const char* data, int len)
{
	RtspMethodT rtsp_method = RTSP_METHOD_MAX;
	if (*data == 'O' && strncmp(data, g_method[RTSP_OPTIONS].method_str, strlen(g_method[RTSP_OPTIONS].method_str)) == 0)
		rtsp_method = RTSP_OPTIONS;
	else if (*data == 'D' && strncmp(data, g_method[RTSP_DESCRIBE].method_str, strlen(g_method[RTSP_DESCRIBE].method_str)) == 0)
		rtsp_method = RTSP_DESCRIBE;
	else if (*data == 'S' && strncmp(data, g_method[RTSP_SETUP].method_str, strlen(g_method[RTSP_SETUP].method_str)) == 0)
		rtsp_method = RTSP_SETUP;
	else if (*data == 'P' && strncmp(data, g_method[RTSP_PLAY].method_str, strlen(g_method[RTSP_PLAY].method_str)) == 0)
		rtsp_method = RTSP_PLAY;
	else if (*data == 'P' && strncmp(data, g_method[RTSP_PAUSE].method_str, strlen(g_method[RTSP_PAUSE].method_str)) == 0)
		rtsp_method = RTSP_PAUSE;
	else if (*data == 'T' && strncmp(data, g_method[RTSP_TEARDOWN].method_str, strlen(g_method[RTSP_TEARDOWN].method_str)) == 0)
		rtsp_method = RTSP_TEARDOWN;
	else if (*data == 'S' && strncmp(data, g_method[RTSP_SET_PARAMETER].method_str, strlen(g_method[RTSP_SET_PARAMETER].method_str)) == 0)
		rtsp_method = RTSP_SET_PARAMETER;
	else if (*data == 'G' && strncmp(data, g_method[RTSP_GET_PARAMETER].method_str, strlen(g_method[RTSP_GET_PARAMETER].method_str)) == 0)
		rtsp_method = RTSP_GET_PARAMETER;
	return rtsp_method;
}

void CRtspConnection::handleReadBytes()
{
	char *recv_buf = m_recv_buf;
	m_recv_buf[m_recv_len] = '\0';
	int ret = -1;
	while (m_recv_len > 0) {
		ret = 0;
		if ('$' == *recv_buf) {
			if (m_recv_len <= sizeof(struct RtpTcpHdr))
				break;
			struct RtpTcpHdr* r_t_hd = (struct RtpTcpHdr *)recv_buf;
			uint32_t r_t_len = ntohs(r_t_hd->len);
			if (m_recv_len < r_t_len + 4)
				break;
			;//handle rtcp
			m_recv_len -= r_t_len + 4;
			recv_buf += r_t_len + 4;
		}
		else {
			int parser_ret = parse_data(recv_buf, m_recv_len);
			if (parser_ret < 0)
				break;
			PRINT_RECV_CMD(recv_buf, parser_ret);
			if (handle_cmd(recv_buf, parser_ret) < 0)
				return ;
			m_recv_len -= parser_ret;
			recv_buf += parser_ret;
		}
	}
	if (m_recv_len > 0)
		memmove(m_recv_buf, recv_buf, m_recv_len);
}

bool CRtspConnection::createRtpRtcpOverUdp(MediaSession::TrackId trackId, std::string peerIp,
	uint16_t peerRtpPort, uint16_t peerRtcpPort)
{
	int rtpSockfd, rtcpSockfd;
	int16_t rtpPort, rtcpPort;
	bool ret;

	if (mRtpInstances[trackId] || mRtcpInstances[trackId])
		return false;

	int i;
	for (i = 0; i < 10; ++i)
	{
		rtpSockfd = sockets::createUdpSock();
		if (rtpSockfd < 0)
		{
			return false;
		}

		rtcpSockfd = sockets::createUdpSock();
		if (rtcpSockfd < 0)
		{
			sockets::close(rtpSockfd);
			return false;
		}

		uint16_t port = rand() & 0xfffe;
		if (port < 10000)
			port += 10000;

		rtpPort = port;
		rtcpPort = port + 1;

		ret = sockets::bind(rtpSockfd, "0.0.0.0", rtpPort);
		if (ret != true)
		{
			sockets::close(rtpSockfd);
			sockets::close(rtcpSockfd);
			continue;
		}

		ret = sockets::bind(rtcpSockfd, "0.0.0.0", rtcpPort);
		if (ret != true)
		{
			sockets::close(rtpSockfd);
			sockets::close(rtcpSockfd);
			continue;
		}

		break;
	}

	if (i == 10)
		return false;

	mRtpInstances[trackId] = std::make_shared<RtpInstance>(rtpSockfd, rtpPort,
		peerIp, peerRtpPort);
	mRtcpInstances[trackId] =std::make_shared<RtcpInstance>(rtcpSockfd, rtcpPort,
		peerIp, peerRtcpPort);

	return true;
}

bool CRtspConnection::createRtpOverTcp(MediaSession::TrackId trackId, int sockfd,
	uint8_t rtpChannel)
{
	mRtpInstances[trackId] =std::make_shared<RtpInstance>(sockfd, rtpChannel);

	return true;
}