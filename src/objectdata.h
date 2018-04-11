/**
* Author ZhongLou Yang, 2018-3-22
*/

#pragma once

#include <glm/glm.hpp>

namespace Concise
{
	struct ObjectData
	{
		glm::mat4 model;
		glm::vec3 pos;
		glm::vec3 rotation;
		float rotationDir;
		float rotationSpeed;
		float scale;
		float deltaT;
		float stateT = 0;
		bool visible = true;
	};
}