#include "stdafx.h"
#include "Guard.h"


CGuard::CGuard(std::mutex &mu)
	:m_pMutex(&mu)
{
	m_pMutex->lock();
}


CGuard::~CGuard()
{
	m_pMutex->unlock();
}
