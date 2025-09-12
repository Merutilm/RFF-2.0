//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include "../configurator/PipelineConfigurator.hpp"
#include "../core/vkh.hpp"
#include "../executor/ScopedCommandBufferExecutor.hpp"
#include "../util/SwapchainUtils.hpp"

namespace merutilm::vkh {
    struct RendererAbstract : public Handler {
        EngineRef engine;
        WindowContextRef wc;

        uint32_t frameIndex = 0;
        bool pipelineInitialized = false;
        std::vector<PipelineConfigurator> configurators = {};

        explicit RendererAbstract(EngineRef engine, const uint32_t windowContextIndex) : engine(engine),
            wc(engine.getWindowContext(windowContextIndex)) {
        }

        ~RendererAbstract() override = default;

        RendererAbstract(const RendererAbstract &windowContext) = delete;

        RendererAbstract(RendererAbstract &&) = delete;

        RendererAbstract &operator=(RendererAbstract &&) = delete;

        RendererAbstract &operator=(const RendererAbstract &) = delete;


        [[nodiscard]] uint32_t getFrameIndex() const {
            return frameIndex;
        }

        void finishPipelineInitialization() const {
            for (const auto &sp: configurators) {
                sp->pipelineInitialized();
            }
        };

        void execute() {
            SwapchainUtils::renderFrame(wc, &frameIndex, [this](const uint32_t swapchainImageIndex) {
                if (frameIndex == 0) {
                    for (auto &rc: wc.getRenderContexts()) {
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

    private:
        virtual void beforeCmdRender() = 0;

        virtual void cmdRender(uint32_t swapchainImageIndex) = 0;
    };

    using Renderer = std::unique_ptr<RendererAbstract>;
    using RendererPtr = RendererAbstract *;
    using RendererRef = RendererAbstract &;
}
