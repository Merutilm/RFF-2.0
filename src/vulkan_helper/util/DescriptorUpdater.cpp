//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/util/DescriptorUpdater.hpp>

namespace merutilm::vkh {
    DescriptorUpdateQueue DescriptorUpdater::createQueue() {
        return {};
    }

    void DescriptorUpdater::write(const VkDevice device, const DescriptorUpdateQueue &queue) {

        std::vector<VkWriteDescriptorSet> writeDescriptorSets(queue.size());
        std::ranges::transform(queue, writeDescriptorSets.begin(), [](const DescriptorUpdateContext &context) {
            return context.writeSet;
        });

        vkUpdateDescriptorSets(device,
                          static_cast<uint32_t>(writeDescriptorSets.size()),
                          writeDescriptorSets.data(), 0, nullptr);
    }
}
