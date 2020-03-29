#include "stdafx.h"
#include "UsageEnvironment.h"


UsageEnvironment::UsageEnvironment()
{
	mThreadPool_ = new ThreadPool(4);
}


UsageEnvironment::~UsageEnvironment()
{
	if (mThreadPool_)
	{
		delete mThreadPool_;
	}
}

ThreadPool* UsageEnvironment::threadPool()
{
	return mThreadPool_;
}
