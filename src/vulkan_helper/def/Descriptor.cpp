//
// Created by Merutilm on 2025-07-09.
//

#include "Descriptor.hpp"

#include <iostream>

#include "DescriptorSetLayout.hpp"
#include "../context/DescriptorUpdateContext.hpp"
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    Descriptor::Descriptor(const Core &core, const DescriptorSetLayout &descriptorSetLayout,
                           std::unique_ptr<DescriptorManager> &&descriptorManager) : CoreHandler(core),
        descriptorSetLayout(descriptorSetLayout),
        descriptorManager(std::move(descriptorManager)) {
        Descriptor::init();
    }

    Descriptor::~Descriptor() {
        Descriptor::destroy();
    }


    void Descriptor::updateIndices(DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                   const std::vector<uint32_t> &indices) const {
        for (const uint32_t index: indices) {
            const auto &raw = descriptorManager->getRaw(index);
            if (std::holds_alternative<std::unique_ptr<Uniform> >(raw)) {
                auto &ubo = std::get<std::unique_ptr<Uniform> >(raw);


                queue.push_back({
                    .bufferInfo = VkDescriptorBufferInfo{
                        .buffer = ubo->getBufferHandle(frameIndex),
                        .offset = 0,
                        .range = ubo->getHostObject().getTotalSizeByte()
                    },
                });
                queue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &queue.back().bufferInfo,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<std::unique_ptr<ShaderStorage> >(raw)) {
                auto &ssbo = std::get<std::unique_ptr<ShaderStorage> >(raw);


                queue.push_back({
                    .bufferInfo = VkDescriptorBufferInfo{
                        .buffer = ssbo->getBufferHandle(frameIndex),
                        .offset = 0,
                        .range = ssbo->getHostObject().getTotalSizeByte()
                    },
                });
                queue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &queue.back().bufferInfo,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<std::unique_ptr<CombinedImageSampler> >(raw)) {
                auto &tex = std::get<std::unique_ptr<CombinedImageSampler> >(raw);

                queue.push_back({
                    .imageInfo = VkDescriptorImageInfo{
                        .sampler = tex->getSampler().getSamplerHandle(),
                        .imageView = tex->getImageContext().imageView,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    }
                });

                queue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &queue.back().imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<ImageContext>(raw)) {
                auto &input = std::get<ImageContext>(raw);
                queue.push_back({
                    .imageInfo = VkDescriptorImageInfo{
                        .sampler = VK_NULL_HANDLE,
                        .imageView = input.imageView,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    }
                });

                queue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .pImageInfo = &queue.back().imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr,
                };
            }
        }
    }


    void Descriptor::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const uint32_t ubo = descriptorManager->getElementCount<std::unique_ptr<Uniform> >();
        const uint32_t ssbo = descriptorManager->getElementCount<std::unique_ptr<ShaderStorage> >();
        const uint32_t sampler = descriptorManager->getElementCount<std::unique_ptr<CombinedImageSampler> >();
        const uint32_t inputAttachment = descriptorManager->getElementCount<ImageContext>();
        const uint32_t elements = descriptorManager->getElements();

        std::vector<VkDescriptorPoolSize> sizes = {};

        if (ubo > 0) {
            sizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = ubo
            });
        }
        if (ssbo > 0) {
            sizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = ssbo
            });
        }

        if (sampler > 0) {
            sizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = sampler
            });
        }

        if (inputAttachment > 0) {
            sizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .descriptorCount = inputAttachment
            });
        }

        const VkDescriptorPoolCreateInfo descriptorPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = elements,
            .poolSizeCount = static_cast<uint32_t>(sizes.size()),
            .pPoolSizes = sizes.data()
        };
        descriptorPools.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; i++) {
            if (vkCreateDescriptorPool(core.getLogicalDevice().getLogicalDeviceHandle(), &descriptorPoolInfo, nullptr,
                                       &descriptorPools[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create descriptor pool!");
            }
        }

        VkDescriptorSetLayout layout = descriptorSetLayout.getLayoutHandle();

        descriptorSets.resize(maxFramesInFlight);
        for (int i = 0; i < maxFramesInFlight; ++i) {
            if (const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = nullptr,
                .descriptorPool = descriptorPools[i],
                .descriptorSetCount = 1,
                .pSetLayouts = &layout
            }; vkAllocateDescriptorSets(core.getLogicalDevice().getLogicalDeviceHandle(), &descriptorSetAllocateInfo,
                                        &descriptorSets[i]) != VK_SUCCESS) {
                throw exception_init("Failed to allocate descriptor sets!");
            }
        }
    }

    void Descriptor::destroy() {
        if (descriptorManager->getElements() == 0) {
            return;
        }
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();

        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyDescriptorPool(core.getLogicalDevice().getLogicalDeviceHandle(), descriptorPools[i], nullptr);
        }
    }
}
