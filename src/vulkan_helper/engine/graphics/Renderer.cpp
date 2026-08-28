//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/graphics/Renderer.hpp>
#include <vulkan_helper/util/SwapchainUtils.hpp>
#include <vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp>

namespace merutilm::vkh {
    Renderer::Renderer(Engine & engine, WindowContext &wc): EngineHandler(engine), wc(wc){
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

    void Renderer::render() {
        SwapchainUtils::tryRenderFrame(wc, &frameIndex, [this](const uint32_t swapchainImageIndex) {
            DescriptorUpdateQueue queue = DescriptorUpdater::createQueue();
            const VkDevice device = wc.core.getLogicalDevice().getLogicalDeviceHandle();

            for (const auto &configurator: configurators) {
                configurator->updateQueue(queue, frameIndex);
            }

            DescriptorUpdater::write(device, queue);

            const CommandBuffer &commandBuffer = wc.getCommandBufferGroup().getCommandBuffer(frameIndex);
            const Fence &fence = wc.getSyncObject().getFence(frameIndex);
            const Semaphore &imageAvailableSemaphore = wc.getSyncObject().getWaitSemaphore(frameIndex);
            const Semaphore &renderFinishedSemaphore = wc.getSyncObject().getSignalSemaphore(frameIndex);

            beforeCmdRender();
            ScopedCommandBufferExecutor executor(wc, commandBuffer, fence, &imageAvailableSemaphore, &renderFinishedSemaphore);
            cmdRender(swapchainImageIndex);
        });
    }
}
