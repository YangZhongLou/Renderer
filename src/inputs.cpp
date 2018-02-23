#include "inputs.h"

namespace Conise
{
	Inputs::Inputs()
	{
	}
	
	Inputs::~Inputs()
	{
	}
	
	void Inputs::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			m_prepared = false;
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			ValidateRect(m_window, NULL);
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case KEY_P:
				m_paused = !m_paused;
				break;
			case KEY_F1:
				if (m_settings.overlay) 
				{
					/** TODO */
				}
				break;
			case KEY_ESCAPE:
				PostQuitMessage(0);
				break;
			}

			if (m_camera.firstperson)
			{
				switch (wParam)
				{
				case KEY_W:
					m_camera.keys.up = true;
					break;
				case KEY_S:
					m_camera.keys.down = true;
					break;
				case KEY_A:
					m_camera.keys.left = true;
					break;
				case KEY_D:
					m_camera.keys.right = true;
					break;
				}
			}

			KeyPressed((UInt32)wParam);
			break;
		case WM_KEYUP:
			if (m_camera.firstperson)
			{
				switch (wParam)
				{
				case KEY_W:
					m_camera.keys.up = false;
					break;
				case KEY_S:
					m_camera.keys.down = false;
					break;
				case KEY_A:
					m_camera.keys.left = false;
					break;
				case KEY_D:
					m_camera.keys.right = false;
					break;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.left = true;
			break;
		case WM_RBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.right = true;
			break;
		case WM_MBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.middle = true;
			break;
		case WM_LBUTTONUP:
			m_mouseButtons.left = false;
			break;
		case WM_RBUTTONUP:
			m_mouseButtons.right = false;
			break;
		case WM_MBUTTONUP:
			m_mouseButtons.middle = false;
			break;
		case WM_MOUSEWHEEL:
		{
			short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			m_zoom += (float)wheelDelta * 0.005f * m_zoomSpeed;
			m_camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f * m_zoomSpeed));
			m_viewUpdated = true;
			break;
		}
		case WM_MOUSEMOVE:
		{
			HandleMouseMove(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_SIZE:
			if ((m_prepared) && (wParam != SIZE_MINIMIZED))
			{
				if ((m_resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
				{
					m_destWidth = LOWORD(lParam);
					m_destHeight = HIWORD(lParam);
					WindowResize();
				}
			}
			break;
		case WM_ENTERSIZEMOVE:
			m_resizing = true;
			break;
		case WM_EXITSIZEMOVE:
			m_resizing = false;
			break;
		}
	}
	
	void Inputs::HandleMouseMove(Int32 x, Int32 y)
	{
		Int32 dx = (Int32)m_mousePos.x - x;
		Int32 dy = (Int32)m_mousePos.y - y;

		bool handled = false;

		MouseMoved((float)x, (float)y, handled);

		if (handled) 
		{
			m_mousePos = glm::vec2((float)x, (float)y);
			return;
		}

		if (m_mouseButtons.left) 
		{
			m_rotation.x += dy * 1.25f * m_rotationSpeed;
			m_rotation.y -= dx * 1.25f * m_rotationSpeed;
			m_camera.rotate(glm::vec3(dy * m_camera.rotationSpeed, -dx * m_camera.rotationSpeed, 0.0f));
			m_viewUpdated = true;
		}
		if (m_mouseButtons.right) 
		{
			m_zoom += dy * .005f * m_zoomSpeed;
			m_camera.translate(glm::vec3(-0.0f, 0.0f, dy * .005f * m_zoomSpeed));
			m_viewUpdated = true;
		}
		if (m_mouseButtons.middle) 
		{
			m_cameraPos.x -= dx * 0.01f;
			m_cameraPos.y -= dy * 0.01f;
			m_camera.translate(glm::vec3(-dx * 0.01f, -dy * 0.01f, 0.0f));
			m_viewUpdated = true;
		}
		m_mousePos = glm::vec2((float)x, (float)y);
	}
}