#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <algorithm>

class SwapChain
{
public:
	SwapChain();
	~SwapChain();
	VkSwapchainKHR swapChain;
	VkFormat swapChainFormat;
	VkExtent2D swapChainImageExtent;
	std::vector<VkImage> swapChainImages;

	VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void create(VkSurfaceKHR surface);
	void destroy();
};

