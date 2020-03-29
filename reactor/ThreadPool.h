#pragma once
#include<queue>
#include<vector>
#include<thread>
#include<mutex>
#include<memory>
#include<condition_variable>
class Task
{
public:
	typedef void(*TaskCallback)(void*);
	Task() { };

	void setTaskCallback(TaskCallback cb, void* arg) {
		mTaskCallback = cb; mArg = arg;
	}

	void handle() {
		if (mTaskCallback)
			mTaskCallback(mArg);
	}

	bool operator=(const Task& task) {
		this->mTaskCallback = task.mTaskCallback;
		this->mArg = task.mArg;
	}
private:
	void(*mTaskCallback)(void*);
	void* mArg;
};

class ThreadPool
{
public:
	ThreadPool(int num);
	~ThreadPool();

	void addTask(Task* task);
private:
	void run();
	void handleTask();
private:

	std::queue<Task*> taskQueue_;

	std::vector<std::shared_ptr<std::thread>> threads_;
	std::mutex mutex_;
	std::condition_variable condition_;
	bool bQuit{ false };
};

