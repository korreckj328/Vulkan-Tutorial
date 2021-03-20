#include "Tools.h"

#include "VulkanContext.h"
#include <stdexcept>

namespace vkTools {
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if(vkCreateImageView(VulkanContext::getInstance()->getDevice()->logicalDevice, &viewInfo,
                    nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view");
        }
        return imageView;
    }

    uint32_t findMemoryTypeIndex(uint32_t typeFilter,
                    VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(VulkanContext::getInstance()->getDevice()->physicalDevice, &memProperties);
        for(uint32_t i=0; i < memProperties.memoryTypeCount; i++) {
            if((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & 
                     properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type");
    }

    void createBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo = {};

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if(vkCreateBuffer(VulkanContext::getInstance()->getDevice()->logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer");
        }

        VkMemoryRequirements memRequirements;

        vkGetBufferMemoryRequirements(VulkanContext::getInstance()->getDevice()->logicalDevice, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};

        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits, properties);
        if (vkAllocateMemory(VulkanContext::getInstance()->getDevice()->logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory");
        }

        vkBindBufferMemory(VulkanContext::getInstance()->getDevice()->logicalDevice, buffer, bufferMemory, 0);

    }

    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(VulkanContext::getInstance()->getDevice()->logicalDevice, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo = {};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void endSingleTimecommands(VkCommandBuffer commandBuffer,
                VkCommandPool commandPool) {
        VkSubmitInfo submitInfo = {};

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(VulkanContext::getInstance()->getDevice()->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VulkanContext::getInstance()->getDevice()->graphicsQueue);
        vkFreeCommandBuffers(VulkanContext::getInstance()->getDevice()->logicalDevice, commandPool, 1, &commandBuffer);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size) {
        QueueFamilyIndices qFamilyIndices = VulkanContext::getInstance()->getDevice()->getQueueFamiliesIndicesOfCurrentDevice();
        VkCommandPool commandPool;
        VkCommandPoolCreateInfo cpInfo = {};

        cpInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cpInfo.queueFamilyIndex = qFamilyIndices.graphicsFamily;
        cpInfo.flags = 0;

        if(vkCreateCommandPool(VulkanContext::getInstance()->getDevice()->logicalDevice,
                        &cpInfo,
                        nullptr,
                        &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);
        VkBufferCopy copyregion = {};
        copyregion.srcOffset = 0;
        copyregion.dstOffset = 0;
        copyregion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyregion);
        endSingleTimecommands(commandBuffer, commandPool);
        vkDestroyCommandPool(VulkanContext::getInstance()->getDevice()->logicalDevice, commandPool, nullptr);
    }
}
