#include <vulkan/vulkan.hpp>

namespace RS{
    class rsPipeline{
        public:
            void createGraphicsPipeline(VkDevice device, VkExtent2D swapChainExtent);

            void cleanupPipeline(VkDevice device);

        private:
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;

            VkPipelineLayout m_pipelineLayout;
    };
}