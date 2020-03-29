#pragma once
#include<memory>
template<class T>
class CSingle
{
public:
	static std::shared_ptr<T> getInstance()
	{
		if (NULL == mInstance)
		{
			mInstance = std::make_shared<T>();
		}
		return mInstance;
	}

protected:
	CSingle() {};
	~CSingle() {};
	static std::shared_ptr<T> mInstance;
};

template<class T>
std::shared_ptr<T> CSingle<T>::mInstance = NULL;
