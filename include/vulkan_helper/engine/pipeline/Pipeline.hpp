//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include <vulkan_helper/engine/manage/PipelineManager.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>

namespace merutilm::vkh {
    struct Pipeline : WindowContextHandler {
        VkPipeline pipeline = nullptr;
        PipelineLayout &pipelineLayout;
        const std::vector<Descriptor *> descriptors;
        const std::vector<ShaderModule *> shaderModules;

        explicit Pipeline(WindowContext &wc, PipelineLayout &pipelineLayout,
                                  PipelineManager &&pipelineManager);

        ~Pipeline() override;

        Pipeline(const Pipeline &) = delete;

        Pipeline &operator=(const Pipeline &) = delete;

        Pipeline(Pipeline &&) = delete;

        Pipeline &operator=(Pipeline &&) = delete;

        virtual void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) const = 0;


        [[nodiscard]] VkPipeline getPipelineHandle() const { return pipeline; }

        [[nodiscard]] Descriptor &getDescriptor(const uint32_t setIndex) const {
            return *descriptors[setIndex];
        }

        [[nodiscard]] PipelineLayout &getLayout() const { return pipelineLayout; }

        [[nodiscard]] const std::vector<Descriptor *> &getDescriptors() const { return descriptors; }

        [[nodiscard]] const std::vector<ShaderModule *> &getShaderModules() const { return shaderModules; }


        [[nodiscard]] std::vector<VkDescriptorSet> enumerateDescriptorSets(uint32_t frameIndex, DescIndexPicker &&descIndices = {}) const;

    protected:
        void cleanup() override;
    };
}
