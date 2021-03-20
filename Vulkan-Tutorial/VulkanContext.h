#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "ApplicationLayersAndValidation.h"
#include "VulkanInstance.h"
#include "Device.h"
#include "SwapChain.h"
#include "Renderpass.h"
#include "RenderTarget.h"
#include "DrawCommandBuffer.h"

// take this out later I need to figure out a way to do this
// with Cmake rather than directly in the code.

#define _DEBUG

#ifdef _DEBUG
const bool isValidationLayersEnabled = true;
#else
const bool isValidationLayersEnabled = false;
#endif

class VulkanContext
{
public:
	static VulkanContext *getInstance();
	Device* getDevice();
	static VulkanContext *instance;
	~VulkanContext();
	void initVulkan(GLFWwindow *window);
    void drawBegin();
    void drawEnd();
    void cleanup();
	SwapChain *getSwapChain();
	Renderpass *getRenderpass();
	VkCommandBuffer getCurrentCommandBuffer();

private:
    uint32_t imageIndex = 0;
    VkCommandBuffer currentCommandBuffer;
	VkSurfaceKHR surface;
	ApplicationLayersAndValidation* valLayersAndExt;
	VulkanInstance *vInstance;
	Device *device;
	SwapChain* swapChain;
    Renderpass *renderPass;
    RenderTarget *renderTarget;
    DrawCommandBuffer *drawCommandBuffer;
	const int MAX_FRAMES_IN_FLIGHT = 2;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
};

