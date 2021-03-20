#include "Descriptor.h"
#include <array>
#include <stdexcept>
#include "VulkanContext.h"
#include "Mesh.h"

Descriptor::Descriptor() {

}

Descriptor::~Descriptor() {

}

void Descriptor::createDescriptorLayoutSetPoolAndAllocate(uint32_t _swapChainImageCount) {
    createDescriptorSetLayout();
    createDescriptorPoolAndAllocateSets(_swapChainImageCount);
}

void Descriptor::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0; //binding location
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> layoutBindings = {uboLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};

    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());

    layoutCreateInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::getInstance()->getDevice()->logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout.");
    }
}

void Descriptor::createDescriptorPoolAndAllocateSets(uint32_t _swapChainImageCount) {
    std::array<VkDescriptorPoolSize, 1> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = _swapChainImageCount;

    VkDescriptorPoolCreateInfo poolInfo = {};

    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = _swapChainImageCount;

    if(vkCreateDescriptorPool(VulkanContext::getInstance()->getDevice()->logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool.");
    }

    std::vector<VkDescriptorSetLayout> layouts(_swapChainImageCount, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo = {};

    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = _swapChainImageCount;
    allocInfo.pSetLayouts = layouts.data();

    if(vkAllocateDescriptorSets(VulkanContext::getInstance()->getDevice()->logicalDevice, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets");
    }
}

void Descriptor::populateDescriptorSets(uint32_t _swapChainImageCount, VkBuffer uniformBuffers) {
    for(size_t i = 0; i < _swapChainImageCount; i++) {
        VkDescriptorBufferInfo uboBufferDescInfo = {};

        uboBufferDescInfo.buffer = uniformBuffers;
        uboBufferDescInfo.offset = 0;
        uboBufferDescInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet uboDescWrites;

        uboDescWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uboDescWrites.pNext = NULL;
        uboDescWrites.dstSet = descriptorSet;
        uboDescWrites.dstBinding = 0; //binding index
        uboDescWrites.dstArrayElement = 0;
        uboDescWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboDescWrites.descriptorCount = 1;
        uboDescWrites.pBufferInfo = &uboBufferDescInfo;
        uboDescWrites.pImageInfo = nullptr;
        uboDescWrites.pTexelBufferView = nullptr;
        std::array<VkWriteDescriptorSet, 1> descWrites = {uboDescWrites};

        vkUpdateDescriptorSets(VulkanContext::getInstance()->getDevice()->logicalDevice, static_cast<uint32_t>(descWrites.size()), descWrites.data(), 0, nullptr);
    }
}

void Descriptor::destroy() {
	vkDestroyDescriptorPool(VulkanContext::getInstance()->getDevice()->logicalDevice,
							descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(VulkanContext::getInstance()->getDevice()->logicalDevice,
								descriptorSetLayout, nullptr);	
}
