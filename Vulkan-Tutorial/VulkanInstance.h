#pragma once

#include <vulkan/vulkan.h>
#include "ApplicationLayersAndValidation.h"

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	VkInstance vkInstance;

	void createAppAndVkInstance(bool enableValidationLayers, ApplicationLayersAndValidation* valLayersAndExtensions);
};

