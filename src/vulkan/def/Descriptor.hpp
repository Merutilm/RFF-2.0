//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

#include "DescriptorSetLayout.hpp"
#include "../context/DescriptorUpdateContext.hpp"
#include "../manage/DescriptorManager.hpp"


namespace merutilm::mvk {
    class Descriptor final : public CoreHandler {
        std::vector<VkDescriptorPool> descriptorPools = {};
        std::vector<VkDescriptorSet> descriptorSets = {};
        const DescriptorSetLayout &descriptorSetLayout;
        const std::unique_ptr<DescriptorManager> descriptorManager = nullptr;

    public:
        explicit Descriptor(const Core &core, const DescriptorSetLayout &descriptorSetLayout,
                            std::unique_ptr<DescriptorManager> &&descriptorManager);

        ~Descriptor() override;

        Descriptor(const Descriptor &) = delete;

        Descriptor &operator=(const Descriptor &) = delete;

        Descriptor(Descriptor &&) = delete;

        Descriptor &operator=(Descriptor &&) = delete;

        [[nodiscard]] DescriptorManager &getDescriptorManager() const { return *descriptorManager; }

        [[nodiscard]] VkDescriptorSet getDescriptorSetHandle(const uint32_t frameIndex) const {
            return descriptorSets[frameIndex];
        }

        [[nodiscard]] VkDescriptorPool getDescriptorPoolHandle(const uint32_t frameIndex) const {
            return descriptorPools[frameIndex];
        }

        void updateQueue(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex) const;

        void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex, std::vector<uint32_t> &&bindings) const;

    private:

        void updateIndices(DescriptorUpdateQueue &queue, uint32_t frameIndex, const std::vector<uint32_t> &indices) const;

        void init() override;

        void destroy() override;
    };


    inline void Descriptor::updateQueue(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex) const {
        const uint32_t elementCount = descriptorManager->getElements();
        std::vector<uint32_t> specifiedIndices(elementCount);
        std::iota(specifiedIndices.begin(), specifiedIndices.end(), 0);
        updateIndices(updateQueue, frameIndex, specifiedIndices);
    }


    inline void Descriptor::updateQueue(DescriptorUpdateQueue &queue, const uint32_t frameIndex, std::vector<uint32_t> &&bindings) const {
        auto bm = std::move(bindings);
        std::ranges::sort(bm);
        updateIndices(queue, frameIndex, bm);
    }
}
