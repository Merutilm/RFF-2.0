//
// Created by Merutilm on 2025-07-09.
//

#include "Descriptor.hpp"

#include <iostream>

#include "DescriptorSetLayout.hpp"
#include "../context/DescriptorUpdateContext.hpp"
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    DescriptorImpl::DescriptorImpl(CoreRef core, DescriptorSetLayoutRef descriptorSetLayout,
                           DescriptorManager &&descriptorManager) : CoreHandler(core),
        descriptorSetLayout(descriptorSetLayout),
        descriptorManager(std::move(descriptorManager)) {
        DescriptorImpl::init();
    }

    DescriptorImpl::~DescriptorImpl() {
        DescriptorImpl::destroy();
    }



    void DescriptorImpl::queue(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, const uint32_t imageIndex) const {
        const uint32_t elementCount = descriptorManager->getElements();
        std::vector<uint32_t> specifiedIndices(elementCount);
        std::iota(specifiedIndices.begin(), specifiedIndices.end(), 0);
        updateIndices(updateQueue, frameIndex, imageIndex, specifiedIndices);
    }


    void DescriptorImpl::queue(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, const uint32_t imageIndex, std::vector<uint32_t> &&bindings) const {
        auto bm = std::move(bindings);
        std::ranges::sort(bm);
        updateIndices(updateQueue, frameIndex, imageIndex, bm);
    }

    void DescriptorImpl::updateIndices(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, const uint32_t imageIndex,
                                   const std::vector<uint32_t> &indices) const {
        for (const uint32_t index: indices) {
            const auto &raw = descriptorManager->getRaw(index);
            if (std::holds_alternative<Uniform>(raw)) {
                auto &ubo = std::get<Uniform>(raw);


                updateQueue.push_back({
                    .bufferInfo = VkDescriptorBufferInfo{
                        .buffer = ubo->getBufferHandle(frameIndex),
                        .offset = 0,
                        .range = ubo->getHostObject().getTotalSizeByte()
                    },
                });
                updateQueue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &updateQueue.back().bufferInfo,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<ShaderStorage >(raw)) {
                auto &ssbo = std::get<ShaderStorage >(raw);


                updateQueue.push_back({
                    .bufferInfo = VkDescriptorBufferInfo{
                        .buffer = ssbo->getBufferHandle(frameIndex),
                        .offset = 0,
                        .range = ssbo->getHostObject().getTotalSizeByte()
                    },
                });
                updateQueue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &updateQueue.back().bufferInfo,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<CombinedImageSampler>(raw)) {
                auto &tex = std::get<CombinedImageSampler>(raw);

                updateQueue.push_back({
                    .imageInfo = VkDescriptorImageInfo{
                        .sampler = tex->getSampler().getSamplerHandle(),
                        .imageView = tex->getImageContext().readImageView,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    }
                });

                updateQueue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &updateQueue.back().imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<CombinedMultiframeImageSampler>(raw)) {
                auto &tex = std::get<CombinedMultiframeImageSampler>(raw);

                updateQueue.push_back({
                    .imageInfo = VkDescriptorImageInfo{
                        .sampler = tex->getSampler().getSamplerHandle(),
                        .imageView = tex->getImageContext()[imageIndex].readImageView,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    }
                });

                updateQueue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &updateQueue.back().imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr,
                };
            }
            if (std::holds_alternative<MultiframeImageContext>(raw)) {
                auto &input = std::get<MultiframeImageContext>(raw);
                updateQueue.push_back({
                    .imageInfo = VkDescriptorImageInfo{
                        .sampler = VK_NULL_HANDLE,
                        .imageView = input[imageIndex].readImageView,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    }
                });

                updateQueue.back().writeSet = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = descriptorSets[frameIndex],
                    .dstBinding = index,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .pImageInfo = &updateQueue.back().imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr,
                };
            }
        }
    }


    void DescriptorImpl::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const uint32_t ubo = descriptorManager->getElementCount<Uniform>();
        const uint32_t ssbo = descriptorManager->getElementCount<ShaderStorage>();
        const uint32_t sampler = descriptorManager->getElementCount<CombinedImageSampler>();
        const uint32_t multiframeSampler = descriptorManager->getElementCount<CombinedMultiframeImageSampler>();
        const uint32_t inputAttachment = descriptorManager->getElementCount<MultiframeImageContext>();
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

        if (sampler + multiframeSampler > 0) {
            sizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = sampler + multiframeSampler
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

    void DescriptorImpl::destroy() {
        if (descriptorManager->getElements() == 0) {
            return;
        }
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();

        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyDescriptorPool(core.getLogicalDevice().getLogicalDeviceHandle(), descriptorPools[i], nullptr);
        }
    }
}
