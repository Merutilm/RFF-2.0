//
// Created by Merutilm on 2025-08-29.
//

#include "RenderPassSyncObject.hpp"
namespace merutilm::vkh {
    RenderPassSyncObjectImpl::RenderPassSyncObjectImpl(CoreRef core) : CoreHandler(core) {
        RenderPassSyncObjectImpl::init();
    }

    RenderPassSyncObjectImpl::~RenderPassSyncObjectImpl() {
        RenderPassSyncObjectImpl::destroy();
    }

    void RenderPassSyncObjectImpl::init() {
        
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        constexpr VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        fences.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (vkCreateFence(device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create sync objects!");
                }
        }
    }

    void RenderPassSyncObjectImpl::destroy() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyFence(device, fences[i], nullptr);
        }
    }
}