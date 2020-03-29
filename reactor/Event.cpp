#include "stdafx.h"
#include "Event.h"


TriggerEvent::TriggerEvent(void* arg)
	:mArg(arg)
{

}

void TriggerEvent::handleEvent()
{
	if (mTriggerCallback)
		mTriggerCallback(mArg);
}

//IOEvent* IOEvent::createNew(int fd, void* arg)
//{
//	if (fd < 0)
//		return NULL;
//
//	return new IOEvent(fd, arg);
//	//return New<IOEvent>::allocate(fd, arg);
//}
//
//IOEvent* IOEvent::createNew(int fd)
//{
//	if (fd < 0)
//		return NULL;
//
//	return new IOEvent(fd, NULL);
//	//return New<IOEvent>::allocate(fd, (void*)0);
//}

IOEvent::IOEvent(int fd, void* arg) :
	mFd(fd),
	mArg(arg),
	mEvent(EVENT_NONE),
	mREvent(EVENT_NONE),
	mReadCallback(NULL),
	mWriteCallback(NULL),
	mErrorCallback(NULL)
{

}

void IOEvent::handleEvent()
{
	if (mReadCallback && (mREvent & EVENT_READ))
	{
		mReadCallback(mArg);
	}

	if (mWriteCallback && (mREvent & EVENT_WRITE))
	{
		mWriteCallback(mArg);
	}

	if (mErrorCallback && (mREvent & EVENT_ERROR))
	{
		mErrorCallback(mArg);
	}
};