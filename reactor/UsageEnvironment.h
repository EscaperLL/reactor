#pragma once
#include"ThreadPool.h"
#include"CSingle.h"
class UsageEnvironment:public CSingle<UsageEnvironment>
{
	friend class CSingle<UsageEnvironment>;
public:
	UsageEnvironment();
	~UsageEnvironment();

	ThreadPool* threadPool();
private:
	ThreadPool *mThreadPool_{NULL};
};

