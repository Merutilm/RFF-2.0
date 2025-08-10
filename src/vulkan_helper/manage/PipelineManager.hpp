//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include <algorithm>

#include "../def/Descriptor.hpp"
#include "../def/PipelineLayout.hpp"
#include "../def/ShaderModule.hpp"

namespace merutilm::mvk {
    class PipelineManager {
        const PipelineLayout &layout;
        std::vector<const Descriptor *> descriptors = {};
        std::vector<const ShaderModule *> shaderModules;

    public:
        explicit PipelineManager(const PipelineLayout &layout) : layout(layout) {
        }

        void attachShader(const ShaderModule *shaderStage);

        void attachDescriptor(std::vector<const Descriptor *> &&descriptor) { descriptors = std::move(descriptor); }

        [[nodiscard]] const Descriptor &getDescriptor(const uint32_t setIndex) const {
            return *descriptors[setIndex];
        }

        [[nodiscard]] std::vector<const Descriptor *> &getDescriptors() { return descriptors; }

        [[nodiscard]] std::vector<VkDescriptorSet> getDescriptorSets(uint32_t frameIndex);

        [[nodiscard]] const PipelineLayout &getLayout() const { return layout; }

        [[nodiscard]] std::span<const ShaderModule * const> getShaderModules() const {
            return shaderModules;
        }
    };

    inline void PipelineManager::attachShader(const ShaderModule *shaderStage) {
        shaderModules.emplace_back(shaderStage);
    }

    inline std::vector<VkDescriptorSet> PipelineManager::getDescriptorSets(uint32_t frameIndex) {
        std::vector<VkDescriptorSet> sets(descriptors.size());
        std::ranges::transform(descriptors, sets.begin(), [frameIndex](const Descriptor *desc) {
            return desc->getDescriptorSetHandle(frameIndex);
        });
        return sets;
    }
}
