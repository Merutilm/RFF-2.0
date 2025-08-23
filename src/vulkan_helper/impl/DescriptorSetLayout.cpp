//
// Created by Merutilm on 2025-07-13.
//


#include "DescriptorSetLayout.hpp"
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    DescriptorSetLayout::DescriptorSetLayout(const CoreRef core, const DescriptorSetLayoutBuilder &layoutBuilder) : CoreHandler(core), layoutBuilder(layoutBuilder) {
        DescriptorSetLayout::init();
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        DescriptorSetLayout::destroy();
    }

    void DescriptorSetLayout::init() {
        if (layout != nullptr) {
            throw exception_invalid_state("double-finish called");
        }

        std::vector<VkDescriptorSetLayoutBinding> bindings(layoutBuilder.size());
        for (uint32_t i = 0; i < layoutBuilder.size(); ++i) {
            bindings[i] = {
                .binding = i,
                .descriptorType = layoutBuilder[i].type,
                .descriptorCount = 1,
                .stageFlags = layoutBuilder[i].stage,
                .pImmutableSamplers = nullptr
            };
        }


        const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.empty() ? nullptr : bindings.data(),
        };
        if (vkCreateDescriptorSetLayout(core.getLogicalDevice().getLogicalDeviceHandle(), &descriptorSetLayoutInfo, nullptr, &layout) !=
            VK_SUCCESS) {
            throw exception_init("Failed to create descriptor set layout");
            }
    }



    void DescriptorSetLayout::destroy() {
        if (layout != nullptr) {
            vkDestroyDescriptorSetLayout(core.getLogicalDevice().getLogicalDeviceHandle(), layout, nullptr);
        }
    }
}
