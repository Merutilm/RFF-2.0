//
// Created by Merutilm on 2025-07-12.
//

#pragma once
#include "Core.hpp"
#include "../handle/CoreHandler.hpp"
#include "../manage/DescriptorManager.hpp"


namespace merutilm::vkh {
    class DescriptorSetLayoutImpl final : public CoreHandler {
        DescriptorSetLayoutBuilder layoutBuilder;
        VkDescriptorSetLayout layout = nullptr;

    public:
        explicit DescriptorSetLayoutImpl(const CoreRef core, const DescriptorSetLayoutBuilder &layoutBuilder);

        ~DescriptorSetLayoutImpl() override;

        DescriptorSetLayoutImpl(const DescriptorSetLayoutImpl &) = delete;

        DescriptorSetLayoutImpl &operator=(const DescriptorSetLayoutImpl &) = delete;

        DescriptorSetLayoutImpl(DescriptorSetLayoutImpl &&) = delete;

        DescriptorSetLayoutImpl &operator=(DescriptorSetLayoutImpl &&) = delete;

        [[nodiscard]] VkDescriptorSetLayout getLayoutHandle() const {return layout;}

        [[nodiscard]] const DescriptorSetLayoutBuilder &getBuilder() const {return layoutBuilder;}

    private:
        void init() override;

        void destroy() override;
    };

    
    using DescriptorSetLayout = std::unique_ptr<DescriptorSetLayoutImpl>;
    using DescriptorSetLayoutPtr = DescriptorSetLayoutImpl *;
    using DescriptorSetLayoutRef = DescriptorSetLayoutImpl &;
}
