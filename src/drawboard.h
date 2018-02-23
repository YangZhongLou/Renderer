 /**
* Author ZhongLou Yang, 2018-2-5
*/

#prgama once

#include <string>

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
		HWND CreateWindow(HINSTANCE hinstance, WNDPROC wndproc, std::string name, std::string windowTitle);
		
		void WindowResize();
		void WindowResized();
		void ViewChanged();
	};
}