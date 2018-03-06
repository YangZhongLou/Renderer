#include "pipelines.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.hpp"
#include "shader.h"

namespace Concise
{
	Pipelines::Pipelines()
	{		
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		VK_CHECK_RESULT(vkCreatePipelineCache(Device::Instance().GetLogicalDevice(),
			&pipelineCacheCreateInfo, nullptr, &m_pipelineCache));

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
			VkFactory::PipelineInputAssemblyStateCreateInfo(
				0,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FALSE);

		VkPipelineRasterizationStateCreateInfo rasterizationState =
			VkFactory::PipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_CLOCKWISE,
				0);

		VkPipelineColorBlendAttachmentState blendAttachmentState =
			VkFactory::PipelineColorBlendAttachmentState(
				0xf,
				VK_FALSE);

		VkPipelineColorBlendStateCreateInfo colorBlendState =
			VkFactory::PipelineColorBlendStateCreateInfo(
				1,
				&blendAttachmentState);

		VkPipelineDepthStencilStateCreateInfo depthStencilState =
			VkFactory::PipelineDepthStencilStateCreateInfo();

		VkPipelineViewportStateCreateInfo viewportState =
			VkFactory::PipelineViewportStateCreateInfo(1, 1);

		VkPipelineMultisampleStateCreateInfo multisampleState =
			VkFactory::PipelineMultisampleStateCreateInfo(
				VK_SAMPLE_COUNT_1_BIT);

		std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState =
			VkFactory::PipelineDynamicStateCreateInfo(
				dynamicStateEnables.size(),
				dynamicStateEnables.data());

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		shaderStages[0] = loadShader(getAssetPath() + "shaders/multithreading/phong.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[1] = loadShader(getAssetPath() + "shaders/multithreading/phong.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
			vks::initializers::pipelineCreateInfo(
				pipelineLayout,
				renderPass,
				0);

		pipelineCreateInfo.pVertexInputState = &vertices.inputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.stageCount = shaderStages.size();
		pipelineCreateInfo.pStages = shaderStages.data();

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.phong));

	}
	
	Pipelines::~Pipelines()
	{
		
	}
}