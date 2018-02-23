#include "observer.h"

namespace Concise
{
	Observer::Observer()
	{}
	
	Observer::~Observer()
	{}
	
	Observer & Observer::Instance()
	{
		static Observer instance;
		return instance;
	}
	
	void Observer::Register(std::string eventName, Callback & callback)
	{
		
	}
}