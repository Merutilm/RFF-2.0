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


namespace merutilm::vkh {
    class DescriptorImpl final : public CoreHandler {
        std::vector<VkDescriptorPool> descriptorPools = {};
        std::vector<VkDescriptorSet> descriptorSets = {};
        DescriptorSetLayoutRef descriptorSetLayout;
        const DescriptorManager descriptorManager = nullptr;

    public:
        explicit DescriptorImpl(CoreRef core, DescriptorSetLayoutRef descriptorSetLayout,
                            DescriptorManager &&descriptorManager);

        ~DescriptorImpl() override;

        DescriptorImpl(const DescriptorImpl &) = delete;

        DescriptorImpl &operator=(const DescriptorImpl &) = delete;

        DescriptorImpl(DescriptorImpl &&) = delete;

        DescriptorImpl &operator=(DescriptorImpl &&) = delete;

        [[nodiscard]] DescriptorManagerRef getDescriptorManager() const { return *descriptorManager; }

        [[nodiscard]] VkDescriptorSet getDescriptorSetHandle(const uint32_t frameIndex) const {
            return descriptorSets[frameIndex];
        }

        [[nodiscard]] VkDescriptorPool getDescriptorPoolHandle(const uint32_t frameIndex) const {
            return descriptorPools[frameIndex];
        }

        void queue(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex, uint32_t imageIndex) const;

        void queue(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex, uint32_t imageIndex, std::vector<uint32_t> &&bindings) const;

    private:

        void updateIndices(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex, uint32_t imageIndex, const std::vector<uint32_t> &indices) const;

        void init() override;

        void destroy() override;
    };

    
    using Descriptor = std::unique_ptr<DescriptorImpl>;
    using DescriptorPtr = DescriptorImpl *;
    using DescriptorRef = DescriptorImpl &;
}
