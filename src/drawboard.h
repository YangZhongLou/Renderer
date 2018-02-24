 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <windows.h>
#include <string>
#include "types.h"

namespace Concise
{
	/** abstraction of window */
	class Drawboard
	{
	private:
		HWND m_window;
		HINSTANCE m_windowInstance;
				
		UInt32 m_width = 1280;
		UInt32 m_height = 720;
	public:
		Drawboard();
		~Drawboard(); 
	public:
		HWND Create(HINSTANCE hinstance, WNDPROC wndproc, std::string name, std::string windowTitle);
		
		UInt32 GetWidth() const { return m_width; }
		UInt32 GetHeight() const { return m_height; }
	};
}