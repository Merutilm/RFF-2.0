//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <variant>
#include <vector>

#include "../def/CombinedImageSampler.hpp"
#include "../def/Uniform.hpp"
#include "../hash/DescriptorSetLayoutBuildTypeHasher.hpp"
#include "../hash/VectorHasher.hpp"
#include "../struct/DescriptorSetLayoutBuildType.hpp"
#include "../struct/StringHasher.hpp"
#include "../context/ImageContext.hpp"

namespace merutilm::vkh {
    using DescriptorSetLayoutBuilder = std::vector<DescriptorSetLayoutBuildType>;
    using DescriptorSetLayoutBuilderHasher = VectorHasher<DescriptorSetLayoutBuildType,
        DescriptorSetLayoutBuildTypeHasher>;

    using DescriptorType = std::variant<std::unique_ptr<Uniform>, std::unique_ptr<CombinedImageSampler>,  ImageContext >;

    class DescriptorManager {

        std::vector<DescriptorType> data = {};
        DescriptorSetLayoutBuilder layoutBuilder = {};

    public:
        explicit DescriptorManager() = default;

        ~DescriptorManager() = default;

        DescriptorManager(const DescriptorManager &) = delete;

        DescriptorManager &operator=(const DescriptorManager &) = delete;

        DescriptorManager(DescriptorManager &&) noexcept = delete;

        DescriptorManager &operator=(DescriptorManager &&) noexcept = delete;

        void appendUBO(uint32_t bindingExpected, VkShaderStageFlags useStage, std::unique_ptr<Uniform> &&ubo);

        void appendCombinedImgSampler(uint32_t bindingExpected, VkShaderStageFlags useStage,
                                      std::unique_ptr<CombinedImageSampler> &&sampler);

        void appendInputAttachment(uint32_t bindingExpected, VkShaderStageFlags useStage);

        const DescriptorSetLayoutBuilder &getLayoutBuilder() const { return layoutBuilder; }

        template<typename T>
        [[nodiscard]] uint32_t getElementCount() const;

        uint32_t getElements() const;

        const DescriptorType &getRaw(uint32_t bindingIndex);

        template<typename T>
        [[nodiscard]] T &get(uint32_t bindingIndex);
    };

    template<typename T>
    uint32_t DescriptorManager::getElementCount() const {
        uint32_t count = 0;
        for (auto &variant: data) {
            if (std::holds_alternative<T>(variant)) ++count;
        }
        return count;
    }

    inline uint32_t DescriptorManager::getElements() const {
        return static_cast<uint32_t>(data.size());
    }


    inline void DescriptorManager::appendUBO(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
                                             std::unique_ptr<Uniform> &&ubo) {

        IndexChecker::checkIndexEqual(bindingExpected, static_cast<uint32_t>(data.size()), "Descriptor UBO add");
        data.emplace_back(std::move(ubo));
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, useStage);
    }

    inline void DescriptorManager::appendCombinedImgSampler(const uint32_t bindingExpected, const VkShaderStageFlags useStage,
                                                            std::unique_ptr<CombinedImageSampler> &&sampler) {

        IndexChecker::checkIndexEqual(bindingExpected,  static_cast<uint32_t>(data.size()), "Descriptor Sampler add");
        data.emplace_back(std::move(sampler));
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, useStage);
    }

    inline void DescriptorManager::appendInputAttachment(const uint32_t bindingExpected, const VkShaderStageFlags useStage) {
        IndexChecker::checkIndexEqual(bindingExpected,  static_cast<uint32_t>(data.size()), "Descriptor Input Attachment add");
        data.emplace_back(ImageContext{});
        layoutBuilder.emplace_back(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, useStage);
    }


    inline const DescriptorType &DescriptorManager::getRaw(
        const uint32_t bindingIndex) {
        if (bindingIndex >= data.size()) {
            throw exception_invalid_args("bindingIndex out of range");
        }
        return data[bindingIndex];
    }


    template<typename T>
    T &DescriptorManager::get(const uint32_t bindingIndex) {
        if (bindingIndex >= data.size()) {
            throw exception_invalid_args("binding out of range");
        }
        return std::get<T>(data[bindingIndex]);
    }

}
