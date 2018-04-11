/**
* Author ZhongLou Yang, 2018-3-22
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "objectdata.h"

namespace Concise
{

	struct ThreadPushConstantBlock 
	{
		glm::mat4 mvp;
		glm::vec3 color;
	};

	struct ThreadData 
	{
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffer;
		std::vector<ThreadPushConstantBlock> pushConstBlock;
		std::vector<ObjectData> objectData;
	};
}
