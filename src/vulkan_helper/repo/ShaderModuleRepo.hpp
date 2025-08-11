//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../def/ShaderModule.hpp"
#include "../struct/StringHasher.hpp"

namespace merutilm::vkh {
    struct ShaderModuleRepo final : Repo<std::string, const std::string&, ShaderModule, StringHasher, std::equal_to<>>{

        using Repo::Repo;


        const ShaderModule &pick(const std::string &filename) override {
            return *repository.try_emplace(filename, std::make_unique<ShaderModule>(core, filename)).first->second;
        }

    };
}
