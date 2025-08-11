//
// Created by Merutilm on 2025-07-12.
//

#pragma once
#include "Core.hpp"
#include "../handle/CoreHandler.hpp"
#include "../manage/DescriptorManager.hpp"


namespace merutilm::vkh {
    class DescriptorSetLayout final : public CoreHandler {
        const DescriptorSetLayoutBuilder &layoutBuilder;
        VkDescriptorSetLayout layout = nullptr;

    public:
        explicit DescriptorSetLayout(const Core &core, const DescriptorSetLayoutBuilder &layoutBuilder);

        ~DescriptorSetLayout() override;

        DescriptorSetLayout(const DescriptorSetLayout &) = delete;

        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

        DescriptorSetLayout(DescriptorSetLayout &&) = delete;

        DescriptorSetLayout &operator=(DescriptorSetLayout &&) = delete;

        [[nodiscard]] VkDescriptorSetLayout getLayoutHandle() const {return layout;}

    private:
        void init() override;

        void destroy() override;
    };
}
