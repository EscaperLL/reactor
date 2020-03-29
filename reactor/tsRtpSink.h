#pragma once
#include"RtpSink.h"

#include <stdint.h>

class tsRtpSink:public RtpSink
{
public:
	tsRtpSink(MediaSource* mediaSouce);
	virtual ~tsRtpSink();

	virtual std::string getMediaDescription(uint16_t port);
	virtual std::string getAttribute();
	virtual void handleFrame(AVFrame* frame);

private:
	RtpPacket mRtpPacket;
	int mClockRate;
	int mFps;
};

