//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <vulkan_helper/engine/descriptor/Descriptor.hpp>
#include <vulkan_helper/engine/wrapped/DescriptorTemplateInfo.hpp>
#include "GlobalDescriptorSetLayoutRepo.hpp"
#include "Repository.hpp"

namespace merutilm::vkh {
    struct WindowLocalDescriptorRepo final : Repository<uint32_t, const DescriptorTemplateInfo &, std::unique_ptr<Descriptor>, Descriptor &,
                std::hash<uint32_t>, std::equal_to<>, SharedResource &, GlobalDescriptorSetLayoutRepo &> {
        using Repository::Repository;

        Descriptor & pick(const DescriptorTemplateInfo &descTemplateInfo, SharedResource &sharedResource,
                           GlobalDescriptorSetLayoutRepo &layoutRepo) override;
    };
}
