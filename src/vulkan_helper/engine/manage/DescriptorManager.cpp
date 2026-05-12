//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/manage/DescriptorManager.hpp>

namespace merutilm::vkh {
    DescriptorManager::DescriptorManager() = default;

    DescriptorManager::~DescriptorManager() = default;

    void DescriptorManager::appendUBO(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
        std::unique_ptr<Uniform> &&ubo) {
        safe_array::check_index_equal(bindingExpected, static_cast<uint32_t>(data.size()),
                                      "Descriptor UBO add");
        data.emplace_back(std::move(ubo));
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, useStage);
    }

    void DescriptorManager::appendSSBO(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
         std::unique_ptr<ShaderStorage> &&ssbo) {
        safe_array::check_index_equal(bindingExpected, static_cast<uint32_t>(data.size()),
                                      "Descriptor SSBO add");
        data.emplace_back(std::move(ssbo));
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, useStage);
    }

    void DescriptorManager::appendCombinedImgSampler(const uint32_t bindingExpected,
        const VkShaderStageFlags useStage,  std::unique_ptr<CombinedImageSampler> &&sampler) {
        safe_array::check_index_equal(bindingExpected, static_cast<uint32_t>(data.size()),
                                      "Descriptor Sampler add");
        data.emplace_back(std::move(sampler));
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, useStage);
    }

    void DescriptorManager::appendInputAttachment(const uint32_t bindingExpected,
        const VkShaderStageFlags useStage) {
        safe_array::check_index_equal(bindingExpected, static_cast<uint32_t>(data.size()),
                                      "Descriptor Input Attachment add");
        data.emplace_back(std::make_unique<InputAttachment>());
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, useStage);
    }

    void DescriptorManager::appendStorageImage(const uint32_t bindingExpected,
        const VkShaderStageFlags useStage) {
        safe_array::check_index_equal(bindingExpected, static_cast<uint32_t>(data.size()),
                                      "Descriptor Image2D add");
        data.emplace_back(std::make_unique<StorageImage>());
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, useStage);
    }
}