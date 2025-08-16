//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <variant>
#include <vector>

#include "../impl/CombinedImageSampler.hpp"
#include "../impl/Uniform.hpp"
#include "../hash/DescriptorSetLayoutBuildTypeHasher.hpp"
#include "../hash/VectorHasher.hpp"
#include "../struct/DescriptorSetLayoutBuildType.hpp"
#include "../struct/StringHasher.hpp"
#include "../context/ImageContext.hpp"
#include "../impl/ShaderStorage.hpp"

namespace merutilm::vkh {
    using DescriptorSetLayoutBuilder = std::vector<DescriptorSetLayoutBuildType>;
    using DescriptorSetLayoutBuilderHasher = VectorHasher<DescriptorSetLayoutBuildType,
        DescriptorSetLayoutBuildTypeHasher>;

    using DescriptorType = std::variant<std::unique_ptr<Uniform>, std::unique_ptr<ShaderStorage>, std::unique_ptr<
        CombinedImageSampler>, ImageContext>;


    class DescriptorManagerImpl {
        std::vector<DescriptorType> data = {};
        DescriptorSetLayoutBuilder layoutBuilder = {};

    public:
        explicit DescriptorManagerImpl() = default;

        ~DescriptorManagerImpl() = default;

        DescriptorManagerImpl(const DescriptorManagerImpl &) = delete;

        DescriptorManagerImpl &operator=(const DescriptorManagerImpl &) = delete;

        DescriptorManagerImpl(DescriptorManagerImpl &&) noexcept = delete;

        DescriptorManagerImpl &operator=(DescriptorManagerImpl &&) noexcept = delete;

        template<typename T>
        uint32_t getElementCount() const {
            uint32_t count = 0;
            for (auto &variant: data) {
                if (std::holds_alternative<T>(variant)) ++count;
            }
            return count;
        }

        uint32_t getElements() const {
            return static_cast<uint32_t>(data.size());
        }


        void appendUBO(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
                       std::unique_ptr<Uniform> &&ubo) {
            SafeArrayChecker::checkIndexEqual(bindingExpected, static_cast<uint32_t>(data.size()),
                                              "Descriptor UBO add");
            data.emplace_back(std::move(ubo));
            layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, useStage);
        }

        void appendSSBO(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
                        std::unique_ptr<ShaderStorage> &&ssbo) {
            SafeArrayChecker::checkIndexEqual(bindingExpected, static_cast<uint32_t>(data.size()),
                                              "Descriptor SSBO add");
            data.emplace_back(std::move(ssbo));
            layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, useStage);
        }

        void appendCombinedImgSampler(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
                                      std::unique_ptr<CombinedImageSampler> &&sampler) {
            SafeArrayChecker::checkIndexEqual(bindingExpected, static_cast<uint32_t>(data.size()),
                                              "Descriptor Sampler add");
            data.emplace_back(std::move(sampler));
            layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, useStage);
        }

        void appendInputAttachment(const uint32_t bindingExpected, const VkShaderStageFlags useStage) {
            SafeArrayChecker::checkIndexEqual(bindingExpected, static_cast<uint32_t>(data.size()),
                                              "Descriptor Input Attachment add");
            data.emplace_back(ImageContext{});
            layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, useStage);
        }


        const DescriptorType &getRaw(
            const uint32_t bindingIndex) {
            if (bindingIndex >= data.size()) {
                throw exception_invalid_args("bindingIndex out of range");
            }
            return data[bindingIndex];
        }


        template<typename T>
        T &get(const uint32_t bindingIndex) {
            if (bindingIndex >= data.size()) {
                throw exception_invalid_args("binding out of range");
            }
            return std::get<T>(data[bindingIndex]);
        }


        [[nodiscard]] const DescriptorSetLayoutBuilder &getLayoutBuilder() const { return layoutBuilder; }
    };

    using DescriptorManager = std::unique_ptr<DescriptorManagerImpl>;
    using DescriptorManagerPtr = DescriptorManagerImpl *;
    using DescriptorManagerRef = DescriptorManagerImpl &;
}
