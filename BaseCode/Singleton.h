#pragma once
#include <boost/pool/detail/singleton.hpp>

template<class T>
class Singleton
{
public:
	static T &GetInstance()
	{
		return boost::details::pool::singleton_default<T>::instance();
	}
};
