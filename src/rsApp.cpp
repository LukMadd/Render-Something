#include "rsApp.hpp"

namespace RS{
    void rsApp::run(){
        initVulkan();
        mainLoop();
        cleanup();
    }

    void rsApp::initVulkan(){
        appInstance.createInstance(instance);
        appWindow.createSurface(instance, window,surface);
        appInstance.pickPhysicalDevice(instance, surface);
        appInstance.createLogicalDevice(surface);
        appSwapChain.createSwapChain(appInstance.device, appInstance.physicalDevice, surface);
        swapChain = appSwapChain.getSwapChain();
        appSwapChain.createImageViews(appInstance.device);
        appPipeline.createGraphicsPipeline(appInstance.device, appSwapChain.swapChainExtent);
    }

    void rsApp::mainLoop(){
        while(!glfwWindowShouldClose(window)){
            glfwPollEvents();
        }
    }

    void rsApp::cleanup(){
        glfwDestroyWindow(window);

        glfwTerminate();

        appSwapChain.destroyImageViews(appInstance.device);

        appSwapChain.destroySwapChain(appInstance.device);

        appPipeline.cleanupPipeline(appInstance.device);

        vkDestroyDevice(appInstance.device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        vkDestroyInstance(instance, nullptr);
    }
}