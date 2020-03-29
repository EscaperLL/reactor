#pragma once
#include<thread>
using std::thread;
class loopThread
{
public:
	loopThread();
	~loopThread();
	virtual void start(int ms);
	void join();
protected:
	virtual void run() =0;
	void* marg;
	thread *mThread;
};

