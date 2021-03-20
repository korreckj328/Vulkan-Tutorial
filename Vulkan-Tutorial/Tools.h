#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace vkTools {
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    
    void createBuffer(VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkBuffer &buffer,
                        VkDeviceMemory &bufferMemory);
    
    uint32_t findMemoryTypeIndex(uint32_t typeFilter, 
                        VkMemoryPropertyFlags properties);

    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);

    void endSingleTimeCommands(VkCommandBuffer commandBuffer,
                VkCommandPool commandPool);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size);
}
