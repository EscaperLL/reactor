#include "stdafx.h"
#include "RtpInstance.h"





RtpInstance::~RtpInstance()
{
	sockets::close(mSockfd);
}


