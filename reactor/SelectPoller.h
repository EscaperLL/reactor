#pragma once
#include"poller.h"
#include <WinSock2.h>
#include<vector>
class SelectPoller :public poller
{
public:

	static SelectPoller* creatNew();

	SelectPoller();
	virtual ~SelectPoller();
	virtual bool addIOEvent(IOEvent*);
	virtual bool updateIOEvent(IOEvent*);
	virtual bool removeIOEvent(IOEvent*);
	virtual void handleEvent();
private:
	fd_set mReadSet;
	fd_set mWriteSet;
	fd_set mExceptionSet;
	int mMaxNumSockets;
	std::vector<std::shared_ptr<IOEvent>> mEvent;
};

