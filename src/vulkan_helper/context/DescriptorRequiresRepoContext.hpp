//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "../repo/DescriptorSetLayoutRepo.hpp"
#include "../repo/SamplerRepo.hpp"

namespace merutilm::vkh {
    struct DescriptorRequiresRepoContext {
        DescriptorSetLayoutRepo &layoutRepo;
        SamplerRepo &samplerRepo;
    };
}
