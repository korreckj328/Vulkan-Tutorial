#include "RenderTarget.h"
#include "VulkanContext.h"
#include "Tools.h"
#include <stdexcept>

RenderTarget::RenderTarget() {

}

RenderTarget::~RenderTarget() {

}

void RenderTarget::createViewsAndFramebuffer(std::vector<VkImage> swapChainImages, 
        VkFormat swapChainImageFormat, VkExtent2D swapChainImageExtent, VkRenderPass renderPass) {
    _swapChainImages = swapChainImages;
    _swapChainImageExtent = swapChainImageExtent;
    createImageViews(swapChainImageFormat);
    createFrameBuffer(swapChainImageExtent, renderPass);
}

void RenderTarget::createImageViews(VkFormat swapChainImageFormat) {
    swapChainImageViews.resize(_swapChainImages.size());
    for(size_t i = 0; i < _swapChainImages.size(); i++) {
        swapChainImageViews[i] = vkTools::createImageView(_swapChainImages[i], swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void RenderTarget::createFrameBuffer(VkExtent2D swapChainImageExtent, VkRenderPass renderPass) {
    swapChainFrameBuffers.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = { swapChainImageViews[i] };
        VkFramebufferCreateInfo fbInfo = {};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = swapChainImageExtent.width;
        fbInfo.height = swapChainImageExtent.height;
        fbInfo.layers = 1;

        if(vkCreateFramebuffer(VulkanContext::getInstance()->getDevice()->logicalDevice, &fbInfo,
                    NULL, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffers.");
        }
    }
}

void RenderTarget::destroy() {
    for(auto frameBuffer:swapChainFrameBuffers) {
        vkDestroyFramebuffer(VulkanContext::getInstance()->getDevice()->logicalDevice, frameBuffer, nullptr);
    }
}
