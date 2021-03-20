#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include <vector>

class RenderTarget {
public:
    RenderTarget();
    ~RenderTarget();
    std::vector<VkImage> _swapChainImages;
    VkExtent2D _swapChainImageExtent;

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    void createViewsAndFramebuffer(std::vector<VkImage> swapChainImages, VkFormat swapChainFormat, 
            VkExtent2D swapChainImageExtent, VkRenderPass renderPass);
    void createImageViews(VkFormat swapChainImageFormat);
    void createFrameBuffer(VkExtent2D swapChainImageExtent, VkRenderPass renderPass);
    void destroy();
};

