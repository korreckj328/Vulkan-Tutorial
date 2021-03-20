#include "GraphicsPipeline.h"
#include "VulkanContext.h"
#include "Mesh.h"
#include <stdexcept>

GraphicsPipeline::GraphicsPipeline() {

}

GraphicsPipeline::~GraphicsPipeline() {

}

void GraphicsPipeline::createGraphicsPipelineLayoutAndPipeline(VkExtent2D swapChainImageExtent,
									VkDescriptorSetLayout descriptorSetLayout,
									VkRenderPass renderPass) {
	createGraphicsPipelineLayout(descriptorSetLayout);
	createGraphicsPipeline(swapChainImageExtent, renderPass);
}

void GraphicsPipeline::createGraphicsPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if(vkCreatePipelineLayout(VulkanContext::getInstance()->getDevice()->logicalDevice,
								&pipelineLayoutInfo,
								nullptr,
								&pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}
}

std::vector<char> GraphicsPipeline::readfile(const std::string &filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if(!file.is_open()) {
		throw std::runtime_error("failed to open shader file");
	}
	size_t filesize = (size_t)file.tellg();
	std::vector<char> buffer(filesize);
	file.seekg(0);
	file.read(buffer.data(), filesize);
	file.close();
	return buffer;
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char> &code) {
	VkShaderModuleCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	cInfo.codeSize = code.size();
	cInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(VulkanContext::getInstance()->getDevice()->logicalDevice,
							&cInfo,
							nullptr,
							&shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module.");
	}
	return shaderModule;
}

void GraphicsPipeline::createGraphicsPipeline(VkExtent2D swapChainImageExtent,
											VkRenderPass renderPass) {
	auto vertexShaderCode = readfile("Assets/Shaders/basic.vert.spv");
	VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertexShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	auto fragmentShaderCode = readfile("Assets/Shaders/basic.frag.spv");
	VkShaderModule fragShaderModule = createShaderModule(fragmentShaderCode);
	VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageCreateInfo.module = fragShaderModule;
	fragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageCreateInfo,
		fragShaderStageCreateInfo,
	};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
													attributeDescriptions.size());

	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;	
	
	VkPipelineRasterizationStateCreateInfo rastStateCreateInfo = {};
	rastStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rastStateCreateInfo.depthClampEnable = VK_FALSE;
	rastStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rastStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rastStateCreateInfo.lineWidth = 1.0f;
	rastStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rastStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rastStateCreateInfo.depthBiasEnable = VK_FALSE;
	rastStateCreateInfo.depthBiasConstantFactor = 0.0f;
	rastStateCreateInfo.depthBiasClamp = 0.0f;
	rastStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo msStateInfo = {};
	msStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	msStateInfo.sampleShadingEnable = VK_FALSE;
	msStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//depth and stencil create info added here if needed
	
	VkPipelineColorBlendAttachmentState cbAttach = {};
	cbAttach.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	cbAttach.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo cbCreateInfo = {};
	cbCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cbCreateInfo.attachmentCount = 1;
	cbCreateInfo.pAttachments = &cbAttach;

	// no dynamic states so not creating dynmaic state info
	
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)swapChainImageExtent.width;
	viewport.height = (float)swapChainImageExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0,0};
	scissor.extent = swapChainImageExtent;

	VkPipelineViewportStateCreateInfo vpStateInfo = {};
	vpStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vpStateInfo.viewportCount = 1;
	vpStateInfo.pViewports = &viewport;
	vpStateInfo.scissorCount = 1;
	vpStateInfo.pScissors = &scissor;

	VkGraphicsPipelineCreateInfo gpInfo = {};
	gpInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	gpInfo.stageCount = 2;
	gpInfo.pStages = shaderStages;
	gpInfo.pVertexInputState = &vertexInputInfo;
	gpInfo.pInputAssemblyState = &inputAssemblyInfo;
	gpInfo.pRasterizationState = &rastStateCreateInfo;
	gpInfo.pMultisampleState = &msStateInfo;
	gpInfo.pDepthStencilState = nullptr;
	gpInfo.pColorBlendState = &cbCreateInfo;
	gpInfo.pDynamicState = nullptr;
	gpInfo.pViewportState = &vpStateInfo;
	gpInfo.layout = pipelineLayout;
	gpInfo.renderPass = renderPass;
	gpInfo.subpass = 0;

	if(vkCreateGraphicsPipelines(VulkanContext::getInstance()->getDevice()->logicalDevice,
						VK_NULL_HANDLE, 1, &gpInfo, nullptr, &graphicsPipeline) 
						!= VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}
	vkDestroyShaderModule(VulkanContext::getInstance()->getDevice()->logicalDevice,
								vertexShaderModule, nullptr);
	vkDestroyShaderModule(VulkanContext::getInstance()->getDevice()->logicalDevice,
								fragShaderModule, nullptr);
}

void GraphicsPipeline::destroy() {
	vkDestroyPipeline(VulkanContext::getInstance()->getDevice()->logicalDevice,
										graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(VulkanContext::getInstance()->getDevice()->logicalDevice,
										pipelineLayout, nullptr);
}
