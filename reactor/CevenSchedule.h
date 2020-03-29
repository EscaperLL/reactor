#pragma once
#include"CSingle.h"
#include"Event.h"
#include"poller.h"
#include<mutex>
#include<vector>
#include<memory>
using std::vector;
class CevenSchedule:public CSingle<CevenSchedule>
{
	friend class CSingle<CevenSchedule>;
public:
	CevenSchedule();
	~CevenSchedule();
	bool addIOEvent(IOEvent* event);
	bool updateIOEvent(IOEvent* event);
	bool removeIOEvent(IOEvent* event);

	bool addTriggerEvent(TriggerEvent* event);

	void loop();

private:
	void handleTriggerEvents();
private:
	std::vector<std::shared_ptr<TriggerEvent>> mVectTriggerEvents;
	std::vector<std::shared_ptr<IOEvent>> mVectEvent;
	bool bQuit{false};
	std::shared_ptr<poller> mPoller;
	std::mutex* mMutex;
};

