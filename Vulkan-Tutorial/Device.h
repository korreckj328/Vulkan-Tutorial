#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include "VulkanInstance.h"
#include "ApplicationLayersAndValidation.h"

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;
	bool arePresent() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

class Device {
public:
	Device();
	~Device();
    void destroy();
	VkPhysicalDevice physicalDevice;
	SwapChainSupportDetails swapChainSupport;
	QueueFamilyIndices queueFamilyIndices;
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	void pickPhysicalDevice(VulkanInstance* vInstance, VkSurfaceKHR surface);
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkDeviceExtensionsSupported(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	QueueFamilyIndices getQueueFamiliesIndicesOfCurrentDevice();

	// logical device

	void createLogicalDevice(VkSurfaceKHR surface, bool isValidationLayersEnabled, ApplicationLayersAndValidation* appValLayersAndExtensions);
	VkDevice logicalDevice;

    VkQueue graphicsQueue;
	VkQueue presentQueue;
};

