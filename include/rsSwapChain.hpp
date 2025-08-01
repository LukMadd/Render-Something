#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <vector>

namespace RS{
    class SwapChainSupportDetails{
            public:
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
            
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};

    class rsSwapChain{
        public:
            VkExtent2D swapChainExtent;

            rsSwapChain(GLFWwindow* window);

            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

            VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresent);

            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabillities);

            void createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            void createImageViews(VkDevice device);    

            VkSwapchainKHR getSwapChain();

            void destroySwapChain(VkDevice device);

            void destroyImageViews(VkDevice device);

        private:
            GLFWwindow* m_window;
            VkSwapchainKHR m_swapChain;
            std::vector<VkImage> m_swapChainImages;
            VkFormat m_swapChainImageFormat;

            std::vector<VkImageView> m_swapChainImageViews;
    };
}