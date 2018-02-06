/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

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
	};
}