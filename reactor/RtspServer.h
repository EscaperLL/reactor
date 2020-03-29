#pragma once

#include"tcpServer.h"
#include"MediaSession.h"
#include"RtspConnection.h"

#include"Ipv4Address.h"
#include<map>
#include<vector>
#include<mutex>
class RtspServer:public tcpServer
{
public:
	RtspServer(const Ipv4Address& addr);
	virtual ~RtspServer();

	bool addMeidaSession(std::shared_ptr<MediaSession> mediaSession);

	std::shared_ptr<MediaSession> loopupMediaSession(std::string name);
	std::string getUrl(MediaSession* session);
protected:
	virtual void handleNewConnection(int confd);
	static void disconnectionCallback(void* arg, int sockfd);
	void handleDisconnection(int sockfd);
	static void triggerCallback(void*);
	void handleDisconnectionList();

private:
	std::map<std::string, std::shared_ptr<MediaSession>> mMediaSessions;
	std::map<int, std::shared_ptr<CRtspConnection>> mConnections;
	std::vector<int> mDisConnectionList;

	std::shared_ptr<TriggerEvent> mTriggerEvent;
	std::mutex mMutex;
};

