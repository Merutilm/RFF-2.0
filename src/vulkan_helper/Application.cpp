//
// Created by Merutilm on 2026-02-06.
//


#include <utility>
#include <vulkan_helper/Application.hpp>

#ifdef VKH_USE_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#endif
#include "vulkan_helper/engine/internal/RCCImGui.hpp"


namespace merutilm::vkh {

    Application::Application(WindowInitializerSettings rootWindowInitializerSettings) :
        rootWindowInitializerSettings(std::move(rootWindowInitializerSettings)) {
        Application::init();
    }

    Application::~Application() { Application::cleanup(); }


    void Application::init() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        if (!glfwVulkanSupported()) {
            throw exception_init("Vulkan is not supported!");
        }
        engine.emplace();
        configureRootWindowContext();
    }

    void Application::configureRootWindowContext() {
        rootWindowContext = &engine->attachWindowContext(std::move(rootWindowInitializerSettings), 0);
    }

    void Application::addListeners() {

        rootWindowContext->getWindow()->eventSystem.applicationLifecycle.onUpdate.add([this] { update(); });

        rootWindowContext->getWindow()->eventSystem.resize.onResize.add([this](const int w, const int h) {
            if (w > 0 && h > 0) {
                const VkExtent2D extent = {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
                rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
                rootWindowContext->getSwapchain().recreate(extent);
                refreshSharedImgContexts(rootWindowContext->getSwapchain().getSwapchainExtent());
                for (const auto &rc: rootWindowContext->getRenderContexts())
                    rc->recreate();

                callRenderContextRefreshed();
            }
        });

        rootWindowContext->getWindow()->eventSystem.applicationLifecycle.onStart.add([this] {
            onStart();
        });

        rootWindowContext->getWindow()->eventSystem.applicationLifecycle.onQuit.add([this] {
            rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
            onQuit();
        });
    }


#ifdef VKH_USE_IMGUI
    void Application::createImGuiContext(RenderContext *rc) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForVulkan(rootWindowContext->getWindow()->getWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {
                .ApiVersion = VK_API_VERSION_1_0,
                .Instance = engine->getCore().getInstance().getInstanceHandle(),
                .PhysicalDevice = engine->getCore().getPhysicalDeviceLoader().getPhysicalDeviceHandle(),
                .Device = engine->getCore().getLogicalDevice().getLogicalDeviceHandle(),
                .QueueFamily =
                        *engine->getCore().getPhysicalDeviceLoader().getQueueFamilyIndices().graphicsAndComputeFamily,
                .Queue = engine->getCore().getLogicalDevice().getGraphicsQueue(),
                .DescriptorPool = VK_NULL_HANDLE,
                .DescriptorPoolSize = 1024,
                .MinImageCount = engine->getCore().getPhysicalDeviceLoader().getMaxFramesInFlight(),
                .ImageCount = engine->getCore().getPhysicalDeviceLoader().getMaxFramesInFlight(),
                .PipelineCache = VK_NULL_HANDLE,
                .PipelineInfoMain = {.RenderPass = rc->getRenderPass()->getRenderPassHandle(),
                                     .Subpass = 0,
                                     .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
                                     .ExtraDynamicStates = {},
                                     .PipelineRenderingCreateInfo = {}},
                .UseDynamicRendering = {},
                .Allocator = {},
                .CheckVkResultFn = imguiVkResultConsumer,
                .MinAllocationSize = 0,
                .CustomShaderVertCreateInfo = {},
                .CustomShaderFragCreateInfo = {}};
        ImGui_ImplVulkan_Init(&init_info);
    }
    void Application::imguiVkResultConsumer(const VkResult result) {
        if (result != VK_SUCCESS) {
            throw exception_invalid_state("Failed to render imgui context!");
        }
    }

#endif

    void Application::cleanup() {

#ifdef VKH_USE_IMGUI
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#endif
        renderers.clear();
        engine->detachWindowContext(WC_ROOT);
        rootWindowContext = nullptr;
        engine.reset();
        glfwTerminate();
    }

} // namespace merutilm::vkh
