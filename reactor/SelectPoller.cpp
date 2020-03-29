#include "stdafx.h"
#include "SelectPoller.h"


SelectPoller* SelectPoller::creatNew()
{
	return new SelectPoller();
}

SelectPoller::SelectPoller()
	:mMaxNumSockets(0)
{
	FD_ZERO(&mReadSet);
	FD_ZERO(&mWriteSet);
	FD_ZERO(&mExceptionSet);
}


SelectPoller::~SelectPoller()
{
}

bool SelectPoller::addIOEvent(IOEvent* event)
{
	updateIOEvent(event);
	return true;
}

bool SelectPoller::updateIOEvent(IOEvent* event)
{
	int ifd = event->getFd();
	if (ifd < 0)
	{
		//log
		return false;
	}

	FD_CLR(ifd, &mReadSet);
	FD_CLR(ifd, &mWriteSet);
	FD_CLR(ifd, &mExceptionSet);

	IOEventMap::iterator iter = mEventMap.find(ifd);
	if (iter != mEventMap.end())
	{
		//有事件
		if (event->isReadHandling())
		{
			FD_SET(ifd, &mReadSet);
		}
		if (event->isWriteHandling())
		{
			FD_SET(ifd, &mWriteSet);
		}
		if (event->isErrorHandling())
		{
			FD_SET(ifd, &mExceptionSet);
		}
	}
	else
	{
		if (event->isReadHandling())
		{
			FD_SET(ifd, &mReadSet);
		}
		if (event->isWriteHandling())
		{
			FD_SET(ifd, &mWriteSet);
		}
		if (event->isErrorHandling())
		{
			FD_SET(ifd, &mExceptionSet);
		}
		mEventMap.insert(std::make_pair(ifd, std::shared_ptr<IOEvent>(event)));
	}
	if (mEventMap.empty())
	{
		mMaxNumSockets = 0;
	}
	else
	{
		mMaxNumSockets = mEventMap.rbegin()->first + 1;
	}
	return true;
}

bool SelectPoller::removeIOEvent(IOEvent* event)
{
	int ifd = event->getFd();
	if (ifd < 0)
	{
		return false;
	}
	FD_CLR(ifd, &mReadSet);
	FD_CLR(ifd, &mWriteSet);
	FD_CLR(ifd, &mExceptionSet);

	IOEventMap::iterator iter = mEventMap.find(ifd);
	if (iter != mEventMap.end())
	{
		mEventMap.erase(iter);
	}
	if (mEventMap.empty())
	{
		mMaxNumSockets = 0;
	}
	else
	{
		mMaxNumSockets = mEventMap.rbegin()->first + 1;
	}
	return true;
}

void SelectPoller::handleEvent()
{
	int rEvent;
	fd_set readSet = mReadSet;
	fd_set writeSet = mWriteSet;
	fd_set exceptionSet = mExceptionSet;
	struct timeval tmv_timeout = { 0L, 1000000L };//单位微秒，默认1秒超时
	int ret = select(mMaxNumSockets, &mReadSet, &mWriteSet, &mExceptionSet, &tmv_timeout);
	if (0 > ret)
	{
		//log
		return;
	}

	for (auto iter:mEventMap)
	{
		rEvent = 0;
		int iFd = iter.first;
		if (FD_ISSET(iFd, &readSet))
			rEvent |= IOEvent::EVENT_READ;

		if (FD_ISSET(iFd, &writeSet))
			rEvent |= IOEvent::EVENT_WRITE;

		if (FD_ISSET(iFd, &exceptionSet))
			rEvent |= IOEvent::EVENT_ERROR;
		if (rEvent != 0)
		{
			iter.second->setREvent(rEvent);
			mEvent.push_back(iter.second);
		}
	}

	for (auto iter: mEvent)
	{
		iter->handleEvent();
	}
	mEvent.clear();
}
