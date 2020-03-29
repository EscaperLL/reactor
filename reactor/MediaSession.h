#pragma once

#include"RtpInstance.h"
#include"RtpSink.h"

#include<list>
#include<memory>
#define MEDIA_MAX_TRACK_NUM 2

class MediaSession
{
public:

	enum TrackId
	{
		TrackIdNone = -1,
		TrackId0 = 0,
		TrackId1 = 1,
	};

	MediaSession(const std::string& sessionName);
	~MediaSession();

	std::string name() const { return mSessionName; }
	std::string generateSDPDescription();
	bool addRtpSink(MediaSession::TrackId trackId, std::shared_ptr<RtpSink> rtpsink);
	bool addRtpInstance(MediaSession::TrackId trackId, std::shared_ptr< RtpInstance>rtpInstance);
	bool removeRtpInstance(std::shared_ptr<RtpInstance> rtpInstance);
	bool startMulticast();
	bool isStartMulticast();
	std::string getMulticastDestAddr() const { return mMulticastAddr; }
	uint16_t getMulticastDestRtpPort(TrackId trackId);
private:
	class Track
	{
	public:
		std::shared_ptr<RtpSink> mRtpSink;
		int mTrackId;
		bool mIsAlive;
		std::list<std::shared_ptr<RtpInstance>> mRtpInstances;
	};
	Track* getTrack(MediaSession::TrackId trackId);
	static void sendPacketCallback(void* arg1, void* arg2, RtpPacket* rtpPacket);
	void sendPacket(MediaSession::Track* tarck, RtpPacket* rtpPacket);


private:
	std::string mSessionName;
	std::string mSdp;
	Track mTracks[MEDIA_MAX_TRACK_NUM];
	bool mIsStartMulticast;
	std::string mMulticastAddr;
	std::shared_ptr<RtpInstance> mMulticastRtpInstances[MEDIA_MAX_TRACK_NUM];
	std::shared_ptr<RtcpInstance> mMulticastRtcpInstances[MEDIA_MAX_TRACK_NUM];
};

