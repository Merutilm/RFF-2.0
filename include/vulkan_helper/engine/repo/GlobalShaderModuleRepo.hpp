//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/pipeline/ShaderModule.hpp>
#include <vulkan_helper/hash/StringHasher.hpp>
#include "Repository.hpp"

namespace merutilm::vkh {
    struct GlobalShaderModuleRepo final : Repository<std::string, const std::string &, std::unique_ptr<ShaderModule>, ShaderModule &,
                StringHasher, std::equal_to<> > {
        using Repository::Repository;


        ShaderModule & pick(const std::string &filename) override;
    };
}
