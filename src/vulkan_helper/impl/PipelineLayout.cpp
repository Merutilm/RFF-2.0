//
// Created by Merutilm on 2025-07-13.
//

#include "PipelineLayout.hpp"

namespace merutilm::vkh {
    PipelineLayoutImpl::PipelineLayoutImpl(const CoreRef core,
                                   PipelineLayoutManager &&pipelineLayoutManager) : CoreHandler(
        core), pipelineLayoutManager(std::move(pipelineLayoutManager)) {
        PipelineLayoutImpl::init();
    }

    PipelineLayoutImpl::~PipelineLayoutImpl() {
        PipelineLayoutImpl::destroy();
    }

    void PipelineLayoutImpl::cmdPush(const VkCommandBuffer commandBuffer) const {
        uint32_t sizeSum = 0;
        for (auto &pushConstant: pipelineLayoutManager->getPushConstantManagers()) {
            const uint32_t size = pushConstant->getTotalSizeByte();
            vkCmdPushConstants(commandBuffer, layout, pushConstant->getUseStage(),
                             sizeSum, size,
                             pushConstant->getData().data());
            sizeSum += size;
        }
    }


    void PipelineLayoutImpl::init() {
        uint32_t sizeSum = 0;
        std::vector<VkPushConstantRange> pushConstantRanges = {};
        for (const auto &pushConstantManager: pipelineLayoutManager->getPushConstantManagers()) {
            const uint32_t size = pushConstantManager->getTotalSizeByte();
            pushConstantRanges.emplace_back(pushConstantManager->getUseStage(), sizeSum, size);
            sizeSum += size;
        }

        std::vector<VkDescriptorSetLayout> layouts = {};
        for (const auto &layout: pipelineLayoutManager->getDescriptorSetLayouts()){
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


        if (vkCreatePipelineLayout(core.getLogicalDevice().getLogicalDeviceHandle(), &pipelineLayoutInfo, nullptr,
                                   &layout) !=
            VK_SUCCESS) {
            throw exception_init("Failed to create pipeline layout!");
        }
    }


    void PipelineLayoutImpl::destroy() {
        vkDestroyPipelineLayout(core.getLogicalDevice().getLogicalDeviceHandle(), layout, nullptr);
    }
}
