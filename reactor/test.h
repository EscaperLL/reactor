#pragma once
#include"loopThread.h"
class test:public loopThread
{
public:
	test();
	~test();
protected:
	virtual void run();
};

