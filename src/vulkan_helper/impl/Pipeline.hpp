//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include "../manage/PipelineManager.hpp"
#include "../handle/WindowContextHandler.hpp"

namespace merutilm::vkh {
    struct PipelineAbstract : public WindowContextHandler {
        VkPipeline pipeline = nullptr;
        PipelineLayoutRef pipelineLayout;
        const std::vector<DescriptorPtr> descriptors;
        const std::vector<ShaderModulePtr> shaderModules;

        explicit PipelineAbstract(WindowContextRef wc, PipelineLayoutRef pipelineLayout,
                                  PipelineManager &&pipelineManager) : WindowContextHandler(wc),
                                                                       pipelineLayout(pipelineLayout),
                                                                       descriptors(
                                                                           std::move(pipelineManager->descriptors)),
                                                                       shaderModules(
                                                                           std::move(
                                                                               pipelineManager->shaderModules)) {
        }

        ~PipelineAbstract() override = default;

        PipelineAbstract(const PipelineAbstract &) = delete;

        PipelineAbstract &operator=(const PipelineAbstract &) = delete;

        PipelineAbstract(PipelineAbstract &&) = delete;

        PipelineAbstract &operator=(PipelineAbstract &&) = delete;

        virtual void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices = {}) const = 0;


        [[nodiscard]] VkPipeline getPipelineHandle() const { return pipeline; }

        [[nodiscard]] DescriptorRef getDescriptor(const uint32_t setIndex) const {
            return *descriptors[setIndex];
        }

        [[nodiscard]] PipelineLayoutRef getLayout() const { return pipelineLayout; }

        [[nodiscard]] std::span<const DescriptorPtr> getDescriptors() const { return descriptors; }

        [[nodiscard]] std::span<const ShaderModulePtr> getShaderModules() const {
            return shaderModules;
        }


        [[nodiscard]] std::vector<VkDescriptorSet> enumerateDescriptorSets(const uint32_t frameIndex, DescIndexPicker &&descIndices = {}) const {
            std::vector<VkDescriptorSet> sets(descriptors.size());

            if (!descIndices.empty()) {
                safe_array::check_size_equal(descriptors.size(), descIndices.size(), "Descriptor Index");
            }else {
                descIndices = std::vector<uint32_t>(descriptors.size(), 0);
            }

            for (uint32_t i = 0; i < descriptors.size(); i++) {
                sets[i] = descriptors[i]->getDescriptorSetHandle(frameIndex, descIndices[i]);
            }
            return sets;
        }

    protected:
        void destroy() override {
            allocator::invoke(vkDestroyPipeline, wc.core.getLogicalDevice().getLogicalDeviceHandle(), pipeline, nullptr);
        }
    };

    using Pipeline = std::unique_ptr<PipelineAbstract>;
    using PipelinePtr = PipelineAbstract *;
    using PipelineRef = PipelineAbstract &;
}
