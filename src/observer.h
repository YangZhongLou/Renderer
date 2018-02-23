/**
* Author ZhongLou Yang, 2018-2-23
*/

#pragma once

#include <string>

namespace Conicse
{
	class Dispatcher;
	//TODO
	typedef Callback;
	/** event observer */
	class Observer
	{
	public:
		static Observer & Instance();
	private:
		Observer();
		~Observer();
		friend class Dispatcher;
	public:
		void Register(std::string eventName, Callback & callback);
	};
}