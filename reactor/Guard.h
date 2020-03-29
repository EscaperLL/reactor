#pragma once
#include<mutex>
class CGuard
{
public:
	CGuard(std::mutex &mu);
	~CGuard();
private:
	std::mutex *m_pMutex;
};

