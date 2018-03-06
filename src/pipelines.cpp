#include "pipelines.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.hpp"
#include "shader.h"

namespace Concise
{
	Pipelines::Pipelines()
	{	
		VkPushConstantRange pushConstantRange =
			VkFactory::PushConstantRange(
				VK_SHADER_STAGE_VERTEX_BIT,
				sizeof(ThreadPushConstantBlock),
				0);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
			VkFactory::PipelineLayoutCreateInfo(&descriptorSetLayout, 1);

		VK_CHECK_RESULT(vkCreatePipelineLayout(Device::Instance().GetLogicalDevice(), 
			&pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));

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
		shaderStages.push_back(
			Shader(VertexShader, "shaders/multithreading/phong.vert.spv")
			.PipelineShaderStageCreateInfo());
		shaderStages.push_back(
			Shader(FragmentShader, "shaders/multithreading/phong.frag.spv")
			.PipelineShaderStageCreateInfo());

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
			vks::initializers::pipelineCreateInfo(
				m_pipelineLayout,
				renderPass,
				0);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
			VkFactory::GraphicsPipelineCreateInfo(shaderStages, )
	
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

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipelines.phong));
	}
	
	Pipelines::~Pipelines()
	{
		
	}
}