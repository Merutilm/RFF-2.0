//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/descriptor/Descriptor.hpp>


namespace merutilm::vkh {
    Descriptor::Descriptor(Core &core, DescriptorSetLayout &descriptorSetLayout,
                           std::vector<DescriptorManager> &&manager) :
        CoreHandler(core), descriptorSetLayout(descriptorSetLayout) {
        data.resize(manager.size());

        std::transform(std::make_move_iterator(manager.begin()), std::make_move_iterator(manager.end()), data.begin(),
                       [](DescriptorManager &&element) { return std::move(element.data); });
        Descriptor::init();
    }

    Descriptor::~Descriptor() { Descriptor::cleanup(); }


    void Descriptor::queue(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, DescIndexPicker &&descIndices,
                           DescIndexPicker &&bindings) {
        if (descIndices.empty()) {
            const uint32_t descriptorCount = getDescriptorCount();
            descIndices = std::vector<uint32_t>(descriptorCount);
            std::iota(descIndices.begin(), descIndices.end(), 0);
        }


        if (bindings.empty()) {
            const uint32_t elementCount = getDescriptorElements();
            bindings = std::vector<uint32_t>(elementCount);
            std::iota(bindings.begin(), bindings.end(), 0);
        }

        updateIndices(updateQueue, frameIndex, std::move(descIndices), std::move(bindings));
    }


