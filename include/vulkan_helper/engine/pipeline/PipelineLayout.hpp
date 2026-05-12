//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include <vulkan_helper/handle/CoreHandler.hpp>
#include <vulkan_helper/engine/manage/PipelineLayoutManager.hpp>

namespace merutilm::vkh {
    class PipelineLayout final : public CoreHandler {

        VkPipelineLayout layout = nullptr;
        PipelineLayoutBuilder builders;
        uint32_t descriptorSetLayoutCount;

    public:
        explicit PipelineLayout(Core & core, PipelineLayoutManager &&pipelineLayoutManager);

        ~PipelineLayout() override;

        PipelineLayout(const PipelineLayout &) = delete;

        PipelineLayout &operator=(const PipelineLayout &) = delete;

        PipelineLayout(PipelineLayout &&) = delete;

        PipelineLayout &operator=(PipelineLayout &&) = delete;

        void cmdPush(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] VkPipelineLayout getLayoutHandle() const { return layout; }

        [[nodiscard]] std::vector<DescriptorSetLayout *> getDescriptorSetLayouts() const;

        [[nodiscard]] std::vector<PushConstant *> getPushConstants() const;

        [[nodiscard]] PushConstant * getPushConstant(uint32_t pushIndex) const;

    private:
        void init() override;

        void cleanup() override;
    };

    

}
