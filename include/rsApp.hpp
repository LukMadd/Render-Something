#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "rsWindow.hpp"
#include "rsInstance.hpp"
#include "rsPipeline.hpp"
#include "rsCommand.hpp"

namespace RS{
    class rsApp{
        public:
            void run();

        private:
            void initVulkan();
            void mainLoop();
            void cleanup();

            rsWindow appWindow;
            GLFWwindow* window = appWindow.getWindow();
            
            VkSurfaceKHR surface;

            rsInstance appInstance{instance};
            VkInstance instance;

            rsSwapChain appSwapChain{window};
            VkSwapchainKHR swapChain;

            rsPipeline appPipeline;

            rsCommand appCommand;
    };
}