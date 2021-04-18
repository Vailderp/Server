#pragma once
#include <iostream>
namespace vl
{
	template <class T>
	void err(const char* error)
	{
		std::cout << "Error"<< " >> " << "vl" <<  " >> " << typeid(T).raw_name() <<  " >> " << error << ";" << std::endl;
	}

	template <class T>
	void err(const char* error, const bool banned)
	{
		if (!banned)
		{
			err<T>(error);
		}
	}
	
	template <class T>
	void done(const char* done)
	{
		std::cout << "Done" << " >> " << "vl" << " >> " << typeid(T).raw_name() << " >> " << done << ";" << std::endl;
	}

	template <class T>
	void done(const char* done, const bool banned)
	{
		if (!banned)
		{
			err<T>(done);
		}
	}
}


