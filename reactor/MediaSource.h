#pragma once
#include"Def.h"
#include"ThreadPool.h"
#include<queue>
#include<mutex>
#define DEFAULT_FRAME_NUM 4
class AVFrame
{
public:
	AVFrame() :
		mBuffer(new uint8_t[TS_PKT_LEN]),
		mFrameSize(0)
	{ }

	~AVFrame()
	{
		delete mBuffer;
	}

	uint8_t* mBuffer;
	uint8_t* mFrame;
	int mFrameSize;
};
class MediaSource
{
public:
	virtual ~MediaSource();
	AVFrame* getFrame();
	void putFrame(AVFrame* frame);
	int getFps() const { return mFps; }
	virtual void readFrame() = 0;

protected:
	MediaSource();

private:
	static void taskCallback(void*);

protected:
	AVFrame mAVFrames[DEFAULT_FRAME_NUM];
	std::queue<AVFrame*> mAVFrameInputQueue;
	std::queue<AVFrame*> mAVFrameOutputQueue;
	std::mutex mMutex;
	Task* mTask;
	int mFps;
};

