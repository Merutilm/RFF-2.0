//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include "../handle/CoreHandler.hpp"
#include "../manage/PipelineLayoutManager.hpp"

namespace merutilm::vkh {
    class PipelineLayoutImpl final : public CoreHandler {

        VkPipelineLayout layout = nullptr;
        PipelineLayoutBuilder builders;
        uint32_t descriptorSetLayoutCount;

    public:
        explicit PipelineLayoutImpl(CoreRef core, PipelineLayoutManager &&pipelineLayoutManager);

        ~PipelineLayoutImpl() override;

        PipelineLayoutImpl(const PipelineLayoutImpl &) = delete;

        PipelineLayoutImpl &operator=(const PipelineLayoutImpl &) = delete;

        PipelineLayoutImpl(PipelineLayoutImpl &&) = delete;

        PipelineLayoutImpl &operator=(PipelineLayoutImpl &&) = delete;

        void cmdPush(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] VkPipelineLayout getLayoutHandle() const { return layout; }

        [[nodiscard]] std::vector<DescriptorSetLayoutPtr> getDescriptorSetLayouts() const {
            std::vector<DescriptorSetLayoutPtr> v(descriptorSetLayoutCount, nullptr);
            std::transform(builders.begin(), builders.begin() + descriptorSetLayoutCount, v.begin(), [] (const PipelineLayoutBuildType &type){
                return std::get<DescriptorSetLayoutPtr>(type);
            });
            return v;
        }

        [[nodiscard]] std::vector<PushConstantPtr> getPushConstants() const {
            std::vector<PushConstantPtr> v(builders.size() - descriptorSetLayoutCount, nullptr);
            std::transform(builders.begin() + descriptorSetLayoutCount, builders.end(), v.begin(), [] (const PipelineLayoutBuildType &type){
                return std::get<PushConstantPtr>(type);
            });
            return v;
        }

        [[nodiscard]] PushConstantPtr getPushConstant(const uint32_t pushIndex) const {
            return std::get<PushConstantPtr>(builders[pushIndex - descriptorSetLayoutCount]);
        }

    private:
        void init() override;

        void destroy() override;
    };

    
    using PipelineLayout = std::unique_ptr<PipelineLayoutImpl>;
    using PipelineLayoutPtr = PipelineLayoutImpl *;
    using PipelineLayoutRef = PipelineLayoutImpl &;
}
