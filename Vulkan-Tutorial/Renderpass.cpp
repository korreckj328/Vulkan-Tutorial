#include "Renderpass.h"
#include "VulkanContext.h"

Renderpass::Renderpass() {

}

Renderpass::~Renderpass() {

}

void Renderpass::createRenderPass(VkFormat swapChainImageFormat) {
    VkAttachmentDescription colorAttachment = {};

    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachRef = {};

    colorAttachRef.attachment = 0;
    colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachRef;
    
    std::array<VkAttachmentDescription, 1> attachments = {
        colorAttachment
    };

    VkRenderPassCreateInfo rpCreateInfo = {};

    rpCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    rpCreateInfo.pAttachments = attachments.data();
    rpCreateInfo.subpassCount = 1;
    rpCreateInfo.pSubpasses = &subpass;

    if(vkCreateRenderPass(VulkanContext::getInstance()->getDevice()->logicalDevice, &rpCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass.");
    }
}

void Renderpass::destroy() {
    vkDestroyRenderPass(VulkanContext::getInstance()->getDevice()->logicalDevice, renderPass, nullptr);
}

void Renderpass::beginRenderPass(std::array<VkClearValue, 1> clearValues,
        VkCommandBuffer commandBuffer,
        VkFramebuffer swapChainFrameBuffer,
        VkExtent2D swapChainImageExtent) {
    
    VkRenderPassBeginInfo rpBeginInfo = {};

    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = renderPass;
    rpBeginInfo.framebuffer = swapChainFrameBuffer;
    rpBeginInfo.renderArea.offset = { 0, 0 };
    rpBeginInfo.renderArea.extent = swapChainImageExtent;
    rpBeginInfo.pClearValues = clearValues.data();
    rpBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

    vkCmdBeginRenderPass(commandBuffer, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

}

void Renderpass::endRenderPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}
