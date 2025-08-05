#include <vulkan/vulkan.hpp>

namespace RS{
    class rsPipeline{
        public:
            VkPipeline graphicsPipeline;

            VkRenderPass renderPass;

            void createGraphicsPipeline(VkDevice device, VkExtent2D swapChainExtent);

            void createRenderPass(VkDevice device, VkFormat swapChainImageFormat);

            void cleanupPipeline(VkDevice device);

        private:
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;

            VkPipelineLayout m_pipelineLayout;
    };
}