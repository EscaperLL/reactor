#include "stdafx.h"
#include "loopThread.h"


loopThread::loopThread()
	:marg(NULL),mThread(NULL)
{
}


loopThread::~loopThread()
{
}

void loopThread::join()
{
	mThread->join();
}

void loopThread::start(int ms)
{
	mThread = new thread(&loopThread::run, this);
}
