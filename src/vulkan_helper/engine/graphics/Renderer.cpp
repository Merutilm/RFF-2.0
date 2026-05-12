//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/graphics/Renderer.hpp>
#include <vulkan_helper/util/SwapchainUtils.hpp>
#include <vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp>

namespace merutilm::vkh {
    Renderer::Renderer(Engine & engine, WindowContext &wc, VertexBuffer &vertexBufferPP, IndexBuffer &indexBufferPP): EngineHandler(engine), wc(wc), vertexBufferPP(vertexBufferPP), indexBufferPP(indexBufferPP){
    }

    Renderer::~Renderer() = default;

    uint32_t Renderer::getFrameIndex() const {
        return frameIndex;
    }

    void Renderer::finishPipelineInitialization() const {
        for (const auto &sp: configurators) {
            sp->pipelineInitialized();
        }
    }

    void Renderer::execute() {
        SwapchainUtils::tryRenderFrame(wc, &frameIndex, [this](const uint32_t swapchainImageIndex) {
            if (frameIndex == 0) {
                for (auto &rc: wc.getRenderContexts()) {
                    //first frame ended
                    rc->getConfigurator()->allFrameInitialized();
                }
            }
            DescriptorUpdateQueue queue = DescriptorUpdater::createQueue();
            const VkDevice device = wc.core.getLogicalDevice().getLogicalDeviceHandle();

            for (const auto &configurator: configurators) {
                configurator->updateQueue(queue, frameIndex);
            }

            DescriptorUpdater::write(device, queue);

            const VkFence fence = wc.getSyncObject().getFence(frameIndex).getFenceHandle();
            const VkSemaphore imageAvailableSemaphore = wc.getSyncObject().getSemaphore(frameIndex).
                    getImageAvailable();
            const VkSemaphore renderFinishedSemaphore = wc.getSyncObject().getSemaphore(frameIndex).
                    getRenderFinished();
            beforeCmdRender();
            ScopedCommandBufferExecutor executor(wc, frameIndex, fence, imageAvailableSemaphore,
                                                 renderFinishedSemaphore);
            cmdRender(swapchainImageIndex);
        });
    }
}