    void Descriptor::updateUBO(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, const uint32_t descIndex,
                               const uint32_t binding, const Uniform &ubo) const {
        updateQueue.push_back({
                .bufferInfo =
                        VkDescriptorBufferInfo{.buffer = ubo.isMultiframe() ? ubo.getBufferContextMF(frameIndex).buffer
                                                                            : ubo.getBufferContext().buffer,
                                               .offset = 0,
                                               .range = ubo.getHostObject().getTotalSizeByte()},
        });
        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &updateQueue.back().bufferInfo,
                .pTexelBufferView = nullptr,
        };
    }
    void Descriptor::updateSSBO(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex, const uint32_t descIndex,
                                const uint32_t binding, const ShaderStorage &ssbo) const {

        updateQueue.push_back({
                .bufferInfo = VkDescriptorBufferInfo{.buffer = ssbo.isMultiframe()
                                                                       ? ssbo.getBufferContextMF(frameIndex).buffer
                                                                       : ssbo.getBufferContext().buffer,
                                                     .offset = 0,
                                                     .range = ssbo.getHostObject().getTotalSizeByte()},
        });
        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &updateQueue.back().bufferInfo,
                .pTexelBufferView = nullptr,
        };
    }
    void Descriptor::updateExternSSBO(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex,
                                      const uint32_t descIndex, const uint32_t binding,
                                      const ExternShaderStorage &ssbo) const {

        updateQueue.push_back({
                .bufferInfo = VkDescriptorBufferInfo{.buffer = ssbo.context.buffer,
                                                     .offset = 0,
                                                     .range = ssbo.context.bufferSize},
        });
        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &updateQueue.back().bufferInfo,
                .pTexelBufferView = nullptr,
        };
    }
    void Descriptor::updateCombinedImageSampler(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex,
                                                const uint32_t descIndex, const uint32_t binding,
                                                const CombinedImageSampler &sampler) const {

        updateQueue.push_back({.imageInfo = VkDescriptorImageInfo{
                                       .sampler = sampler.getSampler().getSamplerHandle(),
                                       .imageView = sampler.isMultiframe()
                                                            ? sampler.getImageContextMF()[frameIndex].mipmappedImageView
                                                            : sampler.getImageContext().mipmappedImageView,
                                       .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                               }});

        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &updateQueue.back().imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
        };
    }
    void Descriptor::updateInputAttachment(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex,
                                           const uint32_t descIndex, const uint32_t binding,
                                           const InputAttachment &inputAttachment) const {
        updateQueue.push_back(
                {.imageInfo = VkDescriptorImageInfo{.sampler = VK_NULL_HANDLE,
                                                    .imageView = inputAttachment.ctx[frameIndex].mipmappedImageView,
                                                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});

        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .pImageInfo = &updateQueue.back().imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
        };
    }
    void Descriptor::updateStorageImage(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex,
                                        const uint32_t descIndex, const uint32_t binding,
                                        const StorageImage &img) const {
        updateQueue.push_back(
                {.imageInfo = VkDescriptorImageInfo{
                         .sampler = VK_NULL_HANDLE,
                         .imageView = img.isMultiframe() ? img.getImageContextMF()[frameIndex].mipmappedImageView
                                                         : img.getImageContext().mipmappedImageView,
                         .imageLayout = VK_IMAGE_LAYOUT_GENERAL}});

        updateQueue.back().writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSets[frameIndex][descIndex],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .pImageInfo = &updateQueue.back().imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
        };
    }


    void Descriptor::updateIndices(DescriptorUpdateQueue &updateQueue, const uint32_t frameIndex,
                                   std::vector<uint32_t> &&descIndices, std::vector<uint32_t> &&bindings) {
        for (const uint32_t descIndex: std::move(descIndices)) {
            for (const uint32_t binding: std::move(bindings)) {
                auto &raw = getRaw(descIndex, binding);
                if (std::holds_alternative<std::unique_ptr<Uniform>>(raw)) {
                    const auto &ubo = *std::get<std::unique_ptr<Uniform>>(raw);
                    updateUBO(updateQueue, frameIndex, descIndex, binding, ubo);
                }
                else if (std::holds_alternative<std::unique_ptr<ShaderStorage>>(raw)) {
                    const auto &ssbo = *std::get<std::unique_ptr<ShaderStorage>>(raw);
                    updateSSBO(updateQueue, frameIndex, descIndex, binding, ssbo);
                }
                else if (std::holds_alternative<std::unique_ptr<ExternShaderStorage>>(raw)) {
                    const auto &ssbo = *std::get<std::unique_ptr<ExternShaderStorage>>(raw);
                    updateExternSSBO(updateQueue, frameIndex, descIndex, binding, ssbo);
                }
                else if (std::holds_alternative<std::unique_ptr<CombinedImageSampler>>(raw)) {
                    const auto &sampler = *std::get<std::unique_ptr<CombinedImageSampler>>(raw);
                    updateCombinedImageSampler(updateQueue, frameIndex, descIndex, binding, sampler);
                }
                else if (std::holds_alternative<std::unique_ptr<InputAttachment>>(raw)) {
                    const auto &inputAttachment = *std::get<std::unique_ptr<InputAttachment>>(raw);
                    updateInputAttachment(updateQueue, frameIndex, descIndex, binding, inputAttachment);
                }
                else if (std::holds_alternative<std::unique_ptr<StorageImage>>(raw)) {
                    const auto &img = *std::get<std::unique_ptr<StorageImage>>(raw);
                    updateStorageImage(updateQueue, frameIndex, descIndex, binding, img);
                } else throw exception_invalid_state("unhandled descriptor type");
            }
        }
    }

    void Descriptor::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        const uint32_t ubo = getElementCount<Uniform>();
        const uint32_t ssbo = getElementCount<ShaderStorage>() + getElementCount<ExternShaderStorage>();
        const uint32_t sampler = getElementCount<CombinedImageSampler>();
        const uint32_t inputAttachment = getElementCount<InputAttachment>();
        const uint32_t storageImage = getElementCount<StorageImage>();
        const uint32_t elements = getDescriptorElements();

        const uint32_t descriptorCount = getDescriptorCount();
        std::vector<VkDescriptorPoolSize> sizes = {};

        if (ubo > 0) {
            sizes.push_back({.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = ubo * descriptorCount});
        }
        if (ssbo > 0) {
            sizes.push_back({.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = ssbo * descriptorCount});
        }

        if (sampler > 0) {
            sizes.push_back(
                    {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = sampler * descriptorCount});
        }
        if (inputAttachment > 0) {
            sizes.push_back({.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                             .descriptorCount = inputAttachment * descriptorCount});
        }
        if (storageImage > 0) {
            sizes.push_back(
                    {.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = storageImage * descriptorCount});
        }

        const VkDescriptorPoolCreateInfo descriptorPoolInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                                               .pNext = nullptr,
                                                               .flags = 0,
                                                               .maxSets = elements * descriptorCount,
                                                               .poolSizeCount = static_cast<uint32_t>(sizes.size()),
                                                               .pPoolSizes = sizes.data()};
        descriptorPools.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (vkCreateDescriptorPool(core.getLogicalDevice().getLogicalDeviceHandle(), &descriptorPoolInfo, nullptr,
                                       &descriptorPools[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create descriptor pool!");
            }
        }

        std::vector layouts(descriptorCount, descriptorSetLayout.getLayoutHandle());

        descriptorSets.resize(maxFramesInFlight);

        for (int i = 0; i < maxFramesInFlight; ++i) {
            descriptorSets[i].resize(descriptorCount);

            if (const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
                        {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                         .pNext = nullptr,
                         .descriptorPool = descriptorPools[i],
                         .descriptorSetCount = descriptorCount,
                         .pSetLayouts = layouts.data()};
                vkAllocateDescriptorSets(core.getLogicalDevice().getLogicalDeviceHandle(), &descriptorSetAllocateInfo,
                                         descriptorSets[i].data()) != VK_SUCCESS) {
                throw exception_init("Failed to allocate descriptor sets!");
            }
        }
    }

    void Descriptor::cleanup() {
        if (getDescriptorElements() == 0) {
            return;
        }
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();

        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyDescriptorPool(core.getLogicalDevice().getLogicalDeviceHandle(), descriptorPools[i], nullptr);
        }
    }
} // namespace merutilm::vkh
