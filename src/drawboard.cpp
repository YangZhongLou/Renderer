#include <iostream>
#include "drawboard.h"
#include "global.h"


namespace Concise
{
	Drawboard::Drawboard()
	{}
	
	Drawboard::~Drawboard()
	{
	}
	
	HWND Drawboard::Create(HINSTANCE hinstance, WNDPROC wndproc, std::string name, std::string windowTitle)
	{
		m_windowInstance = hinstance;

		WNDCLASSEX wndClass;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = wndproc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hinstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = name.c_str();
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			std::cout << "Could not register window class!\n";
			fflush(stdout);
			exit(1);
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (g_settings.fullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = screenWidth;
			dmScreenSettings.dmPelsHeight = screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if ((m_width != (UInt32)screenWidth) && (m_height != (UInt32)screenHeight))
			{
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					{
						g_settings.fullscreen = false;
					}
					else
					{
						return nullptr;
					}
				}
			}

		}

		DWORD dwExStyle;
		DWORD dwStyle;

		if (g_settings.fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = g_settings.fullscreen ? (long)screenWidth : (long)m_width;
		windowRect.bottom = g_settings.fullscreen ? (long)screenHeight : (long)m_height;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		m_window = CreateWindowEx(0,
			name.c_str(),
			windowTitle.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			hinstance,
			NULL);

		if (!g_settings.fullscreen)
		{
			// Center on screen
			UInt32 x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			UInt32 y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(m_window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		if (!m_window)
		{
			printf("Could not create window!\n");
			fflush(stdout);
			return nullptr;
			exit(1);
		}

		ShowWindow(m_window, SW_SHOW);
		SetForegroundWindow(m_window);
		SetFocus(m_window);

		return m_window;
	}
	
}