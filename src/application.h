/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <string>

namespace Concise
{
	class Application
	{
	private:
		std::string m_name;
	public:
		Application(std::string name);
		~Application();
	};
}