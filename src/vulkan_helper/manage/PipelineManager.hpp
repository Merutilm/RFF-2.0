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
        const PipelineLayout &layout;
        std::vector<const Descriptor *> descriptors = {};
        std::vector<const ShaderModule *> shaderModules;

    public:
        explicit PipelineManagerImpl(const PipelineLayout &layout) : layout(layout) {
        }


        void attachShader(const ShaderModule *shaderStage) {
            shaderModules.emplace_back(shaderStage);
        }

        void attachDescriptor(std::vector<const Descriptor *> &&descriptor) { descriptors = std::move(descriptor); }

        [[nodiscard]] const Descriptor &getDescriptor(const uint32_t setIndex) const {
            return *descriptors[setIndex];
        }

        [[nodiscard]] std::vector<const Descriptor *> &getDescriptors() { return descriptors; }


        std::vector<VkDescriptorSet> getDescriptorSets(uint32_t frameIndex) {
            std::vector<VkDescriptorSet> sets(descriptors.size());
            std::ranges::transform(descriptors, sets.begin(), [frameIndex](const Descriptor *desc) {
                return desc->getDescriptorSetHandle(frameIndex);
            });
            return sets;
        }

        [[nodiscard]] const PipelineLayout &getLayout() const { return layout; }

        [[nodiscard]] std::span<const ShaderModule * const> getShaderModules() const {
            return shaderModules;
        }
    };

    using PipelineManager = std::unique_ptr<PipelineManagerImpl>;
    using PipelineManagerPtr = PipelineManagerImpl *;
    using PipelineManagerRef = PipelineManagerImpl &;
}
