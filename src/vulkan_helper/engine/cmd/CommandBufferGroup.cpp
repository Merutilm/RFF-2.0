//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/cmd/CommandBufferGroup.hpp>

#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    CommandBufferGroup::CommandBufferGroup(Core &core, CommandPool &commandPool) :
        CoreHandler(core), commandPool(commandPool) {
        CommandBufferGroup::init();
    }

    CommandBufferGroup::~CommandBufferGroup() { CommandBufferGroup::cleanup(); }

    void CommandBufferGroup::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        commandBuffers.resize(maxFramesInFlight);
        for (auto &cb: commandBuffers) {
            cb = std::make_unique<CommandBuffer>(core, commandPool);
        }
    }

    void CommandBufferGroup::cleanup() {
        commandBuffers.clear();
    }


} // namespace merutilm::vkh
