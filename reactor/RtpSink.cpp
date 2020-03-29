#include "stdafx.h"
#include "RtpSink.h"


RtpSink::RtpSink(MediaSource* mediasouce, int playLoadType)
	:mMediaSource(mediasouce),
	mSendPacketCallback(NULL),
	mCsrcLen(0),
	mExtension(0),
	mPadding(0),
	mVersion(RTP_VESION),
	mPayloadType(playLoadType),
	mMarker(1),
	mSeq(0)
{
	mSSRC = rand();
}


RtpSink::~RtpSink()
{
}

void RtpSink::setSendFrameCallback(SendPackCallback cb, void* arg1, void* arg2)
{
	mSendPacketCallback = cb;
	mArg1 = arg1;
	mArg2 = arg2;
}

void RtpSink::sendRtppack(RtpPacket* packet)
{
	RtpHeader* rtpHead = packet->mRtpHeadr;
	rtpHead->csrcLen = mCsrcLen;
	rtpHead->extension = mExtension;
	rtpHead->padding = mPadding;
	rtpHead->version = mVersion;
	rtpHead->payloadType = mPayloadType;
	rtpHead->marker = mMarker;
	rtpHead->seq = htons(mSeq);
	rtpHead->timestamp = htonl(mTimestamp);
	rtpHead->ssrc = htonl(mSSRC);
	packet->mSize += RTP_HEADER_SIZE;

	if (mSendPacketCallback)
		mSendPacketCallback(mArg1, mArg2, packet);
}


void RtpSink::start(int ms)
{
	mTimeUp = ms * 1000;
	mThread = new thread(&RtpSink::run, this);
}

void RtpSink::Stop()
{
	mStop = true;
}

void RtpSink::run()
{
	int iCount = 0;
	while (!mStop)
	{
		if (iCount == mTimeUp)
		{
			AVFrame* pFrame = mMediaSource->getFrame();

			handleFrame(pFrame);

			mMediaSource->putFrame(pFrame);

			iCount = 0;
		}
		else
		{
			iCount++;
		}
	}
}