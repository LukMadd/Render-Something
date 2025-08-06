#include "rsCommand.hpp"
#include "rsQueue.hpp"
#include <cstdint>

namespace RS{
    void rsCommand::createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface){
        rsQueue queue;

        QueueFamilyIndices queueFamilyIndices = queue.FindQueueFamily(physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
        if(result != VK_SUCCESS){
            throw std::runtime_error("Failed to create command pool error code: " + std::to_string(result) + "!");
        }
    }

    void rsCommand::cleanupCommandPool(VkDevice device){
        vkDestroyCommandPool(device, commandPool, nullptr);
    }

    void rsCommand::createCommandBuffer(VkDevice device){
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
        if(result != VK_SUCCESS){
            throw std::runtime_error("Failed to allocate command buffers error code " + std::to_string(result) + "!");
        }
    }

    void rsCommand::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkRenderPass renderPass, rsSwapChain swapChain, VkPipeline &graphicsPipeline){
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VkResult beginResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if(beginResult != VK_SUCCESS){
            throw std::runtime_error("Failed to begin command buffer error code " + std::to_string(beginResult) + "!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChain.swapChainFramebuffers[imageIndex];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.swapChainExtent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.swapChainExtent.width);
        viewport.height = static_cast<float>(swapChain.swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain.swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        VkResult endResult = vkEndCommandBuffer(commandBuffer);
        if(endResult != VK_SUCCESS){
            throw std::runtime_error("Failed to record command buffers error code " + std::to_string(endResult) + "!");
        }

    }
}