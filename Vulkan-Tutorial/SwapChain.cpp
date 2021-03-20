#include "SwapChain.h"
#include "VulkanContext.h"

SwapChain::SwapChain()
{
}

SwapChain::~SwapChain()
{
}

VkSurfaceFormatKHR SwapChain::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { 1280, 720 };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

}

void SwapChain::create(VkSurfaceKHR surface)
{
	SwapChainSupportDetails swapChainSupportDetails = VulkanContext::getInstance()->getDevice()->swapChainSupport;
	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupportDetails.surfaceFormats);
	VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainSupportDetails.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.surfaceCapabilities);
	uint32_t imageCount = swapChainSupportDetails.surfaceCapabilities.minImageCount;
	if (swapChainSupportDetails.surfaceCapabilities.maxImageCount > 0 &&
		imageCount > swapChainSupportDetails.surfaceCapabilities.maxImageCount) {
		imageCount = swapChainSupportDetails.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;  // 1 unless making stereoscopic 3d app
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = VulkanContext::getInstance()->getDevice()->getQueueFamiliesIndicesOfCurrentDevice();

	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	if (vkCreateSwapchainKHR(VulkanContext::getInstance()->getDevice()->logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain.");
	}

	vkGetSwapchainImagesKHR(VulkanContext::getInstance()->getDevice()->logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(VulkanContext::getInstance()->getDevice()->logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainFormat = surfaceFormat.format;
	swapChainImageExtent = extent;
}

void SwapChain::destroy()
{
	vkDestroySwapchainKHR(VulkanContext::getInstance()->getDevice()->logicalDevice, swapChain, nullptr);
}
