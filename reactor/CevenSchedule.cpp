#include "stdafx.h"
#include "CevenSchedule.h"
#include"SelectPoller.h"

CevenSchedule::CevenSchedule()
{
	mPoller = std::make_shared<SelectPoller>();
}


CevenSchedule::~CevenSchedule()
{
}

bool CevenSchedule::addIOEvent(IOEvent *event)
{
	return mPoller->addIOEvent(event);
}

bool CevenSchedule::updateIOEvent(IOEvent *event)
{
	return mPoller->updateIOEvent(event);
}

bool CevenSchedule::removeIOEvent(IOEvent *event)
{
	return mPoller->removeIOEvent(event);
}

bool CevenSchedule::addTriggerEvent(TriggerEvent* event)
{
	mVectTriggerEvents.push_back(std::make_shared<TriggerEvent>(event));
	return true;
}

void CevenSchedule::handleTriggerEvents()
{
	if (!mVectTriggerEvents.empty())
	{
		for (std::vector<std::shared_ptr<TriggerEvent>>::iterator iter;iter != mVectTriggerEvents.end();iter++)
		{
			(*iter)->handleEvent();
		}
		mVectTriggerEvents.clear();
	}
}

void CevenSchedule::loop()
{
	while (bQuit)
	{
		//1
		this->handleTriggerEvents();
		mPoller->handleEvent();
		//2
	}
}
