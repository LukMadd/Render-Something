#include <cstdint>
#include <limits>
#include "rsSwapChain.hpp"
#include "rsQueue.hpp"

namespace RS{
    SwapChainSupportDetails SwapChainSupportDetails::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface){
        SwapChainSupportDetails swapChainDetails;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface, &swapChainDetails.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if(formatCount != 0){
            swapChainDetails.formats.resize(formatCount);

            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
        }

        uint32_t presentCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);

        if(presentCount != 0){
            swapChainDetails.presentModes.resize(presentCount);

            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, swapChainDetails.presentModes.data());
        } 
       
        return swapChainDetails;
    }

    rsSwapChain::rsSwapChain(GLFWwindow* window) : m_window(window){};

    VkSurfaceFormatKHR rsSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
        for(const auto& surfaceFormat : availableFormats){
            if(surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                return surfaceFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR rsSwapChain::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes){
        for(const auto& presentMode : availablePresentModes){
            if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR){
                return presentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D rsSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabillities){
        if(surfaceCapabillities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
            return surfaceCapabillities.currentExtent;
        } else{
            int width, height;

            glfwGetFramebufferSize(m_window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, surfaceCapabillities.minImageExtent.width, surfaceCapabillities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, surfaceCapabillities.minImageExtent.height, surfaceCapabillities.maxImageExtent.height);
            
            return actualExtent;
        }
    }

    void rsSwapChain::createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface){
        rsQueue queue;

        SwapChainSupportDetails swapChainSupport;
        swapChainSupport = swapChainSupport.querySwapChainSupport(physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount){
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo{};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = surface;

        swapChainCreateInfo.imageFormat = surfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainCreateInfo.minImageCount = imageCount;
        swapChainCreateInfo.imageExtent = extent;
        swapChainCreateInfo.presentMode = presentMode;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = queue.FindQueueFamily(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if(indices.graphicsFamily != indices.presentFamily){
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else{
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainCreateInfo.queueFamilyIndexCount = 0;
            swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainCreateInfo.clipped = VK_TRUE;
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &m_swapChain);
        if(result != VK_SUCCESS){
            throw std::runtime_error("Failed to create swap chain error code: " + std::to_string(result) + "!");
        }

        vkGetSwapchainImagesKHR(device,m_swapChain, &imageCount, nullptr);
        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, m_swapChainImages.data());

        m_swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    VkSwapchainKHR rsSwapChain::getSwapChain(){
        return m_swapChain;
    }

    void rsSwapChain::destroySwapChain(VkDevice device){
        vkDestroySwapchainKHR(device, m_swapChain, nullptr);
    }

    void rsSwapChain::createImageViews(VkDevice device){
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for(size_t i = 0; i < m_swapChainImages.size(); i++){
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_swapChainImages[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_swapChainImageFormat;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;

            VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]);
            if(result != VK_SUCCESS){
                throw std::runtime_error("Failed to create image view at index " + std::to_string(i) + " error code: "  = std::to_string(result) + "!");
            }
        }
    }

    void rsSwapChain::destroyImageViews(VkDevice device){
        for(auto imageView : m_swapChainImageViews){
            vkDestroyImageView(device, imageView, nullptr);
        }
    }
}