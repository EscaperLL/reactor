// reactor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<WinSock2.h>
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif
#include"test.h"
#include"CevenSchedule.h"
#include"Event.h"
#include"RtspServer.h"
#include"MediaSession.h"
#include"Ipv4Address.h"
#include"TsFileMediaSource.h"
#include"tsRtpSink.h"
#include"UsageEnvironment.h"
#include<memory>
#include<iostream>

int un_init_environment()
{
#ifdef _WIN32
	if (WSACleanup() == SOCKET_ERROR)
		return -1;
#endif
	return 0;
}

int main()
{
	
	ThreadPool *threadPool = new ThreadPool(2);

	Ipv4Address ipAddr("0.0.0.0", 8554);

	RtspServer *server = new RtspServer(ipAddr);
	shared_ptr<MediaSession> session = std::make_shared< MediaSession>("live");
	MediaSource *mediaSource = new CTsFileMediaSource("D:\\1.ts");
	shared_ptr<RtpSink> rtpSink = std::make_shared< tsRtpSink>(mediaSource);
	UsageEnvironment::getInstance()->threadPool();
	session->addRtpSink(MediaSession::TrackId0,rtpSink);
	server->addMeidaSession(session);
	server->start();
	std::cout << "Play the media using the URL \"" << server->getUrl(session.get()) << "\"" << std::endl;
	CevenSchedule::getInstance()->loop();
    return 0;
}

