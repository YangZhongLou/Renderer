/**
* Author ZhongLou Yang, 2018-2-23
*/

#pragma once

#include <string>

namespace Concise
{
	class Observer;
	/** event dispatcher */
	class Dispatcher
	{
	public:
		static Dispatcher & Instance();
	private:
		Dispatcher();
		~Dispatcher();
	public:
		void Dispatch(std::string eventName);
	};
}