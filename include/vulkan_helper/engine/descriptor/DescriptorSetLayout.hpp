//
// Created by Merutilm on 2025-07-12.
//

#pragma once
#include <vulkan_helper/core/Core.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>
#include <vulkan_helper/engine/manage/DescriptorManager.hpp>


namespace merutilm::vkh {
    class DescriptorSetLayout final : public CoreHandler {
        DescriptorSetLayoutBuilder layoutBuilder;
        VkDescriptorSetLayout layout = nullptr;

    public:
        explicit DescriptorSetLayout(Core & core, const DescriptorSetLayoutBuilder &layoutBuilder);

        ~DescriptorSetLayout() override;

        DescriptorSetLayout(const DescriptorSetLayout &) = delete;

        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

        DescriptorSetLayout(DescriptorSetLayout &&) = delete;

        DescriptorSetLayout &operator=(DescriptorSetLayout &&) = delete;

        [[nodiscard]] VkDescriptorSetLayout getLayoutHandle() const {return layout;}

        [[nodiscard]] const DescriptorSetLayoutBuilder &getBuilder() const {return layoutBuilder;}

    private:
        void init() override;

        void cleanup() override;
    };

    

}
