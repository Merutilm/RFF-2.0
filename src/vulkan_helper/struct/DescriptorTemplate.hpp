//
// Created by Merutilm on 2025-08-10.
//

#pragma once
#include "DescriptorTemplateInfo.hpp"
#include "../manage/DescriptorManager.hpp"

namespace merutilm::vkh {
    struct DescriptorTemplate;

    template<typename T>
    concept DescTemplateHasID =
            std::is_base_of_v<DescriptorTemplate, T> &&
            std::default_initializable<T> &&
            requires
            {
                { T::ID } -> std::convertible_to<uint32_t>;
            };

    struct DescriptorTemplate {
        virtual ~DescriptorTemplate() = default;

        virtual std::unique_ptr<DescriptorManager> create(const Core &core) = 0;

        template<DescTemplateHasID D>
        static DescriptorTemplateInfo from() {
            return DescriptorTemplateInfo{
                .id = D::ID,
                .descriptorGenerator = [](const Core &core) {
                    auto instance = D();
                    return instance.create(core);
                }
            };
        }
    };
}
