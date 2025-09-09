//
// Created by Merutilm on 2025-08-15.
//

#pragma once

#include "../context/DescriptorUpdateContext.hpp"

namespace merutilm::vkh {
    struct DescriptorUpdater {
        DescriptorUpdater() = delete;

        static DescriptorUpdateQueue createQueue() {
            return {};
        }

        static void write(const VkDevice device, const DescriptorUpdateQueue &queue) {

            std::vector<VkWriteDescriptorSet> writeDescriptorSets(queue.size());
            std::ranges::transform(queue, writeDescriptorSets.begin(), [](const DescriptorUpdateContext &context) {
                return context.writeSet;
            });

            allocator::invoke(vkUpdateDescriptorSets, device,
                                   static_cast<uint32_t>(writeDescriptorSets.size()),
                                   writeDescriptorSets.data(), 0, nullptr);
        }

    };


}
