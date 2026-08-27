//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/cmd/CommandBufferGroup.hpp>

#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    CommandBufferGroup::CommandBufferGroup(Core &core, CommandPool &commandPool) : CoreHandler(core), commandPool(commandPool) {
        CommandBufferGroup::init();
    }

    CommandBufferGroup::~CommandBufferGroup() { CommandBufferGroup::cleanup(); }

    void CommandBufferGroup::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        commandBuffers.resize(maxFramesInFlight);
        const VkCommandBufferAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                       .pNext = nullptr,
                                                       .commandPool = commandPool.getCommandPoolHandle(),
                                                       .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                       .commandBufferCount =
                                                               static_cast<uint32_t>(commandBuffers.size())};


        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw exception_init("Failed to allocate command buffers!");
        }
    }

    void CommandBufferGroup::cleanup() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkFreeCommandBuffers(device, commandPool.getCommandPoolHandle(), static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());
    }


} // namespace merutilm::vkh
