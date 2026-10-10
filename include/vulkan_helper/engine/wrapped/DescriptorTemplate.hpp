//
// Created by Merutilm on 2025-08-10.
//

#pragma once
#include <vulkan_helper/engine/wrapped/DescriptorTemplateInfo.hpp>
#include <vulkan_helper/engine/manage/DescriptorManager.hpp>

#include "vulkan_helper/engine/SharedResource.hpp"

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

        virtual void configure(Core &core, SharedResource &sharedResource, std::vector<DescriptorManager> &managers) = 0;

        template<DescTemplateHasID D>
        static DescriptorTemplateInfo from() {
            return DescriptorTemplateInfo{
                .id = D::ID,
                .descriptorGenerator = [](Core & core, SharedResource &sharedResource) {
                    std::vector<DescriptorManager> managers = {};
                    auto instance = D();
                    instance.configure(core, sharedResource, managers);
                    return managers;
                }
            };
        }
    };
}
