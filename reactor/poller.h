#pragma once
#include"Event.h"
#include<map>
#include<memory>
using std::map;
class poller
{
public:
	virtual ~poller();
	virtual bool addIOEvent(IOEvent*) = 0;
	virtual bool updateIOEvent(IOEvent*) = 0;
	virtual bool removeIOEvent(IOEvent*) = 0;
	virtual void handleEvent() = 0;
protected:
	poller();
protected:
	typedef map<int, std::shared_ptr<IOEvent>> IOEventMap;
	IOEventMap mEventMap;

};

