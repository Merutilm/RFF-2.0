//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include <algorithm>

#include "../impl/Descriptor.hpp"
#include "../impl/PipelineLayout.hpp"
#include "../impl/ShaderModule.hpp"

namespace merutilm::vkh {
    class PipelineManagerImpl {
        PipelineLayoutRef layout;
        std::vector<DescriptorPtr> descriptors = {};
        std::vector<ShaderModulePtr> shaderModules;

    public:
        explicit PipelineManagerImpl(PipelineLayoutRef layout) : layout(layout) {
        }


        void attachShader(ShaderModulePtr shaderStage) {
            shaderModules.emplace_back(shaderStage);
        }

        void attachDescriptor(std::vector<DescriptorPtr> &&descriptor) { descriptors = std::move(descriptor); }

        [[nodiscard]] DescriptorRef getDescriptor(const uint32_t setIndex) const {
            return *descriptors[setIndex];
        }

        [[nodiscard]] std::vector<DescriptorPtr> &getDescriptors() { return descriptors; }


        std::vector<VkDescriptorSet> getDescriptorSets(uint32_t frameIndex) {
            std::vector<VkDescriptorSet> sets(descriptors.size());
            std::ranges::transform(descriptors, sets.begin(), [frameIndex](const DescriptorPtr desc) {
                return desc->getDescriptorSetHandle(frameIndex);
            });
            return sets;
        }

        [[nodiscard]] PipelineLayoutRef getLayout() const { return layout; }

        [[nodiscard]] std::span<const ShaderModulePtr> getShaderModules() const {
            return shaderModules;
        }
    };

    using PipelineManager = std::unique_ptr<PipelineManagerImpl>;
    using PipelineManagerPtr = PipelineManagerImpl *;
    using PipelineManagerRef = PipelineManagerImpl &;
}
