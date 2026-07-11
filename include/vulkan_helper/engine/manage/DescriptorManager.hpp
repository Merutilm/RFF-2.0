//
// Created by Merutilm on 2025-07-10.
//

#pragma once

#include <vulkan_helper/engine/buffer/ShaderStorage.hpp>
#include <vulkan_helper/engine/buffer/Uniform.hpp>
#include <vulkan_helper/engine/sampler/CombinedImageSampler.hpp>
#include <vulkan_helper/engine/wrapped/DescriptorSetLayoutBuildType.hpp>
#include <vulkan_helper/engine/wrapped/InputAttachment.hpp>
#include <vulkan_helper/engine/wrapped/StorageImage.hpp>
#include <vulkan_helper/hash/DescriptorSetLayoutBuildTypeHasher.hpp>
#include <vulkan_helper/hash/VectorHasher.hpp>

namespace merutilm::vkh {
    using DescriptorSetLayoutBuilder = std::vector<DescriptorSetLayoutBuildType>;
    using DescriptorSetLayoutBuilderHasher =
            VectorHasher<DescriptorSetLayoutBuildType, DescriptorSetLayoutBuildTypeHasher>;

    using DescriptorType = std::variant<std::unique_ptr<Uniform>, std::unique_ptr<ShaderStorage>,
                                        std::unique_ptr<CombinedImageSampler>, std::unique_ptr<InputAttachment>,
                                        std::unique_ptr<StorageImage>>;


    struct DescriptorManager {
        std::vector<DescriptorType> data = {};
        DescriptorSetLayoutBuilder layoutBuilder = {};

        explicit DescriptorManager();

        ~DescriptorManager();

        DescriptorManager(const DescriptorManager &) = delete;

        DescriptorManager &operator=(const DescriptorManager &) = delete;

        DescriptorManager(DescriptorManager &&) noexcept = default;

        DescriptorManager &operator=(DescriptorManager &&) noexcept = delete;


        void appendUBO(uint32_t bindingExpected, VkShaderStageFlags useStage,  std::unique_ptr<Uniform> &&ubo);

        void appendSSBO(uint32_t bindingExpected, VkShaderStageFlags useStage,  std::unique_ptr<ShaderStorage> &&ssbo);

        void appendCombinedImgSampler(uint32_t bindingExpected, VkShaderStageFlags useStage,
                                       std::unique_ptr<CombinedImageSampler> &&sampler);

        void appendInputAttachment(uint32_t bindingExpected, VkShaderStageFlags useStage);

        void appendStorageImage(uint32_t bindingExpected, VkShaderStageFlags useStage);
    };


} // namespace merutilm::vkh
