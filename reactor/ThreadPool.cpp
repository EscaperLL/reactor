#include "stdafx.h"
#include "ThreadPool.h"
#include"Guard.h"
using std::thread;
ThreadPool::ThreadPool(int num)
	:threads_(num)
{
	CGuard guard(mutex_);
	for (size_t i = 0; i < num; i++)
	{
		std::shared_ptr< std::thread> pThread = std::make_shared<thread>(&ThreadPool::run, this);
		threads_.push_back(pThread);
	}
}


ThreadPool::~ThreadPool()
{

	threads_.clear();
}

void ThreadPool::addTask(Task* task)
{
	CGuard guard(mutex_);
	taskQueue_.push(task);
	condition_.notify_one();
}

void ThreadPool::run()
{
	handleTask();
}

void ThreadPool::handleTask()
{
	while (!bQuit)
	{
		Task *task;
		{
			CGuard guard(mutex_);
			if (taskQueue_.empty())
			{
				std::unique_lock <std::mutex> lck(mutex_);
				condition_.wait(lck);
			}
			if (bQuit)
			{
				break;
			}
			if (taskQueue_.empty())
			{
				continue;
			}
			task = taskQueue_.front();
			taskQueue_.pop();
		}
		if (task)
		{
			task->handle();
		}

	}
}
