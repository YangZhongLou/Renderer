#include "application.h"
#include <windows.h>

namespace Concise
{
	Application::Application(std::string name)
	{
		m_name = name;

#if defined(WIN32)
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *stream;
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		freopen_s(&stream, "CONOUT$", "w+", stderr);

		SetConsoleTitle(TEXT(name.c_str()));
#endif
	}
	
	Application::~Application()
	{}
}