/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <windows.h>
#include <glm/glm.hpp>
#include "types.h"

namespace Concise
{
	class Inputs
	{
	private:
		struct 
		{
			bool left = false;
			bool right = false;
			bool middle = false;
		} m_mouseButtons;
		
		float m_zoom = -2.5;

		float m_rotationSpeed = 1.0f;
		float m_zoomSpeed = 1.0f;
		bool m_viewUpdated;

		glm::vec3 m_rotation = glm::vec3();
		glm::vec3 m_cameraPos = glm::vec3();
		glm::vec2 m_mousePos;

		UInt32 m_destWidth;
		UInt32 m_destHeight;
		bool m_resizing;

	public:
		static Inputs & Instance() 
		{
			static Inputs instance;
			return instance;
		}
	public:
		Inputs();
		~Inputs();
	public:
		void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void KeyPressed(UInt32) {}
		void HandleMouseMove(Int32 x, Int32 y);
		void MouseMoved(double x, double y, bool handled) {}
		float GetZoom() const { return m_zoom; };
		glm::vec3 GetRotation() const { return m_rotation; }
	};
}