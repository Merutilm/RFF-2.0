//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "PipelineConfigurator.hpp"

namespace merutilm::vkh {
    class ComputePipelineConfigurator : public PipelineConfiguratorAbstract {
        ShaderModuleRef computeShader;
        VkExtent2D extent = {};
        static constexpr uint32_t WORK_GROUP_SIZE = 16;

    public:
        explicit ComputePipelineConfigurator(EngineRef engine, const uint32_t windowContextIndex,
                                             const std::string &compName) : PipelineConfiguratorAbstract(
                                                                                engine, windowContextIndex),
                                                                            computeShader(
                                                                                pickFromGlobalRepository<
                                                                                    GlobalShaderModuleRepo,
                                                                                    ShaderModuleRef>(compName)) {
        }

        ~ComputePipelineConfigurator() override = default;

        ComputePipelineConfigurator(const ComputePipelineConfigurator &) = delete;

        ComputePipelineConfigurator &operator=(const ComputePipelineConfigurator &) = delete;

        ComputePipelineConfigurator(ComputePipelineConfigurator &&) = delete;

        ComputePipelineConfigurator &operator=(ComputePipelineConfigurator &&) = delete;

        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) override;

        void setExtent(const VkExtent2D &extent) {
            this->extent = extent;
        }

        void configure() override;

    protected:
        void cmdDispatch(const VkCommandBuffer cbh) const {
            const auto [width, height] = extent;
            vkCmdDispatch(cbh, (width + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE,
                          (height + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE, 1);
        }
    };
}
