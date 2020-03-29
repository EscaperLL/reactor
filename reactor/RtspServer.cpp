#include "stdafx.h"
#include "RtspServer.h"
#include"Guard.h"
#include"CevenSchedule.h"

RtspServer::RtspServer(const Ipv4Address& addr)
	:tcpServer(addr)
{
	mTriggerEvent = std::make_shared<TriggerEvent>(this);
	mTriggerEvent->setTriggerCallback(triggerCallback);
}


RtspServer::~RtspServer()
{
}

void RtspServer::handleNewConnection(int connfd)
{
	std::shared_ptr<CRtspConnection>conn = std::make_shared<CRtspConnection>(this,connfd);
	conn->setDisconnectionCallback(disconnectionCallback, this);
	mConnections.insert(std::make_pair(connfd, conn));
}

void RtspServer::disconnectionCallback(void* arg, int sockfd)
{
	RtspServer* server = static_cast<RtspServer*>(arg);
	if (server)
	{
		server->handleDisconnection(sockfd);

	}
}

void RtspServer::handleDisconnection(int sockfd)
{
	CGuard guard(mMutex);
	mDisConnectionList.push_back(sockfd);

	CevenSchedule::getInstance()->addTriggerEvent(mTriggerEvent.get());
}

bool RtspServer::addMeidaSession(std::shared_ptr<MediaSession> mediaSession)
{
	if (mMediaSessions.find(mediaSession->name()) != mMediaSessions.end())
		return false;

	mMediaSessions.insert(std::make_pair(mediaSession->name(), mediaSession));

	return true;
}

std::shared_ptr<MediaSession>  RtspServer::loopupMediaSession(std::string name)
{
	std::map<std::string, std::shared_ptr<MediaSession> >::iterator it = mMediaSessions.find(name);
	if (it == mMediaSessions.end())
		return NULL;

	return it->second;
}

std::string RtspServer::getUrl(MediaSession* session)
{
	char url[200];

	snprintf(url, sizeof(url), "rtsp://%s:%d/%s", sockets::getLocalIp().c_str(),
		mAddr.getPort(), session->name().c_str());

	return std::string(url);
}

void RtspServer::triggerCallback(void* arg)
{
	RtspServer* server = static_cast<RtspServer*>(arg);
	if (server)
	{
		server->handleDisconnectionList();
	}
}

void RtspServer::handleDisconnectionList()
{
	CGuard guard(mMutex);

	for (std::vector<int>::iterator it = mDisConnectionList.begin(); it != mDisConnectionList.end(); ++it)
	{
		int sockfd = *it;
		std::map<int, std::shared_ptr<CRtspConnection>>::iterator _it = mConnections.find(sockfd);
		

		mConnections.erase(sockfd);
	}

	mDisConnectionList.clear();
}