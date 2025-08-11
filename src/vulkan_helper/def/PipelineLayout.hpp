//
// Created by Merutilm on 2025-07-13.
//

#pragma once
\
#include "../handle/CoreHandler.hpp"
#include "../manage/PipelineLayoutManager.hpp"

namespace merutilm::vkh {
    class PipelineLayout final : public CoreHandler {

        std::unique_ptr<PipelineLayoutManager> pipelineLayoutManager = nullptr;
        VkPipelineLayout layout = nullptr;

    public:
        explicit PipelineLayout(const Core &core, std::unique_ptr<PipelineLayoutManager> &&pipelineLayoutManager);

        ~PipelineLayout() override;

        PipelineLayout(const PipelineLayout &) = delete;

        PipelineLayout &operator=(const PipelineLayout &) = delete;

        PipelineLayout(PipelineLayout &&) = delete;

        PipelineLayout &operator=(PipelineLayout &&) = delete;

        void push(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] VkPipelineLayout getLayoutHandle() const { return layout; }

        [[nodiscard]] const PipelineLayoutManager &getPipelineLayoutManager() const { return *pipelineLayoutManager; }

    private:
        void init() override;

        void destroy() override;
    };
}
