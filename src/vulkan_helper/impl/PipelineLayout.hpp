//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include "../handle/CoreHandler.hpp"
#include "../manage/PipelineLayoutManager.hpp"

namespace merutilm::vkh {
    class PipelineLayoutImpl final : public CoreHandler {

        PipelineLayoutManager pipelineLayoutManager = nullptr;
        VkPipelineLayout layout = nullptr;

    public:
        explicit PipelineLayoutImpl(const CoreRef core, PipelineLayoutManager &&pipelineLayoutManager);

        ~PipelineLayoutImpl() override;

        PipelineLayoutImpl(const PipelineLayoutImpl &) = delete;

        PipelineLayoutImpl &operator=(const PipelineLayoutImpl &) = delete;

        PipelineLayoutImpl(PipelineLayoutImpl &&) = delete;

        PipelineLayoutImpl &operator=(PipelineLayoutImpl &&) = delete;

        void push(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] VkPipelineLayout getLayoutHandle() const { return layout; }

        [[nodiscard]] PipelineLayoutManagerRef getPipelineLayoutManager() const { return *pipelineLayoutManager; }

    private:
        void init() override;

        void destroy() override;
    };

    
    using PipelineLayout = std::unique_ptr<PipelineLayoutImpl>;
    using PipelineLayoutPtr = PipelineLayoutImpl *;
    using PipelineLayoutRef = PipelineLayoutImpl &;
}
