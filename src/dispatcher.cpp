#include "dispatcher.h"

namespace Concise
{
	Dispatcher::Dispatcher()
	{}
	
	Dispatcher::~Dispatcher()
	{}
	
	Dispatcher & Dispatcher::Instance()
	{
		static Dispatcher instance;
		return instance;
	}
	
	void Dispatcher::Dispatch(std::string eventName)
	{
		//TODO
	}
	
}