//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include "../manage/PipelineManager.hpp"

#include "../handle/EngineHandler.hpp"

namespace merutilm::vkh {
    struct PipelineAbstract : public EngineHandler {
        VkPipeline pipeline = nullptr;
        PipelineLayoutRef pipelineLayout;
        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        const PipelineManager pipelineManager = nullptr;

        explicit PipelineAbstract(EngineRef engine, PipelineLayoutRef pipelineLayout,
                                  const uint32_t renderContextIndex,
                                  const uint32_t primarySubpassIndex,
                                  PipelineManager &&pipelineManager) : EngineHandler(engine),
                                                                       pipelineLayout(pipelineLayout),
                                                                       renderContextIndex(renderContextIndex),
                                                                       primarySubpassIndex(primarySubpassIndex),
                                                                       pipelineManager(std::move(pipelineManager)) {
        };

        ~PipelineAbstract() override = default;

        PipelineAbstract(const PipelineAbstract &) = delete;

        PipelineAbstract &operator=(const PipelineAbstract &) = delete;

        PipelineAbstract(PipelineAbstract &&) = delete;

        PipelineAbstract &operator=(PipelineAbstract &&) = delete;

        virtual void bind(VkCommandBuffer cbh, uint32_t frameIndex) const = 0;

        [[nodiscard]] PipelineManagerRef getPipelineManager() const { return *pipelineManager; }

        [[nodiscard]] VkPipeline getPipelineHandle() const { return pipeline; }

    protected:
        void destroy() override {
            vkDestroyPipeline(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), pipeline, nullptr);
        }
    };

    using Pipeline = std::unique_ptr<PipelineAbstract>;
    using PipelinePtr = PipelineAbstract *;
    using PipelineRef = PipelineAbstract &;
}
