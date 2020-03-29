#include "stdafx.h"
#include "MediaSource.h"
#include"Guard.h"
#include"UsageEnvironment.h"

MediaSource::MediaSource()
{
	for (size_t i = 0; i < DEFAULT_FRAME_NUM; i++)
	{
		mAVFrameInputQueue.push(&mAVFrames[i]);

	}
	mTask = new Task();
	mTask->setTaskCallback(taskCallback, this);
}


MediaSource::~MediaSource()
{
}


AVFrame* MediaSource::getFrame()
{
	CGuard cuard(mMutex);
	if (mAVFrameOutputQueue.empty())
	{
		return NULL;
	}
	AVFrame* frame = mAVFrameOutputQueue.front();
	mAVFrameOutputQueue.pop();
	return frame;
}

void MediaSource::putFrame(AVFrame* frame)
{
	{
		CGuard cuard(mMutex);
		mAVFrameInputQueue.push(frame);
	}
	UsageEnvironment::getInstance()->threadPool()->addTask(mTask);
}

void MediaSource::taskCallback(void* arg)
{
	MediaSource *pMediaSource = static_cast<MediaSource*>(arg);
	if (pMediaSource)
	{
		pMediaSource->readFrame();
	}
}