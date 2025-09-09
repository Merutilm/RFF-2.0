//
// Created by Merutilm on 2025-07-13.
//

#include "PipelineLayout.hpp"

namespace merutilm::vkh {
    PipelineLayoutImpl::PipelineLayoutImpl(CoreRef core,
                                   PipelineLayoutManager &&pipelineLayoutManager) : CoreHandler(
        core), builders(std::move(pipelineLayoutManager->builders)), descriptorSetLayoutCount(std::move(pipelineLayoutManager->descriptorSetLayoutCount)) {
        PipelineLayoutImpl::init();
    }

    PipelineLayoutImpl::~PipelineLayoutImpl() {
        PipelineLayoutImpl::destroy();
    }

    void PipelineLayoutImpl::cmdPush(const VkCommandBuffer commandBuffer) const {
        uint32_t sizeSum = 0;
        for (auto &pushConstant : getPushConstants()) {
            const uint32_t size = pushConstant->getHostObject().getTotalSizeByte();
            vkCmdPushConstants(commandBuffer, layout, pushConstant->getUseStage(),
                             sizeSum, size,
                             pushConstant->getHostObject().getData().data());
            sizeSum += size;
        }
    }


    void PipelineLayoutImpl::init() {
        uint32_t sizeSum = 0;
        std::vector<VkPushConstantRange> pushConstantRanges = {};
        for (const auto &pushConstantManager: getPushConstants()) {
            const uint32_t size = pushConstantManager->getHostObject().getTotalSizeByte();
            pushConstantRanges.emplace_back(pushConstantManager->getUseStage(), sizeSum, size);
            sizeSum += size;
        }

        std::vector<VkDescriptorSetLayout> layouts = {};
        for (const auto &layout: getDescriptorSetLayouts()){
            layouts.push_back(layout->getLayoutHandle());
        }

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<uint32_t>(layouts.size()),
            .pSetLayouts = layouts.empty() ? nullptr : layouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
            .pPushConstantRanges = pushConstantRanges.empty() ? nullptr : pushConstantRanges.data(),
        };


        if (allocator::invoke(vkCreatePipelineLayout, core.getLogicalDevice().getLogicalDeviceHandle(), &pipelineLayoutInfo, nullptr,
                                   &layout) !=
            VK_SUCCESS) {
            throw exception_init("Failed to create pipeline layout!");
        }
    }


    void PipelineLayoutImpl::destroy() {
        allocator::invoke(vkDestroyPipelineLayout, core.getLogicalDevice().getLogicalDeviceHandle(), layout, nullptr);
    }
}
