//
// Created by Merutilm on 2025-07-13.
//

#include <vulkan_helper/engine/pipeline/PipelineLayout.hpp>

namespace merutilm::vkh {
    PipelineLayout::PipelineLayout(Core &core, PipelineLayoutManager &&pipelineLayoutManager) :
        CoreHandler(core), builders(std::move(pipelineLayoutManager.builders)),
        descriptorSetLayoutCount(std::move(pipelineLayoutManager.descriptorSetLayoutCount)) {
        PipelineLayout::init();
    }

    PipelineLayout::~PipelineLayout() { PipelineLayout::cleanup(); }

    void PipelineLayout::cmdPush(const VkCommandBuffer commandBuffer) const {
        uint32_t sizeSum = 0;
        for (auto &pushConstant: getPushConstants()) {
            const uint32_t size = pushConstant->getHostObject().getTotalSizeByte();
            vkCmdPushConstants(commandBuffer, layout, pushConstant->getUseStage(), sizeSum, size,
                               pushConstant->getHostObject().getData().data());
            sizeSum += size;
        }
    }

    std::vector<DescriptorSetLayout *> PipelineLayout::getDescriptorSetLayouts() const {
        std::vector<DescriptorSetLayout *> v(descriptorSetLayoutCount, nullptr);
        std::transform(builders.begin(), builders.begin() + descriptorSetLayoutCount, v.begin(),
                       [](const PipelineLayoutBuildType &type) { return std::get<DescriptorSetLayout *>(type); });
        return v;
    }

    std::vector<PushConstant *> PipelineLayout::getPushConstants() const {
        std::vector<PushConstant *> v(builders.size() - descriptorSetLayoutCount, nullptr);
        std::transform(builders.begin() + descriptorSetLayoutCount, builders.end(), v.begin(),
                       [](const PipelineLayoutBuildType &type) { return std::get<PushConstant *>(type); });
        return v;
    }

    PushConstant *PipelineLayout::getPushConstant(const uint32_t pushIndex) const {
        return std::get<PushConstant *>(builders[pushIndex - descriptorSetLayoutCount]);
    }


    void PipelineLayout::init() {
        uint32_t sizeSum = 0;
        std::vector<VkPushConstantRange> pushConstantRanges = {};
        for (const auto &pushConstantManager: getPushConstants()) {
            const uint32_t size = pushConstantManager->getHostObject().getTotalSizeByte();
            pushConstantRanges.emplace_back(pushConstantManager->getUseStage(), sizeSum, size);
            sizeSum += size;
        }

        std::vector<VkDescriptorSetLayout> layouts = {};
        for (const auto &layout: getDescriptorSetLayouts()) {
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


        if (vkCreatePipelineLayout(core.getLogicalDevice().getLogicalDeviceHandle(),
                              &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
            throw exception_init("Failed to create pipeline layout!");
        }
    }


    void PipelineLayout::cleanup() {
        vkDestroyPipelineLayout(core.getLogicalDevice().getLogicalDeviceHandle(), layout, nullptr);
    }
} // namespace merutilm::vkh
