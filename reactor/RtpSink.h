#pragma once
#include"MediaSource.h"

#include"Event.h"
#include"Rtp.h"

#include"loopThread.h"
#include<memory>
using std::shared_ptr;
class RtpSink:public loopThread
{
public:
	typedef void (*SendPackCallback)(void* arg1,void* arg2,RtpPacket* mediaPacket);
	RtpSink(MediaSource* mediasouce,int playLoadType);
	virtual ~RtpSink();

	virtual std::string getMediaDescription(uint16_t port) = 0;
	virtual std::string getAttribute() = 0;

	void setSendFrameCallback(SendPackCallback cb, void* arg1, void* arg2);

protected:

	virtual void handleFrame(AVFrame* frame) = 0;
	void sendRtppack(RtpPacket* pack);
    virtual void start(int ms);
	void Stop();


	virtual void run();
private:
	static void timeoutCallback(void*);

protected:
	std::shared_ptr<MediaSource> mMediaSource;
	SendPackCallback mSendPacketCallback;

	void* mArg1;
	void* mArg2;

	uint8_t mCsrcLen;
	uint8_t mExtension;
	uint8_t mPadding;
	uint8_t mVersion;
	uint8_t mPayloadType;
	uint8_t mMarker;
	uint16_t mSeq;
	uint32_t mTimestamp;
	uint32_t mSSRC;

	int mTimeUp;
	bool mStop{false};
};

