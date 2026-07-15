//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "PipelineConfigurator.hpp"

namespace merutilm::vkh {
    class ComputePipelineConfigurator : public PipelineConfigurator {
        ShaderModule &computeShader;
        VkExtent2D extent = {};
        static constexpr uint32_t WORK_GROUP_SIZE = 16;

    public:
        explicit ComputePipelineConfigurator(Engine &engine, WindowContext &wc, const std::string &compName) :
            PipelineConfigurator(engine, wc),
            computeShader(pickFromGlobalRepository<GlobalShaderModuleRepo, ShaderModule &>(compName)) {}

        ~ComputePipelineConfigurator() override = default;

        ComputePipelineConfigurator(const ComputePipelineConfigurator &) = delete;

        ComputePipelineConfigurator &operator=(const ComputePipelineConfigurator &) = delete;

        ComputePipelineConfigurator(ComputePipelineConfigurator &&) = delete;

        ComputePipelineConfigurator &operator=(ComputePipelineConfigurator &&) = delete;

        template<typename F>
            requires std::is_base_of_v<ComputePipelineConfigurator, F>
        static F *createComputePipeline(std::vector<std::unique_ptr<PipelineConfigurator>> &configurators,
                                        Engine &engine, WindowContext &wc) {
            F *ptr = dynamic_cast<F *>(configurators.emplace_back(std::make_unique<F>(engine, wc)).get());
            ptr->configure(nullptr, UINT32_MAX); // unused parameters for compute pipelines
            return ptr;
        }

        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) override;

        void setExtent(const VkExtent2D &newExtent) { this->extent = newExtent; }

        void configure([[maybe_unused]] RenderPass *rp, [[maybe_unused]] uint32_t subpass) override;

    private:
        void cmdDispatch(const VkCommandBuffer cbh) const {

            const auto [width, height] = extent;
            vkCmdDispatch(cbh, (width + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE,
                          (height + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE, 1);
        }
    };
} // namespace merutilm::vkh
