#include "stdafx.h"
#include "tsRtpSink.h"

#include <stdio.h>
#include <string.h>

tsRtpSink::tsRtpSink(MediaSource* mediaSouce)
	:RtpSink(mediaSouce, RTP_PAYLOAD_TS),
	mClockRate(90000),
	mFps(mediaSouce->getFps())
{
	start(1000 / mFps);
}


tsRtpSink::~tsRtpSink()
{
}


std::string tsRtpSink::getMediaDescription(uint16_t port)
{
	char buf[100] = { 0 };
	sprintf(buf, "m=video %hu RTP/AVP %d", port, mPayloadType);

	return std::string(buf);
}

std::string tsRtpSink::getAttribute()
{
	char buf[100];
	sprintf(buf, "a=rtpmap:%d H264/%d\r\n", mPayloadType, mClockRate);
	sprintf(buf + strlen(buf), "a=framerate:%d", mFps);

	return std::string(buf);
}

void tsRtpSink::handleFrame(AVFrame* frame)
{
	RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;
	uint8_t naluType = frame->mFrame[0];

	if (frame->mFrameSize <= RTP_MAX_PKT_SIZE)
	{
		memcpy(rtpHeader->payload, frame->mFrame, frame->mFrameSize);
		mRtpPacket.mSize = frame->mFrameSize;
		sendRtppack(&mRtpPacket);
		mSeq++;

		if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // �����SPS��PPS�Ͳ���Ҫ��ʱ���
			return;
	}
	else
	{
		int pktNum = frame->mFrameSize / RTP_MAX_PKT_SIZE;       // �м��������İ�
		int remainPktSize = frame->mFrameSize % RTP_MAX_PKT_SIZE; // ʣ�಻�������Ĵ�С
		int i, pos = 1;

		/* ���������İ� */
		for (i = 0; i < pktNum; i++)
		{
			/*
			*     FU Indicator
			*    0 1 2 3 4 5 6 7
			*   +-+-+-+-+-+-+-+-+
			*   |F|NRI|  Type   |
			*   +---------------+
			* */
			rtpHeader->payload[0] = (naluType & 0x60) | 28; //(naluType & 0x60)��ʾnalu����Ҫ�ԣ�28��ʾΪ��Ƭ

															/*
															*      FU Header
															*    0 1 2 3 4 5 6 7
															*   +-+-+-+-+-+-+-+-+
															*   |S|E|R|  Type   |
															*   +---------------+
															* */
			rtpHeader->payload[1] = naluType & 0x1F;

			if (i == 0) //��һ������
				rtpHeader->payload[1] |= 0x80; // start
			else if (remainPktSize == 0 && i == pktNum - 1) //���һ������
				rtpHeader->payload[1] |= 0x40; // end

			memcpy(rtpHeader->payload + 2, frame->mFrame + pos, RTP_MAX_PKT_SIZE);
			mRtpPacket.mSize = RTP_MAX_PKT_SIZE + 2;
			sendRtppack(&mRtpPacket);

			mSeq++;
			pos += RTP_MAX_PKT_SIZE;
		}

		/* ����ʣ������� */
		if (remainPktSize > 0)
		{
			rtpHeader->payload[0] = (naluType & 0x60) | 28;
			rtpHeader->payload[1] = naluType & 0x1F;
			rtpHeader->payload[1] |= 0x40; //end

			memcpy(rtpHeader->payload + 2, frame->mFrame + pos, remainPktSize);
			mRtpPacket.mSize = remainPktSize + 2;
			sendRtppack(&mRtpPacket);

			mSeq++;
		}
	}

	mTimestamp += mClockRate / mFps;
}