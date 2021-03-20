#include "VulkanContext.h"
#include "VulkanInstance.h"
#include <limits>
#include <stdexcept>

VulkanContext *VulkanContext::instance = NULL;

VulkanContext *VulkanContext::getInstance()
{
	if (!instance) {
		instance = new VulkanContext();
	}
	return instance;
}

Device* VulkanContext::getDevice()
{
	if (!device) {
		throw std::runtime_error("no device!");
	}

	return device;
}

VulkanContext::~VulkanContext()
{
	if (instance) {
        delete renderPass;
        renderPass = nullptr;
        delete swapChain;
        swapChain = nullptr;
		delete instance;
		instance = nullptr;
	}
}

void VulkanContext::initVulkan(GLFWwindow* window)
{
	valLayersAndExt = new ApplicationLayersAndValidation();

	if (isValidationLayersEnabled && !valLayersAndExt->checkValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers Not Available !");
	}

	vInstance = new VulkanInstance();
	vInstance->createAppAndVkInstance(isValidationLayersEnabled, valLayersAndExt);

	valLayersAndExt->setupDebugCallback(isValidationLayersEnabled, vInstance->vkInstance);

	if (glfwCreateWindowSurface(vInstance->vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window Surface");
	}

	// PickPhysicalDevice and CreateLogicalDevice();
	device = new Device();
	device->pickPhysicalDevice(vInstance, surface);
	device->createLogicalDevice(surface, isValidationLayersEnabled, valLayersAndExt);

	// Create SwapChain
	swapChain = new SwapChain();
	swapChain->create(surface);

    // Create Renderpass
    renderPass = new Renderpass();
    renderPass->createRenderPass(swapChain->swapChainFormat);

    // Create RenderTarget
    renderTarget = new RenderTarget();
    renderTarget->createViewsAndFramebuffer(swapChain->swapChainImages, swapChain->swapChainFormat, swapChain->swapChainImageExtent, renderPass->renderPass);

    // Create Command Buffer
    drawCommandBuffer = new DrawCommandBuffer();
    drawCommandBuffer->createCommandPoolAndBuffer(swapChain->swapChainImages.size());
	
	//Sync
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(device->logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	vkCreateSemaphore(device->logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if(vkCreateFence(device->logicalDevice, &fenceCreateInfo, nullptr,
					&inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create sync objects per frame.");
		}
	}
}

void VulkanContext::drawBegin() {
    vkAcquireNextImageKHR(VulkanContext::getInstance()->getDevice()->logicalDevice,
            swapChain->swapChain,
            std::numeric_limits<uint64_t>::max(),
            imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &imageIndex);

	vkWaitForFences(device->logicalDevice, 1, &inFlightFences[imageIndex], VK_TRUE,
			std::numeric_limits<uint64_t>::max());

	vkResetFences(device->logicalDevice, 1, &inFlightFences[imageIndex]);

    currentCommandBuffer = drawCommandBuffer->commandBuffers[imageIndex];

    // Begin command buffer recording

    drawCommandBuffer->beginCommandBuffer(currentCommandBuffer);

    // Begin Renderpass
    VkClearValue clearcolor = {1.0f, 0.0f, 1.0f, 1.0f};
    std::array<VkClearValue, 1>clearValues = {clearcolor};
    renderPass->beginRenderPass(clearValues, currentCommandBuffer,
            renderTarget->swapChainFrameBuffers[imageIndex],
            renderTarget->_swapChainImageExtent);
}

void VulkanContext::drawEnd() {
    renderPass->endRenderPass(currentCommandBuffer);
    drawCommandBuffer->endCommandBuffer(currentCommandBuffer);

    VkSubmitInfo submitInfo = {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;

	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

    vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, inFlightFences[imageIndex]);
    
    // Present frame
    VkPresentInfoKHR presentInfo = {};

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain->swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(device->presentQueue, &presentInfo);
    
    vkQueueWaitIdle(device->presentQueue);

}

void VulkanContext::cleanup() {
    vkDeviceWaitIdle(device->logicalDevice);

	vkDestroySemaphore(device->logicalDevice, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(device->logicalDevice, imageAvailableSemaphore, nullptr);

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(device->logicalDevice, inFlightFences[i], nullptr);
	}

    drawCommandBuffer->destroy();
    renderPass->destroy();
    swapChain->destroy();
    getDevice()->destroy();
    valLayersAndExt->destroy(vInstance->vkInstance, isValidationLayersEnabled);

    vkDestroySurfaceKHR(vInstance->vkInstance, surface, nullptr);
    vkDestroyInstance(vInstance->vkInstance, nullptr);
    delete drawCommandBuffer;
    delete renderTarget;
    delete swapChain;
    delete device;
    delete valLayersAndExt;
    delete vInstance;
    if (instance) {
        delete instance;
    }
    instance = nullptr;
}

SwapChain* VulkanContext::getSwapChain() {
	return swapChain;
}

Renderpass* VulkanContext::getRenderpass() {
	return renderPass;
}

VkCommandBuffer VulkanContext::getCurrentCommandBuffer() {
	return currentCommandBuffer;
}

