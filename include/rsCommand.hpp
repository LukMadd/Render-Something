#include <vulkan/vulkan.hpp>
#include "rsSwapChain.hpp"

namespace RS{
    class rsCommand{
        public:
            VkCommandPool commandPool;

            void createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            void cleanupCommandPool(VkDevice device);

            VkCommandBuffer commandBuffer;

            void createCommandBuffer(VkDevice device);

            void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkRenderPass renderPass, rsSwapChain swapChain, VkPipeline &graphicsPipeline);
    };
}